#pragma once

#include <stdint.h>
#include <stdlib.h>

// DISCLAIMER:
// THIS FILE IS CREATED FROM SCRATCH, BY READING THE OFFICIAL CANN API
// DOCUMENTATION REFERENCED BELOW, IN ORDER TO MAKE FASTFETCH MIT COMPLIANT.

// https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/80RC3alpha003/apiref/appdevgapi/aclcppdevg_03_1345.html
// https://www.hiascend.com/document/detail/en/CANNCommunityEdition/600alphaX/infacldevg/aclcppdevg/aclcppdevg_03_0656.html
typedef int aclError;

// https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/80RC3alpha003/apiref/appdevgapi/aclcppdevg_03_1345.html
// https://www.hiascend.com/document/detail/en/CANNCommunityEdition/600alphaX/infacldevg/aclcppdevg/aclcppdevg_03_0656.html
typedef enum ascendError_enum {
  ACL_SUCCESS = 0,
  ACL_ERROR_INVALID_PARAM = 100000,
  ACL_ERROR_UNINITIALIZE = 100001,
  ACL_ERROR_REPEAT_INITIALIZE  = 100002,
  ACL_ERROR_REPEAT_FINALIZE = 100037,
  ACL_ERROR_COMPILING_STUB_MODE = 100039,
  // ignore other errors now.
} ACLresult;

// https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/80RC3alpha003/apiref/appdevgapi/aclcppdevg_03_1398.html
// https://www.hiascend.com/document/detail/en/CANNCommunityEdition/600alphaX/infacldevg/aclcppdevg/aclcppdevg_03_0083.html
typedef enum aclrtMemAttr {
    ACL_DDR_MEM,            //DDR memory, that is, all huge page memory and normal page memory on the DDR
    ACL_HBM_MEM,            //HBM memory, that is, all huge page memory and normal page memory on the HBM
    ACL_DDR_MEM_HUGE,       //DDR huge page memory
    ACL_DDR_MEM_NORMAL,     //DDR normal page memory
    ACL_HBM_MEM_HUGE,       //HBM huge page memory
    ACL_HBM_MEM_NORMAL,     //HBM normal page memory
    ACL_DDR_MEM_P2P_HUGE,   //Huge page memory for inter-device memory copy on the DDR
    ACL_DDR_MEM_P2P_NORMAL, //Normal page memory for inter-device memory copy on the DDR
    ACL_HBM_MEM_P2P_HUGE,   //Huge page memory for inter-device memory copy on the HBM
    ACL_HBM_MEM_P2P_NORMAL, //Normal page memory for inter-device memory copy on the HBM
} aclrtMemAttr;

// https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/80RC3alpha003/apiref/appdevgapi/aclcppdevg_03_1406.html
typedef enum {
    ACL_DEVICE_INFO_UNDEFINED = -1,       // Unknown
    ACL_DEVICE_INFO_AI_CORE_NUM = 0,      // AI Core number
    ACL_DEVICE_INFO_VECTOR_CORE_NUM = 1,  // Vector Core number
    ACL_DEVICE_INFO_L2_SIZE = 2           // L2 Buffer size，unit: Byte
} aclDeviceInfo;

typedef struct aclrtUtilizationExtendInfo aclrtUtilizationExtendInfo;

// https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/80RC3alpha003/apiref/appdevgapi/aclcppdevg_03_1394.html
typedef struct aclrtUtilizationInfo {
    int32_t cubeUtilization;   // Cube利用率
    int32_t vectorUtilization; // Vector利用率
    int32_t aicpuUtilization;  // AI CPU利用率
    int32_t memoryUtilization; // Device内存利用率
    aclrtUtilizationExtendInfo *utilizationExtend; // 预留参数，当前设置为null
} aclrtUtilizationInfo;

int32_t DEVICE_UTILIZATION_NOT_SUPPORT = -1;

// https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/80RC3alpha003/apiref/appdevgapi/aclcppdevg_03_0022.html
// https://www.hiascend.com/document/detail/en/CANNCommunityEdition/600alphaX/infacldevg/aclcppdevg/aclcppdevg_03_0011.html
// Initializes AscendCL. This API is synchronous.
extern aclError aclInit(const char *configPath);

// https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/80RC3alpha003/apiref/appdevgapi/aclcppdevg_03_0023.html
// https://www.hiascend.com/document/detail/en/CANNCommunityEdition/600alphaX/infacldevg/aclcppdevg/aclcppdevg_03_0012.html
// Deinitializes AscendCL to clean up AscendCL allocations. This API is synchronous.
extern aclError aclFinalize();

// https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/80RC3alpha003/apiref/appdevgapi/aclcppdevg_03_0045.html
// https://www.hiascend.com/document/detail/en/CANNCommunityEdition/600alphaX/infacldevg/aclcppdevg/aclcppdevg_03_0025.html
// Obtains the number of compute-capable devices. This API is synchronous.
extern aclError aclrtGetDeviceCount(uint32_t *count);

// https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/80RC3alpha003/apiref/appdevgapi/aclcppdevg_03_0048.html
// https://www.hiascend.com/document/detail/en/CANNCommunityEdition/600alphaX/infacldevg/aclcppdevg/aclcppdevg_03_0015.html
// Obtains the SoC version of the operating environment. This API is synchronous.
extern const char *aclrtGetSocName();

// https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/80RC3alpha003/apiref/appdevgapi/aclcppdevg_03_0039.html
// https://www.hiascend.com/document/detail/en/CANNCommunityEdition/600alphaX/infacldevg/aclcppdevg/aclcppdevg_03_0020.html
// Sets the compute device in the calling process or thread, and creates a default context implicitly. This API is synchronous.
extern aclError aclrtSetDevice(int32_t deviceId);

// https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/80RC3alpha003/apiref/appdevgapi/aclcppdevg_03_0107.html
// https://www.hiascend.com/document/detail/en/CANNCommunityEdition/600alphaX/infacldevg/aclcppdevg/aclcppdevg_03_0083.html
// Obtains the free memory and total memory of a specified attribute. This API is synchronous.
extern aclError aclrtGetMemInfo(aclrtMemAttr attr, size_t *free, size_t *total);

// https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/80RC3alpha003/apiref/appdevgapi/aclcppdevg_03_0036.html
// Obtains the hardware specifications and size corresponding to the Device in the running environment.
extern aclError aclGetDeviceCapability(uint32_t deviceId, aclDeviceInfo deviceInfo, int64_t *value);

// https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/80RC3alpha003/apiref/appdevgapi/aclcppdevg_03_0046.html
// Obtains the Cube, Vector, AI CPU utilization rate.
extern aclError aclrtGetDeviceUtilizationRate(int32_t deviceId, aclrtUtilizationInfo *utilizationInfo);