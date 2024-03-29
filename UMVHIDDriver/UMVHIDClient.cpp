//
//  UMVHIDClient.cpp
//  UMVHIDDriver
//
//  Created by fOrest on 6/25/16.
//  Copyright © 2016 fOrest. All rights reserved.
//

#include "UMVHIDClient.hpp"

#define super IOUserClient
OSDefineMetaClassAndStructors(UMVHIDClient, IOUserClient)

bool UMVHIDClient::initWithTask(task_t owningTask, void *securityToken, UInt32 type, OSDictionary *properties) {
    LogD("Executing 'UMVHIDClient::initWithTask()'.");
    
    if (!owningTask) {
        return false;
    }
    
    if (!super::initWithTask(owningTask, securityToken, type, properties)) {
        return false;
    }
    
    m_task_owner = owningTask;
    
    return true;
}

bool UMVHIDClient::start(IOService *provider) {
    LogD("Executing 'UMVHIDClient::start()'.");
    
    if (!super::start(provider)) {
        return false;
    }
    
    m_hid_provider = OSDynamicCast(UMVHIDService, provider);
    if (!m_hid_provider) {
        return false;
    }
    LogD("Executing 'UMVHIDClient::start() successfully'.");
    return true;
}

void UMVHIDClient::stop(IOService *provider) {
    
    LogD("Executing 'UMVHIDClient::stop()'.");
    
    super::stop(provider);
}

void UMVHIDClient::free() {
    LogD("Executing 'UMVHIDClient::free()'.");
    super::free();
}
/**
 * A dispatch table for this User Client interface, used by 'com_fOrest_umvhid_client::externalMethod()'.
 * The fields of the IOExternalMethodDispatch type follows:
 *
 *  struct IOExternalMethodDispatch
 *  {
 *      IOExternalMethodAction function;
 *      uint32_t		   checkScalarInputCount;
 *      uint32_t		   checkStructureInputSize;
 *      uint32_t		   checkScalarOutputCount;
 *      uint32_t		   checkStructureOutputSize;
 *  };
 */
const IOExternalMethodDispatch UMVHIDClient::externalMethodDispatchTable[com_fOrest_UMVHID_external_method_count] = {
    
    {(IOExternalMethodAction)&UMVHIDClient::staticExternalMethodCreateDevice,    8, 0, 0, 0},
    {(IOExternalMethodAction)&UMVHIDClient::staticExternalMethodDestroyDevice,   2, 0, 0, 0},
    {(IOExternalMethodAction)&UMVHIDClient::staticExternalMethodHandleReport,    4, 0, 0, 0},
    {(IOExternalMethodAction)&UMVHIDClient::staticExternalMethodListDevices,     2, 0, 2, 0},
    {(IOExternalMethodAction)&UMVHIDClient::staticExternalMethodGetDeviceState,  2, 0, 1, 0},
};

IOReturn UMVHIDClient::externalMethod(uint32_t selector, IOExternalMethodArguments *arguments,IOExternalMethodDispatch *dispatch, OSObject *target, void *reference) {
    
    LogD("Executing 'UMVHIDClient::externalMethod()' with selector #%d.", selector);
    
    if (selector >= com_fOrest_UMVHID_external_method_count) {
        return kIOReturnUnsupported;
    }
    
    dispatch = (IOExternalMethodDispatch *)&externalMethodDispatchTable[selector];
    target = this;
    reference = nullptr;
    
    return super::externalMethod(selector, arguments, dispatch, target, reference);
}



IOReturn UMVHIDClient::staticExternalMethodCreateDevice(UMVHIDClient *target, void *reference, IOExternalMethodArguments *arguments) {
    return target->externalMethodCreateDevice(arguments);
}

IOReturn UMVHIDClient::staticExternalMethodDestroyDevice(UMVHIDClient *target, void *reference,IOExternalMethodArguments *arguments) {
    return target->externalMethodDestroyDevice(arguments);
}

IOReturn UMVHIDClient::staticExternalMethodHandleReport(UMVHIDClient *target, void *reference, IOExternalMethodArguments *arguments) {
    return target->externalMethodHandleReport(arguments);
}

IOReturn UMVHIDClient::staticExternalMethodListDevices(UMVHIDClient *target, void *reference, IOExternalMethodArguments *arguments) {
    return target->externalMethodListDevices(arguments);
}

IOReturn UMVHIDClient::staticExternalMethodGetDeviceState(UMVHIDClient *target, void *reference, IOExternalMethodArguments *arguments) {
    return target->externalMethodGetDeviceState(arguments);
}


