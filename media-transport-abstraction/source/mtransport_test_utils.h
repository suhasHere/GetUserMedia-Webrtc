/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

// Original author: ekr@rtfm.com

#ifndef mtransport_test_utils_h__
#define mtransport_test_utils_h__

#include "nsCOMPtr.h"
#include "nsNetCID.h"
#include "nsXPCOMGlue.h"
#include "nsXPCOM.h"

#include "nsIComponentManager.h"
#include "nsIComponentRegistrar.h"
#include "nsIIOService.h"
#include "nsIServiceManager.h"
#include "nsISocketTransportService.h"

#include "nsServiceManagerUtils.h"


class MtransportTestUtils {
 public:
  bool InitServices() {
    nsresult rv;

    NS_InitXPCOM2(getter_AddRefs(servMan_), nsnull, nsnull);
    manager_ = do_QueryInterface(servMan_);
    rv = manager_->CreateInstanceByContractID(NS_IOSERVICE_CONTRACTID,
                                             nsnull, NS_GET_IID(nsIIOService),
                                              getter_AddRefs(ioservice_));
    if (!NS_SUCCEEDED(rv))
      return false;

    sts_target_ = do_GetService(NS_SOCKETTRANSPORTSERVICE_CONTRACTID, &rv);
    if (!NS_SUCCEEDED(rv))
      return false;
    
    return true;
  }

  nsCOMPtr<nsIEventTarget> sts_target() { return sts_target_; }
  
 private:
  nsCOMPtr<nsIServiceManager> servMan_;
  nsCOMPtr<nsIComponentManager> manager_;
  nsCOMPtr<nsIIOService> ioservice_;
  nsCOMPtr<nsIEventTarget> sts_target_;
};


#endif

