//
//  UMVHIDDevice.hpp
//  UMVHIDDriver
//
//  Created by fOrest on 6/25/16.
//  Copyright © 2016 fOrest. All rights reserved.
//

#ifndef UMVHIDDevice_hpp
#define UMVHIDDevice_hpp

//#include <stdio.h>
#include <IOKit/hid/IOHIDDevice.h>
#include "debug.h"
#include "config.h"

class UMVHIDDevice: public IOHIDDevice {
    OSDeclareDefaultStructors(UMVHIDDevice)
    
private:
    
    static const uint32_t locationIdBase = 0xFAFAFAFA;
    
    uint32_t                    m_location_id;
    HIDCapabilities             m_capabilities;
    OSString                    *m_serial_number_string = nullptr;
    OSNumber                    *m_vendor_id = nullptr;
    OSNumber                    *m_product_id = nullptr;
    OSString                    *m_product_string = nullptr;

    IOBufferMemoryDescriptor    *m_report_descriptor;
    IOBufferMemoryDescriptor    *m_state_buffer;
        
public:
    
    virtual bool init(const void *hidDescriptorData, size_t hidDescriptorDataSize, OSString *productString,OSString *serialNumberString, uint32_t vendorID = 0, uint32_t productID = 0, OSDictionary *dictionary = 0);
    
    virtual bool init(OSDictionary *dictionary = 0) override;
    
    virtual void free(void) override;
    virtual bool start(IOService *provider) override;
    virtual void stop(IOService *provider) override;
    
    virtual bool didTerminate(IOService *provider, IOOptionBits options, bool *defer) override;
    
    virtual void detach(IOService *provider) override;
    
//    void taggedRetain(const void* tag) const override;
//    
//    void taggedRelease(const void * tag) const override;
    /**
     *  Set the device name.
     *  The reference count is automatically increased.
     *
     *  @param name A unique device name.
     */
    virtual void setProductString(OSString *productString);
    
    /**
     *  Set the serial number string.
     *  The reference count is automatically increased.
     *
     *  @param serialNumberString: A serial number string.
     */
    virtual void setSerialNumberString(OSString *serialNumberString);
    
    /**
     *  Set the vendor ID.
     *
     *  @param vendorID The vendor ID.
     */
    virtual void setVendorID(uint32_t vendorID);
    
    /**
     *  Set the product ID.
     *
     *  @param productID The product ID.
     */
    virtual void setProductID(uint32_t productID);
    /**
     *  update report data with device
     *
     *  @return true if update successfully, false vice versa
     */
    bool parseHidDescriptor(const void *hidDescriptorData, size_t hidDescriptorDataSize);

    /**
     *  Return the device name, WITHOUT increasing the reference count.
     *
     *  @return The device name.
     */
    virtual OSString *productString();
    
    
    virtual OSString *newTransportString() const override;
    virtual OSString *newManufacturerString() const override;
    virtual OSNumber *newVersionNumber() const override;
    virtual OSNumber *newPrimaryUsageNumber() const override;
    virtual OSNumber *newPrimaryUsagePageNumber() const override;
    virtual OSNumber *newLocationIDNumber() const override;
    virtual OSNumber *newVendorIDSourceNumber() const override;
    virtual OSNumber *newCountryCodeNumber() const override;
    
    virtual OSString *newProductString() const override;
    virtual OSString *newSerialNumberString() const override;
    virtual OSNumber *newVendorIDNumber() const override;
    virtual OSNumber *newProductIDNumber() const override;
    
    virtual IOReturn newReportDescriptor(IOMemoryDescriptor **descriptor) const override;
};




#endif /* UMVHIDDevice_hpp */