IOReturn UMVHIDClient::externalMethodCreateDevice(IOExternalMethodArguments *arguments) {
    
    IOMemoryDescriptor *user_buf = nullptr;
    IOMemoryDescriptor *descriptor_buf = nullptr;
    IOMemoryDescriptor *serial_number_buf = nullptr;
    
    bool user_buf_complete = false;
    bool descriptor_buf_complete = false;
    bool serial_number_buf_complete = false;
    
    IOMemoryMap *map = nullptr;
    IOMemoryMap *map2 = nullptr;
    IOMemoryMap *map3 = nullptr;
    
    char *ptr = nullptr;
    unsigned char *ptr2 = nullptr;
    char *ptr3 = nullptr;
    
    bool ret = false;
    
    UInt8 *name_ptr = (UInt8 *)arguments->scalarInput[0];
    UInt8 name_len = (UInt8)arguments->scalarInput[1];
    UInt8 *descriptor_ptr = (UInt8 *)arguments->scalarInput[2];
    UInt16 descriptor_len = (UInt16)arguments->scalarInput[3];
    
    UInt8 *serial_number_ptr = (UInt8 *)arguments->scalarInput[4];
    UInt8 serial_number_len = (UInt8)arguments->scalarInput[5];
    UInt32 vendorID = (UInt32)arguments->scalarInput[6];
    UInt32 productID = (UInt32)arguments->scalarInput[7];
    
    user_buf = IOMemoryDescriptor::withAddressRange((vm_address_t)name_ptr, name_len, kIODirectionOut, m_task_owner);
    if (!user_buf) goto nomem;
    if (user_buf->prepare() != kIOReturnSuccess) goto nomem;
    user_buf_complete = true;
    
    descriptor_buf = IOMemoryDescriptor::withAddressRange((vm_address_t)descriptor_ptr, descriptor_len, kIODirectionOut, m_task_owner);
    if (!descriptor_buf) goto nomem;
    if (descriptor_buf->prepare() != kIOReturnSuccess) goto nomem;
    descriptor_buf_complete = true;
    
    serial_number_buf = IOMemoryDescriptor::withAddressRange((vm_address_t)serial_number_ptr, serial_number_len, kIODirectionOut, m_task_owner);
    
    if (!serial_number_buf) goto nomem;
    if (serial_number_buf->prepare() != kIOReturnSuccess) goto nomem;
    serial_number_buf_complete = true;
    
    map = user_buf->map();
    if (!map) goto nomem;
    
    map2 = descriptor_buf->map();
    if (!map2) goto nomem;
    
    map3 = serial_number_buf->map();
    if (!map3) goto nomem;
    
    ptr = (char *)map->getAddress();
    if (!ptr) goto nomem;
    
    ptr2 = (unsigned char *)map2->getAddress();
    if (!ptr2) goto nomem;
    
    ptr3 = (char *)map3->getAddress();
    if (!ptr3) goto nomem;
    

    ret = m_hid_provider->createDevice(ptr, name_len, ptr2, descriptor_len, ptr3,
                                       serial_number_len, vendorID, productID);
    
    user_buf->complete();
    descriptor_buf->complete();
    serial_number_buf->complete();
    
    map->release();
    map2->release();
    map3->release();
    
    user_buf->release();
    descriptor_buf->release();
    serial_number_buf->release();
    
    if (ret) {
        return kIOReturnSuccess;
    }
    
    return kIOReturnDeviceError;
    
nomem:
    if (map) map->release();
    if (map2) map2->release();
    if (map3) map2->release();
    if (user_buf_complete) user_buf->complete();
    if (user_buf) user_buf->release();
    if (descriptor_buf_complete) descriptor_buf->complete();
    if (descriptor_buf) descriptor_buf->release();
    if (serial_number_buf_complete) serial_number_buf->complete();
    if (serial_number_buf) serial_number_buf->release();
    return kIOReturnNoMemory;
}

IOReturn UMVHIDClient::externalMethodDestroyDevice(IOExternalMethodArguments *arguments) {
    IOMemoryDescriptor *user_buf = nullptr;
    bool user_buf_complete = false;
    
    IOMemoryMap *map = nullptr;
    
    char *ptr = nullptr;
    
    bool ret = false;
    
    UInt8 *name_ptr = (UInt8 *)arguments->scalarInput[0];
    UInt8 name_len = (UInt8)arguments->scalarInput[1];
    
    user_buf = IOMemoryDescriptor::withAddressRange((vm_address_t)name_ptr, name_len, kIODirectionOut, m_task_owner);
    if (!user_buf) goto nomem;
    if (user_buf->prepare() != kIOReturnSuccess) goto nomem;
    user_buf_complete = true;
    
    map = user_buf->map();
    if (!map) goto nomem;
    
    ptr = (char *)map->getAddress();
    if (!ptr) goto nomem;
    ret = m_hid_provider->destroyDevice(ptr, name_len);
    
    user_buf->complete();
    map->release();
    user_buf->release();
    
    if (ret) {
        return kIOReturnSuccess;
    }
    
    return kIOReturnDeviceError;
    
nomem:
    if (map) map->release();
    if (user_buf_complete) user_buf->complete();
    if (user_buf) user_buf->release();
    return kIOReturnNoMemory;
}

