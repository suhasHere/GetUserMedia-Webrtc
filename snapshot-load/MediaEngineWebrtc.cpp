/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MediaEngineWebrtc.h"

#define CHANNELS 1

namespace mozilla {

/**
 * Webrtc video source.
 */

NS_IMPL_THREADSAFE_ISUPPORTS1(MediaEngineWebrtcVideoSource, nsITimerCallback)

/*
 * Shutdown video renderer and capture threads Async
 */
class AsyncShutdownThread : public nsRunnable
{
public:
  AsyncShutdownThread(nsIThread* aThread) : mThread(aThread) {}
  NS_IMETHODIMP Run() 
  { 
  	return mThread->Shutdown(); 
  }
private:
  nsCOMPtr<nsIThread> mThread;
};



/**
 * Runnable to start the capture device
 */
class VideoStartCaptureEvent: public nsRunnable
{
public:
  VideoStartCaptureEvent(MediaEngineWebrtcVideoSource* aOwner)
  {
    mOwner = aOwner;
  }

  NS_IMETHOD Run()
  {
  	int error = mOwner->mViECapture->StartCapture(mOwner->mCapIndex);
  	if (-1 == error ) 
	{
    	return NS_ERROR_FAILURE;
  	}
    printf("\n Started Capture  RETURNED ");
  	return NS_OK; 
  }

  MediaEngineWebrtcVideoSource* mOwner;
};

/**
 * Runnable to write the video frames to MediaStream 
 */
class VideoRenderToStreamEvent : public nsRunnable
{

public:
  VideoRenderToStreamEvent(MediaEngineWebrtcVideoSource* aOwner,
						   unsigned char* aBuffer, 
						   int aSize, 
						   uint32_t aTimestamp,
						   int64_t aRenderTime)
  {
    mOwner = aOwner;
  	mBuffer = aBuffer;
    mSize = aSize;
    mTimestamp = aTimestamp;
    mRenderTime = aRenderTime;
  }

  NS_IMETHOD Run()
  {
  	 ReentrantMonitorAutoEnter enter(mOwner->mMonitor);
 	 // create a VideoFrame and push it to the input stream;
  	layers::Image::Format format = layers::Image::PLANAR_YCBCR;
  	nsRefPtr<layers::Image> image = mOwner->mImageContainer->CreateImage(&format, 1);
  	image->AddRef();
  	layers::PlanarYCbCrImage* videoImage = static_cast<mozilla::layers::PlanarYCbCrImage*> (image.get());
  	PRUint8* frame = static_cast<PRUint8*> (mBuffer);
  	const PRUint8 lumaBpp = 8;
  	const PRUint8 chromaBpp = 4;

  	layers::PlanarYCbCrImage::Data data;
  	data.mYChannel = frame;
  	data.mYSize = gfxIntSize(mOwner->mWidth, mOwner->mHeight);
  	data.mYStride = mOwner->mWidth * lumaBpp/ 8.0;
  	data.mCbCrStride = mOwner->mWidth * chromaBpp / 8.0;
  	data.mCbChannel = frame + mOwner->mHeight * data.mYStride;
  	data.mCrChannel = data.mCbChannel + mOwner->mHeight * data.mCbCrStride / 2;
  	data.mCbCrSize = gfxIntSize(mOwner->mWidth/ 2, mOwner->mHeight/ 2);
  	data.mPicX = 0;
  	data.mPicY = 0;
  	data.mPicSize = gfxIntSize(mOwner->mWidth, mOwner->mHeight);
  	data.mStereoMode = layers::STEREO_MODE_MONO;

  	videoImage->SetData(data);

  	mOwner->mVideoSegment.AppendFrame(videoImage,1, gfxIntSize(mOwner->mWidth, mOwner->mHeight));
  	mOwner->mSource->AppendToTrack(mOwner->mTrackID, &(mOwner->mVideoSegment));
	return NS_OK;
  }


