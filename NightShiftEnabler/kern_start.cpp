//
//  kern_start.cpp
//  NSE
//
//  Copyright © 2021 cdf. All rights reserved.
//

#include <Headers/plugin_start.hpp>
#include <Headers/kern_api.hpp>

#include "kern_nse.hpp"

static NSE nse;

const char *bootargOff[] {
    "-nseoff"
};

const char *bootargDebug[] {
    "-nsedbg"
};

const char *bootargBeta[] {
    "-nsebeta"
};

PluginConfiguration ADDPR(config) {
    xStringify(PRODUCT_NAME),
    parseModuleVersion(xStringify(MODULE_VERSION)),
    LiluAPI::AllowNormal,
    bootargOff,
    arrsize(bootargOff),
    bootargDebug,
    arrsize(bootargDebug),
    bootargBeta,
    arrsize(bootargBeta),
    KernelVersion::Sierra,
    KernelVersion::BigSur,
    []() {
        nse.init();
    }
};
