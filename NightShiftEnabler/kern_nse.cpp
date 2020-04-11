//
//  kern_nse.cpp
//  NSE
//
//  Copyright © 2020 cdf. All rights reserved.
//

#include <Headers/kern_api.hpp>

#include "kern_nse.hpp"

static const char *coreBrightness = "/System/Library/PrivateFrameworks/CoreBrightness.framework/Versions/A/CoreBrightness";

static const char *coreBrightnessD = "/usr/libexec/corebrightnessd";
static uint32_t coreBrightnessDSize = sizeof("/usr/libexec/corebrightnessd") - 1;

static const char *notificationCenter = "/System/Library/CoreServices/NotificationCenter.app/Contents/MacOS/NotificationCenter";
static uint32_t notificationCenterSize = sizeof("/System/Library/CoreServices/NotificationCenter.app/Contents/MacOS/NotificationCenter") - 1;

// 10.12.4+, 10.13, 10.14
static const char *sierraSystemPreferences = "/Applications/System Preferences.app/Contents/MacOS/System Preferences";
static uint32_t sierraSystemPreferencesSize = sizeof("/Applications/System Preferences.app/Contents/MacOS/System Preferences") - 1;

// 10.15
static const char *catalinaSystemPreferences = "/System/Applications/System Preferences.app/Contents/MacOS/System Preferences";
static uint32_t catalinaSystemPreferencesSize = sizeof("/System/Applications/System Preferences.app/Contents/MacOS/System Preferences") - 1;

// Make _CBU_IsNightShiftSupported always return true.

// 10.12.4+
static uint8_t sierraFind[] {
    0x55, 0x48, 0x89, 0xE5, 0x41, 0x57, 0x41, 0x56, 0x41, 0x55, 0x41, 0x54, 0x53, 0x48, 0x83, 0xEC, 0x18, 0xBF, 0x01
};

// 10.13, 10.14, 10.15
static uint8_t highSierraFind[] {
    0x55, 0x48, 0x89, 0xE5, 0x41, 0x57, 0x41, 0x56, 0x41, 0x55, 0x41, 0x54, 0x53, 0x50, 0xBF, 0x01, 0x00, 0x00, 0x00
};

static uint8_t replace[] {
    0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
};

// 10.12.4+
static UserPatcher::BinaryModPatch sierraModPatch {
    CPU_TYPE_X86_64,
    0,
    sierraFind,
    replace,
    arrsize(sierraFind),
    0,
    1,
    UserPatcher::FileSegment::SegmentTextText,
    1
};

// 10.13, 10.14, 10.15
static UserPatcher::BinaryModPatch highSierraModPatch {
    CPU_TYPE_X86_64,
    0,
    highSierraFind,
    replace,
    arrsize(highSierraFind),
    0,
    1,
    UserPatcher::FileSegment::SegmentTextText,
    1
};

// 10.12.4+
static UserPatcher::BinaryModInfo sierraModInfo { coreBrightness, &sierraModPatch, 1 };

// 10.13, 10.14, 10.15
static UserPatcher::BinaryModInfo highSierraModInfo { coreBrightness, &highSierraModPatch, 1 };

// 10.12.4+, 10.13, 10.14
static UserPatcher::ProcInfo sierraProcInfo[] {
    { coreBrightnessD, coreBrightnessDSize, 1 },
    { notificationCenter, notificationCenterSize, 1 },
    { sierraSystemPreferences, sierraSystemPreferencesSize, 1 }
};

// 10.15
static UserPatcher::ProcInfo catalinaProcInfo[] {
    { coreBrightnessD, coreBrightnessDSize, 1 },
    { notificationCenter, notificationCenterSize, 1 },
    { catalinaSystemPreferences, catalinaSystemPreferencesSize, 1 }
};

void NSE::init() {
    if (getKernelVersion() == KernelVersion::Sierra && getKernelMinorVersion() >= 4) {
        lilu.onProcLoad(sierraProcInfo, 3, nullptr, nullptr, &sierraModInfo, 1);
    }
    else if (getKernelVersion() == KernelVersion::HighSierra || getKernelVersion() == KernelVersion::Mojave) {
        lilu.onProcLoad(sierraProcInfo, 3, nullptr, nullptr, &highSierraModInfo, 1);
    }
    else if (getKernelVersion() >= KernelVersion::Catalina) {
        lilu.onProcLoad(catalinaProcInfo, 3, nullptr, nullptr, &highSierraModInfo, 1);
    }
}

void NSE::deinit() {
    
}