IOReturn UMVHIDClient::externalMethodHandleReport(IOExternalMethodArguments *arguments) {
    IOMemoryDescriptor *user_buf = nullptr;
    IOMemoryDescriptor *descriptor_buf = nullptr;
    IOMemoryMap *map = nullptr;
    IOMemoryMap *map2 = nullptr;
    
    char *ptr = nullptr;
    unsigned char *ptr2 = nullptr;
    
    bool user_buf_complete = false;
    bool descriptor_buf_complete = false;
    
    bool ret = false;
    
    UInt8 *name_ptr = (UInt8 *)arguments->scalarInput[0];
    UInt8 name_len = (UInt8)arguments->scalarInput[1];
    UInt8 *descriptor_ptr = (UInt8 *)arguments->scalarInput[2];
    UInt16 descriptor_len = (UInt16)arguments->scalarInput[3];
    
    user_buf = IOMemoryDescriptor::withAddressRange((vm_address_t)name_ptr, name_len, kIODirectionOut, m_task_owner);
    if (!user_buf) goto nomem;
    if (user_buf->prepare() != kIOReturnSuccess) goto nomem;
    user_buf_complete = true;
    
    descriptor_buf = IOMemoryDescriptor::withAddressRange((vm_address_t)descriptor_ptr, descriptor_len,
                                                          kIODirectionOut, m_task_owner);
    
    if (!descriptor_buf) goto nomem;
    if (descriptor_buf->prepare() != kIOReturnSuccess) goto nomem;
    descriptor_buf_complete = true;
    
    map = user_buf->map();
    if (!map) goto nomem;
    
    map2 = descriptor_buf->map();
    if (!map2) goto nomem;
    
    ptr = (char *)map->getAddress();
    if (!ptr) goto nomem;
    
    ptr2 = (unsigned char *)map2->getAddress();
    if (!ptr2) goto nomem;
    
    ret = m_hid_provider->handleReport(ptr, name_len, ptr2, descriptor_len);

    user_buf->complete();
    descriptor_buf->complete();
    
    map->release();
    map2->release();
    
    user_buf->release();
    descriptor_buf->release();
    
    if (ret) {
        return kIOReturnSuccess;
    }
    
    return kIOReturnDeviceError;
    
nomem:
    if (user_buf_complete) user_buf->complete();
    if (descriptor_buf_complete) descriptor_buf->complete();
    if (map) map->release();
    if (map2) map2->release();
    if (user_buf) user_buf->release();
    if (descriptor_buf) descriptor_buf->release();
    return kIOReturnNoMemory;
}

IOReturn UMVHIDClient::externalMethodListDevices(IOExternalMethodArguments *arguments) {
    IOMemoryDescriptor *user_buf = nullptr;
    
    IOMemoryMap *map = nullptr;
    
    UInt16 needed = 0, items = 0;
    
    char *ptr = nullptr;
    bool ret = false;
    
    UInt8 *name_ptr = (UInt8 *)arguments->scalarInput[0];
    UInt16 name_len = (UInt16)arguments->scalarInput[1];
    
    user_buf = IOMemoryDescriptor::withAddressRange((vm_address_t)name_ptr, name_len, kIODirectionIn, m_task_owner);
    if (!user_buf) goto nomem;
    if (user_buf->prepare() != kIOReturnSuccess) goto nomem;
    
    map = user_buf->map();
    if (!map) goto nomem;
    
    ptr = (char *)map->getAddress();
    if (!ptr) goto nomem;
    
    ret = m_hid_provider->listDevices(ptr, name_len, &needed, &items);
    
    user_buf->complete();
    
    map->release();
    
    user_buf->release();
    if (ret) {
        arguments->scalarOutput[0] = needed;
        arguments->scalarOutput[1] = items;
        return kIOReturnSuccess;
    }
    
    return kIOReturnDeviceError;
    
nomem:
    if (map) map->release();
    if (user_buf) user_buf->release();
    return kIOReturnNoMemory;
}

IOReturn UMVHIDClient::externalMethodGetDeviceState(IOExternalMethodArguments *arguments) {
    
    IOMemoryDescriptor *user_buf = nullptr;
    bool user_buf_complete = false;
    
    IOMemoryMap *map = nullptr;
    
    char *ptr = nullptr;
    
    bool ret = false;
    
    UInt32 state = 0;
    
    UInt8 *name_ptr = (UInt8 *)arguments->scalarInput[0];
    UInt8 name_len = (UInt8)arguments->scalarInput[1];

    user_buf = IOMemoryDescriptor::withAddressRange((vm_address_t)name_ptr, name_len, kIODirectionOut, m_task_owner);
    if (!user_buf) goto nomem;
    if (user_buf->prepare() != kIOReturnSuccess) goto nomem;
    user_buf_complete = true;

    map = user_buf->map();
    if (!map) goto nomem;
    
    ptr = (char *)map->getAddress();
    if (!ptr) goto nomem;

    ret = m_hid_provider->getDeviceState(ptr, name_len, state);
    
    user_buf->complete();
    map->release();
    user_buf->release();
    
    if (ret) {
        arguments->scalarOutput[0] = state;
        return kIOReturnSuccess;
    }
    
    return kIOReturnDeviceError;
    
nomem:
    
    if (map) map->release();
    if (user_buf_complete) user_buf->complete();
    if (user_buf) user_buf->release();
    return kIOReturnNoMemory;
}

