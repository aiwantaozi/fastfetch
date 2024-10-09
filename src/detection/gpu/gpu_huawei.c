#include "gpu_driver_specific.h"

#include "common/library.h"
#include "acl.h"

struct FFAclData {
    FF_LIBRARY_SYMBOL(aclrtGetDeviceCount)
    FF_LIBRARY_SYMBOL(aclrtGetSocName)
    FF_LIBRARY_SYMBOL(aclrtSetDevice)
    FF_LIBRARY_SYMBOL(aclrtGetMemInfo)
    // FF_LIBRARY_SYMBOL(aclGetDeviceCapability)
    // FF_LIBRARY_SYMBOL(aclrtGetDeviceUtilizationRate)
    FF_LIBRARY_SYMBOL(aclFinalize)
    bool inited;
} aclData;

static void shutdownAcl()
{
    if (aclData.inited)
        aclData.ffaclFinalize();
}

const char* ffDetectHuaweiGpuInfo(const FFGpuDriverCondition* cond, FFGpuDriverResult result, const char* soName)
{
#ifndef FF_DISABLE_DLOPEN

    if (!aclData.inited)
    {
        aclData.inited = true;
        FF_LIBRARY_LOAD(libacl, "dlopen acl failed", soName , 1);
        FF_LIBRARY_LOAD_SYMBOL_MESSAGE(libacl, aclInit)
        FF_LIBRARY_LOAD_SYMBOL_VAR_MESSAGE(libacl, aclData, aclFinalize)
        FF_LIBRARY_LOAD_SYMBOL_VAR_MESSAGE(libacl, aclData, aclrtGetDeviceCount)
        FF_LIBRARY_LOAD_SYMBOL_VAR_MESSAGE(libacl, aclData, aclrtGetSocName)
        FF_LIBRARY_LOAD_SYMBOL_VAR_MESSAGE(libacl, aclData, aclrtSetDevice)
        FF_LIBRARY_LOAD_SYMBOL_VAR_MESSAGE(libacl, aclData, aclrtGetMemInfo)
        // FF_LIBRARY_LOAD_SYMBOL_VAR_MESSAGE(libacl, aclData, aclGetDeviceCapability) // 1.9.0 and above
        // FF_LIBRARY_LOAD_SYMBOL_VAR_MESSAGE(libacl, aclData, aclrtGetDeviceUtilizationRate) // 1.6.0 and above
        if (ffaclInit(NULL) != ACL_SUCCESS)
        {
            aclData.ffaclrtGetDeviceCount = NULL;
            return "aclInit() failed";
        }
        atexit(shutdownAcl);
        libacl = NULL; // don't close acl
    }

    if (aclData.ffaclrtGetDeviceCount == NULL)
        return "loading acl library failed";

    uint32_t count;
    if (aclData.ffaclrtGetDeviceCount(&count) != ACL_SUCCESS)
        return "aclrtGetDeviceCount() failed";

    int32_t deviceId = 6;
    // for (uint32_t i = 0; i < count; i++)
    // {
    //     if (*result.index == i)
    //     {
    //         deviceId = (int32_t)i;
    //         break;
    //     }
    // }

    if (deviceId == -1)
        return "Device not found";

    if (aclData.ffaclrtSetDevice(deviceId) != ACL_SUCCESS)
        return "aclrtSetDevice() failed";

    if (result.index)
        *result.index = (uint32_t) deviceId;

    if (result.type)
        *result.type = FF_GPU_TYPE_DISCRETE;


    if (result.memory)
    {
        // https://github.com/mindspore-ai/akg/blob/7ce3506054cdf191c5a14ef7cb499833df4b4af3/src/runtime/ascend/ascend_memory_manager.cc#L29
        // Try ACL_HBM_MEM first, then ACL_DDR_MEM
        size_t free = 0;
        size_t total = 0;
        if (aclData.ffaclrtGetMemInfo(ACL_HBM_MEM, &free, &total) == ACL_SUCCESS)
        {
            result.memory->total = (uint64_t) total;
            result.memory->used = (uint64_t) (total - free);
        }
        else
        {
            if (aclData.ffaclrtGetMemInfo(ACL_DDR_MEM, &free, &total) == ACL_SUCCESS)
            {
                result.memory->total = (uint64_t) total;
                result.memory->used = (uint64_t) (total - free);
            }
        }
    }

    // if (result.coreCount){
    //     int64_t aiCoreNum = 0;
    //     aclData.ffaclGetDeviceCapability((uint32_t)deviceId, ACL_DEVICE_INFO_AI_CORE_NUM, &aiCoreNum);
    //     *result.coreCount =  (uint32_t) aiCoreNum;
    // }

    // if (result.coreUsage)
    // {
    //     // https://github.com/Ascend/pytorch/blob/cf14e09e53597415678578cb5be21be5d0a74180/torch_npu/csrc/npu/Module.cpp#L417
    //     aclrtUtilizationInfo utilization;
    //     if (aclData.ffaclrtGetDeviceUtilizationRate(deviceId, &utilization) == ACL_SUCCESS)
    //     {
    //         if (utilization.cubeUtilization == DEVICE_UTILIZATION_NOT_SUPPORT && utilization.vectorUtilization != DEVICE_UTILIZATION_NOT_SUPPORT) {
    //             *result.coreUsage = utilization.vectorUtilization;
    //         } else if (utilization.cubeUtilization != DEVICE_UTILIZATION_NOT_SUPPORT && utilization.vectorUtilization == DEVICE_UTILIZATION_NOT_SUPPORT) {
    //             *result.coreUsage = utilization.cubeUtilization;
    //         } else if (utilization.cubeUtilization != DEVICE_UTILIZATION_NOT_SUPPORT && utilization.vectorUtilization != DEVICE_UTILIZATION_NOT_SUPPORT) {
    //             *result.coreUsage = (utilization.cubeUtilization + utilization.vectorUtilization) / 2;
    //         }
    //     }
    // }

    if (result.name)
    {
        const char *socName = aclData.ffaclrtGetSocName();
        ffStrbufSetS(result.name, socName);
    }

    return NULL;

#else

    FF_UNUSED(cond, result, soName);
    return "dlopen is disabled";

#endif
}
