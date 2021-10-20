//
//  kern_nse.cpp
//  NSE
//
//  Copyright © 2021 cdf. All rights reserved.
//

#include <Headers/kern_api.hpp>

#include "kern_nse.hpp"

NSE *NSE::callbackNSE;

void NSE::init() {
    callbackNSE = this;
    
    if (getKernelVersion() == KernelVersion::Sierra && getKernelMinorVersion() < 4)
        return;
    
    lilu.onPatcherLoadForce([](void *user, KernelPatcher &patcher) {
            static_cast<NSE *>(user)->processKernel(patcher);
        }, this);
}

void NSE::PatchModelMinVersionBLR(const void *data, size_t size, const char (&path)[PATH_MAX]) {
    if (UserPatcher::matchSharedCachePath(path)) {
        
        if ((getKernelVersion() == KernelVersion::HighSierra && getKernelMinorVersion() >= 2) || getKernelVersion() >= KernelVersion::Mojave) {
            static const uint8_t find[28] = {
                0x09, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
                0x05, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00
            };
            static const uint8_t repl[28] = {
                0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
                0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
            };
            if (UNLIKELY(KernelPatcher::findAndReplace(const_cast<void *>(data), size, find, sizeof(find), repl, sizeof(repl))))
                DBGLOG("nse", "patched Night Shift");
            
        } else {
            static const uint8_t find[24] = {
                0x09, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
                0x06, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00
            };
            static const uint8_t repl[24] = {
                0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
                0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
            };
            if (UNLIKELY(KernelPatcher::findAndReplace(const_cast<void *>(data), size, find, sizeof(find), repl, sizeof(repl))))
                DBGLOG("nse", "patched Night Shift");
        }
    }
}

void NSE::csValidateRange(vnode *vp, memory_object_t pager, memory_object_offset_t offset, const void *data, vm_size_t size, unsigned *result) {
    FunctionCast(csValidateRange, callbackNSE->orgCsValidate)(vp, pager, offset, data, size, result);

    char path[PATH_MAX];
    int pathlen = PATH_MAX;
    if (vn_getpath(vp, path, &pathlen) == 0)
        PatchModelMinVersionBLR(data, size, path);
}

void NSE::csValidatePage(vnode *vp, memory_object_t pager, memory_object_offset_t offset, const void *data, int *validated_p, int *tainted_p, int *nx_p) {
    FunctionCast(csValidatePage, callbackNSE->orgCsValidate)(vp, pager, offset, data, validated_p, tainted_p, nx_p);

    char path[PATH_MAX];
    int pathlen = PATH_MAX;
    if (vn_getpath(vp, path, &pathlen) == 0)
        PatchModelMinVersionBLR(data, PAGE_SIZE, path);
}

void NSE::processKernel(KernelPatcher &patcher) {
    DBGLOG("nse", "activating");
    
    if (getKernelVersion() >= KernelVersion::BigSur) {
        KernelPatcher::RouteRequest csRoute("_cs_validate_page", csValidatePage, orgCsValidate);
        if (!patcher.routeMultipleLong(KernelPatcher::KernelID, &csRoute, 1))
            SYSLOG("nse", "failed to route cs validation pages");
    
    } else {
        KernelPatcher::RouteRequest csRoute("_cs_validate_range", csValidateRange, orgCsValidate);
        if (!patcher.routeMultipleLong(KernelPatcher::KernelID, &csRoute, 1))
            SYSLOG("nse", "failed to route cs validation pages");
    }
}

void NSE::deinit() {
    
}
