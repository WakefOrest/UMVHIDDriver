//
//  UMVHIDService.cpp
//  UMVHIDDriver
//
//  Created by fOrest on 6/25/16.
//  Copyright © 2016 fOrest. All rights reserved.
//

#include "UMVHIDService.hpp"


#define super IOService
OSDefineMetaClassAndStructors(UMVHIDService, IOService);


bool UMVHIDService::start(IOService *provider) {
    LogD("Executing 'UMVHIDService::start()'.");
    
    bool ret = super::start(provider);
    if (ret) {
        LogD("Calling 'UMVHIDService:registerService()'.");
        registerService();
    }
    
    return ret;
}

void UMVHIDService::stop(IOService *provider) {
    
    LogD("Executing 'UMVHIDService:stop()'.");
    
    // Terminate every managed HID device.
    OSCollectionIterator *iterator = OSCollectionIterator::withCollection(m_hid_devices);
    if (iterator) {
        const OSString *key = nullptr;
        while ((key = (OSString *)iterator->getNextObject())) {
            UMVHIDDevice *device = (UMVHIDDevice *)m_hid_devices->getObject(key);
 
            if (device) {
                
                device->terminate();
                m_hid_devices->removeObject(key);
                device->release();
                device = nullptr;
            }
        }
        iterator->release();
    }
    
    super::stop(provider);
}

bool UMVHIDService::init(OSDictionary *dictionary) {
    LogD("Executing 'UMVHIDService:init()'.");
    
    m_hid_devices = OSDictionary::withCapacity(1);
    if (!m_hid_devices) {
        LogD("Unable to inizialize HID devices dictionary.");
        return false;
    }
    return super::init(dictionary);
}

void UMVHIDService::free() {
    LogD("Executing 'UMVHIDService:free()'.");
    
    // Clear the HID devices dictionary.
    if (m_hid_devices) {
        m_hid_devices->release();
    }
    
    super::free();
}

bool UMVHIDService::createDevice(char *product_string, UInt8 product_string_len, unsigned char *report_descriptor, UInt16 report_descriptor_len, char *serial_number, UInt16 serial_number_len, UInt32 vendor_id, UInt32 product_id )  {
    
    UMVHIDDevice *device = nullptr;
    
    OSString *key = nullptr;
    OSString *serial_number_s = nullptr;
    
    if (report_descriptor_len == 0 || product_string == 0) {
        return false;
    }
    
    {
        char *c_product_string = (char *)IOMalloc(product_string_len + 1);
        if (!c_product_string) {
            return false;
        }
        memcpy(c_product_string, product_string, product_string_len);
        c_product_string[product_string_len] = 0;
        key = OSString::withCString(c_product_string);
        IOFree(c_product_string, product_string_len + 1);
    }
    
    if (!key) return false;
    
    {
        char *c_serial_number = (char *)IOMalloc(serial_number_len + 1);
        if (!c_serial_number) return false;
        memcpy(c_serial_number, serial_number, serial_number_len);
        c_serial_number[serial_number_len] = 0;
        serial_number_s = OSString::withCString(c_serial_number);
        IOFree(c_serial_number, serial_number_len + 1);
    }
    if (!serial_number_s) goto fail;
    
    LogD("Attempting to init a new virtual device with name: '%s'; " "serial number ('%s'); vendor ID (%d); product ID (%d).", key->getCStringNoCopy(), serial_number_s->getCStringNoCopy(), vendor_id, product_id);
    
    // Has the device already been created?
    device = (UMVHIDDevice *)m_hid_devices->getObject(key);
    if (device)  return false;
    
    device = OSTypeAlloc(UMVHIDDevice);
    if (!device) goto fail;
    
    if (!device->init(report_descriptor, report_descriptor_len, key, serial_number_s, vendor_id, product_id)) {
        goto fail;
    }
    if (!m_hid_devices->setObject(key, device)) {
        goto fail;
    }
    
    device->attach(this);
    device->start(this);
    
    key->release();
    serial_number_s->release();
    
    device = nullptr;
    LogD("Create virtual device successfully; ");
         
    return true;
    
fail:
    if (key) key->release();
    if (serial_number_s) serial_number_s->release();
    if (device) device->release();
    
    return false;
}

