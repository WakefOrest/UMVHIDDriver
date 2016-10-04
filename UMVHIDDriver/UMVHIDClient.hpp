//
//  UMVHIDClient.hpp
//  UMVHIDDriver
//
//  Created by fOrest on 6/25/16.
//  Copyright © 2016 fOrest. All rights reserved.
//

#ifndef UMVHIDClient_hpp
#define UMVHIDClient_hpp

#include <IOKit/IOUserClient.h>
#include "UMVHIDDevice.hpp"
#include "UMVHIDService.hpp"

/**
 The goal of this User Client is to expose to user space the following selector.
 */
enum {
    
    com_fOrest_UMVHID_external_method_create_device,
    com_fOrest_UMVHID_external_method_destroy_device,
    com_fOrest_UMVHID_external_method_handle_report,
    com_fOrest_UMVHID_external_method_list_devices,
    com_fOrest_UMVHID_external_method_get_device_state,
    
    com_fOrest_UMVHID_external_method_count  // Keep track of the length of this enum.
};

class UMVHIDClient : public IOUserClient {
    OSDeclareDefaultStructors(UMVHIDClient);
    
public:
    virtual bool initWithTask(task_t owningTask, void *securityToken, UInt32 type, OSDictionary *properties) override;
    
    virtual bool start(IOService *provider) override;
    virtual void stop(IOService *provider) override;
    virtual void free(void) override;
    
    virtual IOReturn externalMethod(uint32_t selector, IOExternalMethodArguments *arguments, IOExternalMethodDispatch *dispatch, OSObject *target, void *reference) override;
    
protected:
    /**
     * The following methods unpack/handle the given arguments and
     * call the related driver method.
     */
    virtual IOReturn externalMethodCreateDevice(IOExternalMethodArguments *arguments);
    virtual IOReturn externalMethodDestroyDevice(IOExternalMethodArguments *arguments);
    virtual IOReturn externalMethodHandleReport(IOExternalMethodArguments *arguments);
    virtual IOReturn externalMethodListDevices(IOExternalMethodArguments *arguments);
    virtual IOReturn externalMethodGetDeviceState(IOExternalMethodArguments *arguments);
    /**
     *  The following static methods redirect the call to the 'target' instance.
     */
    
    static IOReturn staticExternalMethodCreateDevice(UMVHIDClient *target,  void *reference, IOExternalMethodArguments *arguments);
    static IOReturn staticExternalMethodDestroyDevice(UMVHIDClient *target, void *reference, IOExternalMethodArguments *arguments);
    static IOReturn staticExternalMethodHandleReport(UMVHIDClient *target,    void *reference, IOExternalMethodArguments *arguments);
    static IOReturn staticExternalMethodListDevices(UMVHIDClient *target,    void *reference, IOExternalMethodArguments *arguments);
    static IOReturn staticExternalMethodGetDeviceState(UMVHIDClient *target,  void *reference, IOExternalMethodArguments *arguments);
    
private:
    /**
     *  Method dispatch table.
     */
    static const IOExternalMethodDispatch externalMethodDispatchTable[com_fOrest_UMVHID_external_method_count];
    
    /**
     *  Driver provider.
     */
    UMVHIDService *m_hid_provider;
    
    /**
     *  Task owner.
     */
    task_t m_task_owner;
};


#endif /* UMVHIDClient_hpp */
