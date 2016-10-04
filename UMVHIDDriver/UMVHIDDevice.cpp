//
//  UMVHIDDevice.cpp
//  UMVHIDDriver
//
//  Created by fOrest on 6/25/16.
//  Copyright © 2016 fOrest. All rights reserved.
//

#include "UMVHIDDevice.hpp"



#define super IOHIDDevice
OSDefineMetaClassAndStructors(UMVHIDDevice, IOHIDDevice)

//void UMVHIDDevice::taggedRetain(const void* tag) const
//{
//    OSReportWithBacktrace(
//                          "UMVHIDDevice" CLASS_OBJECT_FORMAT_STRING "::taggedRetain(tag=%p)\n", CLASS_OBJECT_FORMAT(this), tag);
//    
//    LogD("UMVHIDDevice:: taggedRetain retainCount: %d, tag= %p", this->getRetainCount(), tag);
//    IOService::taggedRetain(tag);
//}
//void UMVHIDDevice::taggedRelease(const void * tag) const
//{
//    OSReportWithBacktrace(
//                          "UMVHIDDevice" CLASS_OBJECT_FORMAT_STRING "::taggedRelease(tag=%p)\n", CLASS_OBJECT_FORMAT(this), tag);
//    int count = getRetainCount();
//    IOService::taggedRelease(tag);
//
//    if (count == 1)
//        printf(
//               "UMVHIDDevice::taggedRelease(tag=%p) final done\n", tag);
//    else
//        printf(
//               "UMVHIDDevice" CLASS_OBJECT_FORMAT_STRING "::taggedRelease(tag=%p) done\n", CLASS_OBJECT_FORMAT(this), tag);
//}


bool UMVHIDDevice::parseHidDescriptor(const void *hidDescriptorData, size_t hidDescriptorDataSize) {
    
    HIDPreparsedDataRef preparsedDataRef = 0;
    
    if (HIDOpenReportDescriptor(const_cast< void* >(hidDescriptorData), hidDescriptorDataSize, &preparsedDataRef, 0) != kIOReturnSuccess) {
        
        return false;
    }
    
    bool result = (HIDGetCapabilities(preparsedDataRef, &m_capabilities) == kIOReturnSuccess);
    HIDCloseReportDescriptor(preparsedDataRef);
    return result;
}


bool UMVHIDDevice::init(OSDictionary *dictionary) {
    
    LogD("Initializing a new virtual HID device.");
    
    if (!super::init(dictionary)) {
        return false;
    }
    
    return true;
}

bool UMVHIDDevice::init(const void *hidDescriptorData, size_t hidDescriptorDataSize, OSString *productString,OSString *serialNumberString, uint32_t vendorID, uint32_t productID, OSDictionary *dictionary) {
    
    m_vendor_id              = OSNumber::withNumber(vendorID, 32);
    m_product_id             = OSNumber::withNumber(productID, 32);;
    m_report_descriptor      = 0;
    m_state_buffer           = 0;
    m_product_string         = 0;
    m_serial_number_string   = 0;
    
    if(productString == 0 || hidDescriptorData == 0) {
        
        return false;
    }
    if(!super::init(dictionary)) {
        return false;
    }
    if(!parseHidDescriptor(hidDescriptorData, hidDescriptorDataSize)) {
        return false;
    }
    if(m_capabilities.inputReportByteLength > kMaxHIDReportSize) {
        return false;
    }
    
    if(m_capabilities.usagePage == kHIDPage_GenericDesktop)
    {
        if(m_capabilities.usage == kHIDUsage_GD_Mouse || m_capabilities.usage == kHIDUsage_GD_Keyboard)
        {
            // hack for Apple HID subsystem
            OSString *str = OSString::withCString( (m_capabilities.usage == kHIDUsage_GD_Mouse)? ("Mouse"): ("Keyboard"));
            
            if(str == NULL) {
                return false;
            }
            
            if(!setProperty("HIDDefaultBehavior", str)) {
                str->release();
                return false;
            }
            str->release();
        }
    }
    
    m_report_descriptor = IOBufferMemoryDescriptor::withBytes( hidDescriptorData, hidDescriptorDataSize, kIODirectionInOut);
    if(m_report_descriptor == 0) {
        return false;
    }
    m_state_buffer = IOBufferMemoryDescriptor::withCapacity(m_capabilities.inputReportByteLength, kIODirectionInOut);
    if(m_state_buffer == 0) {
        return false;
    }
    // init report state buffer
    memset(m_state_buffer->getBytesNoCopy(), 0, m_state_buffer->getLength());
    
    {
        static uint32_t lastId = locationIdBase;
        
        m_location_id = lastId++;
    }
    
    m_product_string          = productString;
    m_serial_number_string    = serialNumberString;
    
    m_product_string->retain();
    m_serial_number_string->retain();
    
    LogD("Init UMVHIDDevice succeed");
    
    return true;
}

