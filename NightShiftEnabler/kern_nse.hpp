//
//  kern_nse.hpp
//  NSE
//
//  Copyright © 2021 cdf. All rights reserved.
//

#ifndef kern_nse_hpp
#define kern_nse_hpp

#include <Headers/kern_patcher.hpp>

class NSE {
public:
    void init();
    void deinit();
    
    /**
     *  Property patching routine
     *
     *  @param patcher  KernelPatcher instance
     */
    void processKernel(KernelPatcher &patcher);
    
private:
    /**
     *  Private self instance for callbacks
     */
    static NSE *callbackNSE;
    
    /**
     *  Codesign page validation wrapper used for userspace patching
     */
    static void csValidatePage(vnode *vp, memory_object_t pager, memory_object_offset_t page_offset, const void *data, int *validated_p, int *tainted_p, int *nx_p);

    /**
     * Original codesign page validation pointer
     */
    mach_vm_address_t orgCsValidatePage {0};
    
    /**
     *  Current shared cache
     */
    const char *sharedCachePath {nullptr};
};

#endif /* kern_nse */