  MediaEngineWebrtcVideoSource* mOwner;
  unsigned char* mBuffer;
  int mSize;
  uint32_t mTimestamp;
  int64_t mRenderTime;
};

//static initialization - video source
const unsigned int MediaEngineWebrtcVideoSource::KMaxDeviceNameLength = 128;
const unsigned int MediaEngineWebrtcVideoSource::KMaxUniqueIdLength = 256;

// Webrtc_External Renderer Implementation
int
MediaEngineWebrtcVideoSource::FrameSizeChange(
  	unsigned int w, unsigned int h, unsigned int streams)
{
  mWidth = w;
  mHeight = h;
  return 0;
}

// Webrtc_External Renderer - video frame callback
int
MediaEngineWebrtcVideoSource::DeliverFrame(
  	unsigned char* buffer, int size, uint32_t time_stamp, int64_t render_time)
{
 if(mInSnapshotMode)
 {
   	// while taking snapshot we don't process frames
  	return 0;
 }

 // let our rendering thread deal with the media-streams.
 nsCOMPtr<nsIRunnable> event = new VideoRenderToStreamEvent(this, buffer, size, time_stamp, render_time); 
 mVideoRenderingThread->Dispatch(event,0);
 return 0; 
}


/**
 * Acquires the basic interfaces for the Video Engine 
 */

void
MediaEngineWebrtcVideoSource::Init()
{

  if(NULL == mVideoEngine)
  	return;

  mViEBase = webrtc::ViEBase::GetInterface(mVideoEngine);
  if (NULL == mViEBase ) 
  {
    printf( "ERROR in VideoEngine::ViEBase ");
    return;
  }

  //get interfaces for capture, render for now
  mViECapture = webrtc::ViECapture::GetInterface(mVideoEngine);
  mViERender = webrtc::ViERender::GetInterface(mVideoEngine);

  if(NULL == mViECapture || NULL == mViERender) 
  	return ;

  //temporary logging 
  mVideoEngine->SetTraceFilter(webrtc::kTraceAll);
  mVideoEngine->SetTraceFile( "Vievideotrace.out" );


  // we should be good here 
  mInitDone = true;
}

void
MediaEngineWebrtcVideoSource::GetName(nsAString& aName)
{

  char deviceName[KMaxDeviceNameLength];
  memset(deviceName, 0, KMaxDeviceNameLength);
  char uniqueId[KMaxUniqueIdLength];
  memset(uniqueId, 0, KMaxUniqueIdLength);

  if(true == mInitDone )
  {
  	mViECapture->GetCaptureDevice(
    	mCapIndex, deviceName, KMaxDeviceNameLength, uniqueId, KMaxUniqueIdLength
  	);

   aName.Assign(NS_ConvertASCIItoUTF16(deviceName));

  }
}

void
MediaEngineWebrtcVideoSource::GetUUID(nsAString& aUUID)
{
  char deviceName[KMaxDeviceNameLength];
  memset(deviceName, 0, KMaxDeviceNameLength);
  char uniqueId[KMaxUniqueIdLength];
  memset(uniqueId, 0, KMaxUniqueIdLength);
  if(true == mInitDone) 
  {
  	 mViECapture->GetCaptureDevice(
    		mCapIndex, deviceName, KMaxDeviceNameLength, uniqueId, KMaxUniqueIdLength
  	);

  	aUUID.Assign(NS_ConvertASCIItoUTF16(uniqueId));
  }
}

already_AddRefed<nsDOMMediaStream>
MediaEngineWebrtcVideoSource::Allocate()
{
  if (mState != kReleased) {
    return NULL;
  }

  //let's allocate the device
  char deviceName[KMaxDeviceNameLength];
  char uniqueId[KMaxUniqueIdLength];
  memset(deviceName, 0, KMaxDeviceNameLength);
  memset(uniqueId, 0, KMaxUniqueIdLength);
  mViECapture->GetCaptureDevice(
 				mCapIndex, deviceName, KMaxDeviceNameLength, uniqueId, KMaxUniqueIdLength
  	);

  if(0 == mViECapture->AllocateCaptureDevice(
    					uniqueId, KMaxUniqueIdLength, mCapIndex)) 
  {
  	mState = kAllocated;
    // start the capture on the device
  	if (!mVideoCaptureThread)
  	{
   	 NS_NewThread(getter_AddRefs(mVideoCaptureThread),
   	                     nsnull,
   	                     MEDIA_THREAD_STACK_SIZE);
  	}
    //no point in proceeding
    if(!mVideoCaptureThread)
    	return NULL;

   	nsCOMPtr<nsIRunnable> event = new VideoStartCaptureEvent(this);
  	mVideoCaptureThread->Dispatch(event,0);
   	return nsDOMMediaStream::CreateInputStream();
  }

  return NULL; 
}

nsresult
MediaEngineWebrtcVideoSource::Deallocate()
{
  if (mState != kStopped && mState != kAllocated) {
    return NS_ERROR_FAILURE;
  }

  mViECapture->StopCapture(mCapIndex);
  if(mVideoCaptureThread)
  {
      nsCOMPtr<nsIRunnable> event = new AsyncShutdownThread(mVideoCaptureThread);
      NS_DispatchToMainThread(event);
  }

  mViECapture->ReleaseCaptureDevice(mCapIndex);
  mState = kReleased;
  return NS_OK;
}

MediaEngineVideoOptions
MediaEngineWebrtcVideoSource::GetOptions()
{
  MediaEngineVideoOptions aOpts;
  aOpts.mWidth = mWidth;
  aOpts.mHeight = mHeight;
  aOpts.mMaxFPS = mFps;
  aOpts.codecType = kVideoCodecI420;
  return aOpts;
}

nsresult
MediaEngineWebrtcVideoSource::Start(SourceMediaStream* aStream, TrackID aID)
{
  if (false == mInitDone || mState != kAllocated) {
    return NULL;
  }

  if(!aStream)
  	return NULL;

  if(mState == kStarted)
    return NS_OK;

  mSource = aStream;
  mTrackID = aID;

  //setup  a blank track
  mImageContainer = layers::LayerManager::CreateImageContainer();
  mSource->AddTrack(aID, mFps, 0, new VideoSegment());
  mSource->AdvanceKnownTracksTime(STREAM_TIME_MAX);
  
  //let's start the rendering
  if (!mVideoRenderingThread)
  {
    NS_NewThread(getter_AddRefs(mVideoRenderingThread),
                        nsnull,
                        MEDIA_THREAD_STACK_SIZE);
  }
  // no point in proceeding
  if(!mVideoRenderingThread)
	return NS_ERROR_FAILURE;

  int error = mViERender->AddRenderer(mCapIndex, webrtc::kVideoI420, (webrtc::ExternalRenderer*) this);
  error = mViERender->StartRender(mCapIndex);
  if (-1 == error)
  {
        printf( "ERROR in ViERender::StartRender %d ", mViEBase->LastError());
        return NS_ERROR_FAILURE;
  }

  mState = kStarted;
  return NS_OK;
}


nsresult
MediaEngineWebrtcVideoSource::Stop()
{
  if (mState != kStarted) {
    return NS_ERROR_FAILURE;
  }


  mSource->EndTrack(mTrackID);
  mSource->Finish();

  mViERender->StopRender(mCapIndex);
  mViERender->RemoveRenderer(mCapIndex);
  
  if(mVideoRenderingThread)
  {
  	 nsCOMPtr<nsIRunnable> event = new AsyncShutdownThread(mVideoRenderingThread);
     NS_DispatchToMainThread(event);
  }
  
  mState = kStopped;
  return NS_OK;
}

nsresult
MediaEngineWebrtcVideoSource::Snapshot(PRUint32 aDuration, nsIDOMFile** aFile)
{
  printf("\n Taking Snapshot "); 
  *aFile = nsnull;

  if (false == mInitDone || mState != kAllocated) 
  {
    printf("\n Video Source not allocated / started");
    return NS_ERROR_FAILURE;
  }

  //we need to stop our capture thread for the engine to continue
  // capturing on its own thread
  if(mVideoCaptureThread) 
  {
  	mVideoCaptureThread->Shutdown();
  }

  mInSnapshotMode = true;

  sleep(1);
  //let's start the rendering - this is needed for frames to be 
  // prcessed out of capture engine
  //int error = mViERender->AddRenderer(mCapIndex, 
//				webrtc::kVideoI420, (webrtc::ExternalRenderer*) this);
//
 // error = mViERender->StartRender(mCapIndex);
  // we shud be good for snapshot now 
  webrtc::ViEFile* vieFile =  webrtc::ViEFile::GetInterface(mVideoEngine);
  if(NULL == vieFile)
  {
	return NS_ERROR_FAILURE;
  }
  printf("\n Starting to get the snapsot ");
  webrtc::ViEPicture viePicture;
  if(-1 == vieFile->GetCaptureDeviceSnapshot(mCapIndex,viePicture))
  {
  	printf("\n BOOOOO BOOOOOOO BOOOOO ");
    if(-1 == vieFile->GetCaptureDeviceSnapshot(mCapIndex,viePicture))
  	printf("\n BOOOOO BOOOOOOO BOOOOO  AGAIN");
  	  return NS_ERROR_FAILURE;
  }
#if 0
  sleep(2);
  const char snapshotCaptureDeviceFileName[256] =
            "/tmp/snapshotCaptureDevice.jpg";
  if(-1 == vieFile->GetCaptureDeviceSnapshot(mCapIndex,snapshotCaptureDeviceFileName))
  {
  	  printf("\n GetCaptureDeviceSnapshot Failed %d ", mViEBase->LastError());
  }
#endif
  nsAutoString type;
  type.AssignLiteral("image/png");
  nsRefPtr<nsDOMMemoryFile> file = 
		new nsDOMMemoryFile((void*)viePicture.data, viePicture.size, type, EmptyString());
  file.forget(aFile);


  //cleanup
  vieFile->Release(); 
  vieFile = NULL;
  // Stop and remove our renderer
  mViERender->StopRender(mCapIndex);
  mViERender->RemoveRenderer(mCapIndex);
  mInSnapshotMode = false;
  return NS_OK;
}


void
MediaEngineWebrtcVideoSource::Shutdown()
{
  int error = 0;
  bool continueShutdown = false;
  if(false == mInitDone)
    return;


  // check if there is some activity going on
  if(kStarted == mState)
  {
  	error = mViERender->StopRender(mCapIndex);
    //stop the rendering thred
    if(mVideoRenderingThread)
    {
 	  nsCOMPtr<nsIRunnable> event = new AsyncShutdownThread(mVideoRenderingThread);
      NS_DispatchToMainThread(event);
    }
    error = mViERender->RemoveRenderer(mCapIndex);
    continueShutdown = true;
  }

  if(-1 == error)
  	printf("VideoSource Shutdown: Stopping renderer failed %d ", mViEBase->LastError());

  error = 0;
  if(kAllocated == mState || continueShutdown)
  {
    error = mViECapture->StopCapture(mCapIndex);
    if(mVideoCaptureThread)
    {
 	  nsCOMPtr<nsIRunnable> event = new AsyncShutdownThread(mVideoCaptureThread);
      NS_DispatchToMainThread(event);
    }
    error = mViECapture->ReleaseCaptureDevice(mCapIndex);
    continueShutdown = false;
  }

  if(-1 == error)
  	printf("VideoSource Shutdown: Releasing capture device failed %d ", mViEBase->LastError());

  //let's clean up the interfaces now
  mViECapture->Release();
  mViERender->Release();
  mViEBase->Release();
  mState = kReleased;
  mInitDone = false;
}

//XXX: How to get rid of this ??
NS_IMETHODIMP
MediaEngineWebrtcVideoSource::Notify(nsITimer* aTimer)
{
  return NS_OK;
}



/**
 * Webrtc audio source.
 */

NS_IMPL_THREADSAFE_ISUPPORTS1(MediaEngineWebrtcAudioSource, nsITimerCallback)

//static initialization
const unsigned int MediaEngineWebrtcAudioSource::PLAYOUT_SAMPLE_FREQUENCY = 16000;
const unsigned int MediaEngineWebrtcAudioSource::PLAYOUT_SAMPLE_LENGTH  = 160;
const unsigned int MediaEngineWebrtcAudioSource::KMaxDeviceNameLength = 128;
const unsigned int MediaEngineWebrtcAudioSource::KMaxUniqueIdLength = 128;




// Performs very basic & common initialization 
void
MediaEngineWebrtcAudioSource::Init()
{
  if(NULL == mVoiceEngine)
  	return;

  mVoEBase = webrtc::VoEBase::GetInterface(mVoiceEngine);
  if (NULL == mVoEBase ) 
  {
    printf( "ERROR in AudioEngine::VoEBase ");
    return;
  }

  //get interfaces for capture, render for now
  mVoEHw = webrtc::VoEHardware::GetInterface(mVoiceEngine);

  // check if all the interfaces were ok till now
  if(NULL == mVoEHw) 
  	return ;

  mChannel = mVoEBase->CreateChannel();
  if(-1 == mChannel)
  {
	return;
  }

  mVoEXmedia = webrtc::VoEExternalMedia::GetInterface(mVoiceEngine);
  if(NULL == mVoEXmedia)
  {
    return;
  }  
  //temp logging
  mVoiceEngine->SetTraceFilter(webrtc::kTraceAll);
  mVoiceEngine->SetTraceFile( "Voevideotrace.out" );
  mAudioSegment.Init(CHANNELS);

  // we should be good by now
  mInitDone = true;

}

void
MediaEngineWebrtcAudioSource::GetName(nsAString& aName)
{
  char deviceName[KMaxDeviceNameLength];
  memset(deviceName, 0, KMaxDeviceNameLength);
  char uniqueId[KMaxUniqueIdLength];
  memset(uniqueId, 0, KMaxUniqueIdLength);
  if(true == mInitDone)
  {
    mVoEHw->GetRecordingDeviceName(
        			mCapIndex, deviceName,  uniqueId );
   aName.Assign(NS_ConvertASCIItoUTF16(deviceName));
  }

  return;
}

void
MediaEngineWebrtcAudioSource::GetUUID(nsAString& aUUID)
{
  char deviceName[KMaxDeviceNameLength];
  memset(deviceName, 0, KMaxDeviceNameLength);
  char uniqueId[KMaxUniqueIdLength];
  memset(uniqueId, 0, KMaxUniqueIdLength);
  if(true == mInitDone )
  {
    mVoEHw->GetRecordingDeviceName(
        			mCapIndex, deviceName,  uniqueId );
   aUUID.Assign(NS_ConvertASCIItoUTF16(uniqueId));
  }

  return;
}

already_AddRefed<nsDOMMediaStream>
MediaEngineWebrtcAudioSource::Allocate()
{
  if (mState != kReleased) {
    return NULL;
  }
  // no special webrtc code to be done , i hope
  mState = kAllocated;
  return nsDOMMediaStream::CreateInputStream();
}

nsresult
MediaEngineWebrtcAudioSource::Deallocate()
{
  if (mState != kStopped && mState != kAllocated) {
    return NS_ERROR_FAILURE;
  }
  mState = kReleased;
  return NS_OK;
}

//Loop back audio through media-stream
nsresult
MediaEngineWebrtcAudioSource::Start(SourceMediaStream* aStream, TrackID aID)
{
  const int DEFAULT_PORT = 55555;
  printf("\n MediaEngineWebrtcAudioSource : Start: Entered ");
  if (false == mInitDone || mState != kAllocated) {
    return NULL;
  }

  if(!aStream)
  	return NULL;

  mTimer = do_CreateInstance(NS_TIMER_CONTRACTID);
  if (!mTimer) {
    return NULL;
  }

  mSource = aStream;

  AudioSegment* segment = new AudioSegment();
  segment->Init(CHANNELS);
  //segment->InsertNullDataAtStart(1);
  mSource->AddTrack(aID, PLAYOUT_SAMPLE_FREQUENCY, 0, segment);
  mSource->AdvanceKnownTracksTime(STREAM_TIME_MAX);
  mTrackID = aID;

  printf("\n Starting the audio engine ");
  mVoEBase->SetLocalReceiver(mChannel,DEFAULT_PORT);
  mVoEBase->SetSendDestination(mChannel,DEFAULT_PORT,"127.0.0.1");

  if(-1 == mVoEXmedia->SetExternalPlayoutStatus(true)) {
    printf("\n SetExternalPlayoutStatus failed %d ", mVoEBase->LastError() );
  	return NULL;
  } 
  //loopback audio
  mVoEBase->StartPlayout(mChannel);
  mVoEBase->StartSend(mChannel);
  mVoEBase->StartReceive(mChannel);
   
  mState = kStarted;
  // call every 10 milliseconds
  mTimer->InitWithCallback(this, 10, nsITimer::TYPE_REPEATING_SLACK);
  return NS_OK;
}

nsresult
MediaEngineWebrtcAudioSource::Stop()
{
  if (mState != kStarted) {
    return NS_ERROR_FAILURE;
  }
  if (!mTimer) {
    return NS_ERROR_FAILURE;
  }

  if(!mVoEBase) {
    return NS_ERROR_FAILURE;
  }
  
  mVoEBase->StopReceive(mChannel);
  mVoEBase->StopSend(mChannel);
  mVoEBase->StopPlayout(mChannel); 

  mTimer->Cancel();
  mTimer = NULL;
  mState = kStopped;
  return NS_OK;
}

nsresult
MediaEngineWebrtcAudioSource::Snapshot(PRUint32 aDuration, nsIDOMFile** aFile)
{
   return NS_ERROR_NOT_IMPLEMENTED;
}


void
MediaEngineWebrtcAudioSource::Shutdown()
{
  if(false == mInitDone)
  	return;

  if(kStarted == mState)
  {
  	//Stop External playout
  	mVoEBase->StopReceive(mChannel);
  	mVoEBase->StopSend(mChannel);
  	mVoEBase->StopPlayout(mChannel);
  	mTimer->Cancel();
  	mTimer = NULL;
  }

  mVoEBase->Terminate();
  mVoEXmedia->Release();
  mVoEHw->Release();
  mVoEBase->Release();
  mState = kReleased;
  mInitDone = false;
 
}


NS_IMETHODIMP
MediaEngineWebrtcAudioSource::Notify(nsITimer* aTimer)
{
  // just one audio sample
  static int16_t audio10ms[PLAYOUT_SAMPLE_LENGTH];
  int sample_length  =0;
  memset(audio10ms, 0, PLAYOUT_SAMPLE_LENGTH * sizeof(short));

  mVoEXmedia->ExternalPlayoutGetData(audio10ms, PLAYOUT_SAMPLE_FREQUENCY, 100, sample_length); 
  if(sample_length == 0)
  {
	return NS_OK;
  }

  // allocate shared buffer of lenght bytes
  int buff_size = PLAYOUT_SAMPLE_LENGTH * sizeof(short);
  nsRefPtr<SharedBuffer> buffer = SharedBuffer::Create(buff_size);
  //buffer->AddRef();
  int16_t* dest = static_cast<int16_t*>(buffer->Data());
  for(int i=0; i < sample_length; i++)
  {
	dest[i] = audio10ms[i];
  }

  mAudioSegment.AppendFrames(buffer.forget(), sample_length, 0, sample_length, nsAudioStream::FORMAT_S16_LE);
  mSource->AppendToTrack(mTrackID, &mAudioSegment);
  return NS_OK;
}

//XXX: Not multi-threded and non-renterant -???
void
MediaEngineWebrtc::EnumerateVideoDevices(nsTArray<nsRefPtr<MediaEngineVideoSource> >* aVSources)
{
  int error = 0;
  printf("\n EnumerateVideo Devices ");

  //we can use it function locals .. no need to save it in the class
  webrtc::ViEBase* ptrViEBase;
  webrtc::ViECapture* ptrViECapture;
  
  if(NULL == mVideoEngine)
  {
      mVideoEngine = webrtc::VideoEngine::Create();
      if (NULL == mVideoEngine ) 
	{
         printf( "ERROR in VideoEngine::Create\n");
    	   return;
     }
  }

  // te should have VideoEngine Created here
  ptrViEBase = webrtc::ViEBase::GetInterface(mVideoEngine);
  if (NULL == ptrViEBase) {
    return;
  }


  if( false == mVideoEngineInit)
  {
  	error = ptrViEBase->Init();
  	if (-1 == error ) {
   	 printf( "ERROR in VideoEngine::Init\n");
   	 return;
  	}
    mVideoEngineInit = true;
  }

  ptrViECapture = webrtc::ViECapture::GetInterface(mVideoEngine);
  if (NULL == ptrViECapture ) {
    printf("ERROR in ViECapture::GetInterface\n");
    return;
  }

  int num = ptrViECapture->NumberOfCaptureDevices();
  if (num <= 0) {
    printf( "ERROR no video devices found\n");
    return;
  } else {
    printf("GetUserMedia:: Found %d devices!\n", num);
  }
 
  for(int i=0; i < num; i++)
  {
    //let's create VideoSouce objects
    nsRefPtr<MediaEngineVideoSource> vSource = new MediaEngineWebrtcVideoSource(mVideoEngine , i);
    aVSources->AppendElement( vSource ); 
  }

  // safe to release local interfaces on the engine
   ptrViEBase->Release();
   ptrViECapture->Release();

  return;
}

void
MediaEngineWebrtc::EnumerateAudioDevices(nsTArray<nsRefPtr<MediaEngineAudioSource> >* aASources)
{
  
  // Be sure to release these at the end of the function
  // safety measure though
  webrtc::VoEBase* ptrVoEBase = NULL;
  webrtc::VoEHardware* ptrVoEHw = NULL;
  int error = 0; 
 
  if(NULL == mVoiceEngine)
  {
    mVoiceEngine = webrtc::VoiceEngine::Create();
    if(NULL == mVoiceEngine)
    {	
	printf(" Unable to create voice engine ");
	return;
    }
  }  

  // all interfaces pointers are ref-counted 
  // we should be good on mutiple calls to this function ..
  ptrVoEBase = webrtc::VoEBase::GetInterface( mVoiceEngine );
  if(NULL == ptrVoEBase)
  {
	printf(" VoEBase creation failed ");
	return;
  }   

  //Init the voice library for the common parts
  if(false == mAudioEngineInit)
  {
	error = ptrVoEBase->Init();
	if(-1 == error)
	{
		printf("\n Audio Engine Init Failed ");
	}
	mAudioEngineInit = true;
  } 

  ptrVoEHw = webrtc::VoEHardware::GetInterface(mVoiceEngine);
  if(NULL == ptrVoEHw)
  {
	printf("\n Unable to get Audio Hardware Interface ");
    return;
  }

  //let's enumerate audio input devices
  int nDevices = 0;
  error = ptrVoEHw->GetNumOfRecordingDevices(nDevices);
  printf("\n Number if devices is %d ",nDevices);
  for(int i = 0; i < nDevices; i++) 
  {
   	char deviceName[128];
  	memset(deviceName, 0, 128);
  	char uniqueId[128];
  	memset(uniqueId, 0, 128);
   	ptrVoEHw->GetRecordingDeviceName(i, deviceName,  uniqueId );
        nsRefPtr<MediaEngineAudioSource> aSource = new MediaEngineWebrtcAudioSource(mVoiceEngine , i);
	aSource->AddRef();
  	aASources->AppendElement( aSource );
  } 

  // releasing them to re-set the reference count ... 
  ptrVoEHw->Release(); 
  ptrVoEBase->Release();
}


void
MediaEngineWebrtc::Shutdown()
{

  if(mVideoEngine)
    webrtc::VideoEngine::Delete(mVideoEngine);

  if(mVoiceEngine)
   webrtc::VoiceEngine::Delete(mVoiceEngine);

  mVideoEngine = NULL;
  mVoiceEngine = NULL;
}

}