bool UMVHIDDevice::start(IOService *provider) {
    
    LogD("Executing 'UMVHIDDevice::start()'.");
    return super::start(provider);
}

void UMVHIDDevice::stop(IOService *provider) {
    
    LogD("Executing 'UMVHIDDevice::stop(), name: %s'", m_product_string->getCStringNoCopy());
    super::stop(provider);
}

void UMVHIDDevice::free() {
    
    LogD("Executing 'UMVHIDDevice::free() name: %s'", m_product_string->getCStringNoCopy());
    if (m_report_descriptor) {
        m_report_descriptor->release();
    }
    if (m_state_buffer) {
        m_state_buffer->release();
    }
    if (m_product_string) {
        m_product_string->release();
    }
    if (m_serial_number_string) {
        m_serial_number_string->release();
    }
    super::free();
}

bool UMVHIDDevice::didTerminate(IOService *provider, IOOptionBits options, bool *defer) {
    
    LogD("Executing 'UMVHIDDevice::didTerminate() name: %s,'", m_product_string->getCStringNoCopy());
    
    return super::didTerminate(provider, options, defer);
}

void UMVHIDDevice::detach(IOService *provider) {
    
    LogD("Executing 'UMVHIDDevice::detach() name: %s'", m_product_string->getCStringNoCopy());
    super::detach(provider);
}

OSString *UMVHIDDevice::productString() {
    
    return m_product_string;
}

void UMVHIDDevice::setProductString(OSString *productString) {
    if (productString) {
        productString->retain();
    }
    m_product_string = productString;
}

void UMVHIDDevice::setSerialNumberString(OSString *serialNumberString) {
    if (serialNumberString) {
        serialNumberString->retain();
    }
    m_serial_number_string = serialNumberString;
}

void UMVHIDDevice::setVendorID(uint32_t vendorID) {
    m_vendor_id = OSNumber::withNumber(vendorID, 32);
}

void UMVHIDDevice::setProductID(uint32_t productID) {
    m_product_id = OSNumber::withNumber(productID, 32);
}

IOReturn UMVHIDDevice::newReportDescriptor(IOMemoryDescriptor **descriptor) const
{
    IOBufferMemoryDescriptor *result = IOBufferMemoryDescriptor::withBytes(m_report_descriptor->getBytesNoCopy(), m_report_descriptor->getLength(), kIODirectionInOut);
    
    if(result == 0) {
        return kIOReturnError;
    }
    *descriptor = result;
    
    return kIOReturnSuccess;
}


OSString *UMVHIDDevice::newProductString() const {
    m_product_string->retain();
    return m_product_string;
}

OSString *UMVHIDDevice::newSerialNumberString() const {
    m_serial_number_string->retain();
    return m_serial_number_string;
}

OSNumber *UMVHIDDevice::newVendorIDNumber() const {
    m_vendor_id->retain();
    return m_vendor_id;
}

OSNumber *UMVHIDDevice::newProductIDNumber() const {
    m_product_id->retain();
    return m_product_id;
}


OSString *UMVHIDDevice::newTransportString() const
{
    return OSString::withCString("Ultimate Matser");
}

OSString *UMVHIDDevice::newManufacturerString() const
{
    return OSString::withCString("Microsoft");
}

OSNumber *UMVHIDDevice::newVersionNumber() const
{
    return OSNumber::withNumber(1, 32);
}

OSNumber *UMVHIDDevice::newPrimaryUsageNumber() const
{
    return OSNumber::withNumber(m_capabilities.usage, 32);
}

OSNumber *UMVHIDDevice::newPrimaryUsagePageNumber() const
{
    return OSNumber::withNumber(m_capabilities.usagePage, 32);
}

OSNumber *UMVHIDDevice::newLocationIDNumber() const
{
    return OSNumber::withNumber(m_location_id, 32);
}

OSNumber *UMVHIDDevice::newVendorIDSourceNumber() const
{
    uint32_t number = 0;
    return OSNumber::withNumber(number, 32);
}

OSNumber *UMVHIDDevice::newCountryCodeNumber() const
{
    uint32_t number = 0;
    return OSNumber::withNumber(number, 32);
}