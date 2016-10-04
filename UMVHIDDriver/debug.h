//
//  Header.h
//  UMGameControllerDriver
//
//  Created by fOrest on 6/25/16.
//  Copyright © 2016 fOrest. All rights reserved.
//

#ifndef Header_h
#define Header_h


#include <IOKit/IOLib.h>
#include <libkern/OSDebug.h>

#define TAG "UMVHID: "

#ifdef DEBUG



#define LogD(fmt, ...) IOLog((TAG fmt "\n"), ##__VA_ARGS__)

#else /* DEBUG */

#define LogD(...)

#endif /* DEBUG */

#endif /* Header_h */
