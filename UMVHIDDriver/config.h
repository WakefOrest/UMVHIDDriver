//
//  config.h
//  UMGameControllerDriver
//
//  Created by fOrest on 6/25/16.
//  Copyright © 2016 fOrest. All rights reserved.
//

#ifndef config_h
#define config_h

#define UMVHIDService                   com_fOrest_umvhid_service
#define UMVHIDDevice                    com_fOrest_umvhid_device
#define UMVHIDClient                    com_fOrest_umvhid_client

#ifndef kMaxHIDReportSize
#define kMaxHIDReportSize               256
#endif

#define CLASS_OBJECT_FORMAT_STRING "[%s@%p:%dx]"
#define CLASS_OBJECT_FORMAT(obj) myClassName(obj), obj, myRefCount(obj)

inline int myRefCount(const OSObject* obj)
{
    return obj ? obj->getRetainCount() : 0;
}

inline const char* myClassName(const OSObject* obj)
{
    if (!obj) return "(null)";
    return obj->getMetaClass()->getClassName();
}

#endif /* config_h */