bool UMVHIDService::destroyDevice(char *product_name, UInt8 product_name_length) {
    
    OSString *key = nullptr;
    UMVHIDDevice *device = nullptr;
    
    if (product_name_length == 0) return false;
    
    {
        char *cname = (char *)IOMalloc(product_name_length + 1);
        if (!cname) return false;
        memcpy(cname, product_name, product_name_length);
        cname[product_name_length] = 0;
        key = OSString::withCString(product_name);
        IOFree(cname, product_name_length + 1);
    }
    if (!key) goto end;
    
    device = (UMVHIDDevice *)m_hid_devices->getObject(key);
    if (!device) goto end;
    
    device->terminate();
    m_hid_devices->removeObject(key);
    key->release();
    device->release();
    
    return true;
    
end:
    if (key) key->release();
    return false;
}

bool UMVHIDService::handleReport(char *product_string, UInt8 product_string_len, unsigned char *reportData, UInt16 report_len) {
    
    UMVHIDDevice *device = nullptr;
    OSString *key = nullptr;
    IOMemoryDescriptor *report = nullptr;
    
    bool ret = false;
    
    if (product_string_len == 0) return false;
    
    {
        char *cname = (char *)IOMalloc(product_string_len + 1);
        if (!cname) return false;
        memcpy(cname, product_string, product_string_len);
        cname[product_string_len] = 0;
        key = OSString::withCString(cname);
        IOFree(cname, product_string_len + 1);
    }
    
    if (!key) goto end;
    
    device = (UMVHIDDevice *)m_hid_devices->getObject(key);
    if (!device) goto end;
    
    
    report = IOBufferMemoryDescriptor::inTaskWithOptions(kernel_task, 0,
                                                         report_len);
    if (!report) goto end;
    
    LogD("Handling report of size: %d.", (int)report->getLength());
    report->writeBytes(0, reportData, report_len);
    
    if (device->handleReport(report, kIOHIDReportTypeInput) == kIOReturnSuccess) {
        LogD("Report correctly sent to device.");
        //report->readBytes(0, reportData, report_len);
        ret = true;
        
    } else {
        LogD("Error while sending report to device.");
    }
    
    report->release();
    key->release();
    
    return ret;
    
end:
    if (key) key->release();
    if (report) report->release();
    return false;
}


bool UMVHIDService::listDevices(char *buf, UInt16 buf_len,
                                 UInt16 *neededLength, UInt16 *items) {
    if (buf_len == 0) return false;
    
    LogD("Building HID virtual devices list.");
    
    UInt16 current_len = 0;
    *neededLength = 0;
    *items = 0;
    
    // Iterate through managed HID devices.
    OSCollectionIterator *iterator = OSCollectionIterator::withCollection(m_hid_devices);
    if (iterator) {
        const OSString *key = nullptr;
        while ((key = (OSString *)iterator->getNextObject())) {
            UInt8 key_len = key->getLength();
            
            if (key_len + 1 + current_len > buf_len) {
                *neededLength = buf_len + key_len + 1;
                break;
            }
            
            memcpy(buf + current_len, key->getCStringNoCopy(), key_len);
            buf[current_len + key_len] = 0;
            current_len += key_len + 1;
            (*items)++;
        }
        
        iterator->release();
    }
    
    if (*neededLength != 0) return false;
    return true;
}

bool UMVHIDService::getDeviceState(char *product_string, UInt8 product_string_len, UInt32 &state) {
    
    OSString *key = nullptr;
    UMVHIDDevice *device = nullptr;
    
    if (product_string_len == 0) return false;

    {
        char *cname = (char *)IOMalloc(product_string_len + 1);
        if (!cname) return false;
        memcpy(cname, product_string, product_string_len);
        cname[product_string_len] = 0;
        key = OSString::withCString(cname);
        IOFree(cname, product_string_len + 1);
    }

    if (!key) goto end;

    device = (UMVHIDDevice *)m_hid_devices->getObject(key);

    if (!device) {
        
        // device does not exist
        state = 0x00;
    } else {
        
        state = device->getState();
    }
    
    key->release();
    
    return true;
    
end:
    if (key) key->release();
    return false;
}