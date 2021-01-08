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
     *  Function for patching _ModelMinVersionBLR
     */
    static void PatchModelMinVersionBLR(const void *data, size_t size, const char (&path)[PATH_MAX]);
    
    /**
     *  Codesign page validation wrapper used for userspace patching for pre-Big Sur systems
     */
    static void csValidateRange(vnode *vp, memory_object_t pager, memory_object_offset_t page_offset, const void *data, vm_size_t size, unsigned *result);
    
    /**
     *  Codesign page validation wrapper used for userspace patching for Big Sur systems
     */
    static void csValidatePage(vnode *vp, memory_object_t pager, memory_object_offset_t page_offset, const void *data, int *validated_p, int *tainted_p, int *nx_p);

    /**
     * Original codesign page or range validation pointer
     */
    mach_vm_address_t orgCsValidate {0};
    
    /**
     *  Current shared cache
     */
    const char *sharedCachePath {nullptr};
};

#endif /* kern_nse */
