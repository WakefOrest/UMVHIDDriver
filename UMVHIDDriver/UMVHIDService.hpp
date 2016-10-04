//
//  UMVHIDService.hpp
//  UMVHIDDriver
//
//  Created by fOrest on 6/25/16.
//  Copyright © 2016 fOrest. All rights reserved.
//

#ifndef UMVHIDService_hpp
#define UMVHIDService_hpp

//#include <stdio.h>
#include <IOKit/IOService.h>
#include "UMVHIDDevice.hpp"

class UMVHIDService : public IOService {
    OSDeclareDefaultStructors(UMVHIDService)
    
public:
    virtual bool init(OSDictionary *dictionary = 0) override;
    virtual void free(void) override;
    
    virtual bool start(IOService *provider) override;
    virtual void stop(IOService *provider) override;

    /**
     *  Create a new virtual device.
     *
     *  @param product_string        A unique device name.
     *  @param product_string_len    Length of device name.
     *  @param report_descriptor     A report descriptor for this device.
     *  @param report_descriptor_len Length of 'report_descriptor'.
     *  @param serial_number         A serial number for the device.
     *  @param serial_number_len     Length of 'serial_number'
     *  @param vendor_id             A vendor ID.
     *  @param product_id            A product ID.
     *
     *  @return True on success.
     */
    virtual bool createDevice(char *product_string, UInt8 product_string_len,
                              unsigned char *report_descriptor, UInt16 report_descriptor_len,
                              char *serial_number = nullptr, UInt16 serial_number_len = 0,
                              UInt32 vendor_id = 0, UInt32 product_id = 0);
    
    /**
     *  Destroy a given device.
     *
     *  @param product_string     A unique device name.
     *  @param product_string_len Length of device name.
     *
     *  @return True on success.
     */
    virtual bool destroyDevice(char *product_string, UInt8 product_string_len);
    
    /**
     *  Send a report descriptor to the device.
     *
     *  @param product_string        A unique device name.
     *  @param product_string_len    Length of 'name'.
     *  @param report_descriptor     A report descriptor for this device.
     *  @param report_descriptor_len Length of 'report_descriptor'.
     *
     *  @return True on success.
     */
    virtual bool handleReport(char *product_string, UInt8 product_string_len,
                            unsigned char *report_descriptor,
                            UInt16 report_descriptor_len);
    
    /**
     *  Return the names of the currently managed virtual devices,
     *  separated by '\x00'.
     *
     *  @param buf     A buffer in which to store the virtual devices' names.
     *  @param buf_len Length of 'buf'.
     *  @param needed  A suggestion for the next size of 'buf', if currenly insufficient.
     *  @param items   The number of returned items.
     *
     *  @return True on success, False on insufficent buffer size.
     */
    virtual bool listDevices(char *buf, UInt16 buf_len,
                            UInt16 *needed, UInt16 *items);
    /**
     *  get the status of a given device.
     *
     *  @param product_string     A unique device name.
     *  @param product_string_len Length of device name.
     *  @param state 
     *              0: device no exist
     *              other:
     *                        kIOServiceInactiveState = 0x00000001,
     *                        kIOServiceRegisteredState   = 0x00000002,
     *                        kIOServiceMatchedState  = 0x00000004,
     *                        kIOServiceFirstPublishState = 0x00000008,
     *                        kIOServiceFirstMatchState   = 0x00000010
     *  @return True if no error ocurred.
     */
    virtual bool getDeviceState(char *product_string, UInt8 product_string_len, UInt32 &state);
    
private:
    /**
     *  Keep track of managed/created HID devices.
     */
    OSDictionary *m_hid_devices;
};


#endif /* UMVHIDService_hpp */
