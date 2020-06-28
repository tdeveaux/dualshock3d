#include <IOKit/hid/IOHIDLib.h>

static void parse_report(void *context, IOReturn result, void *sender, IOHIDReportType type, uint32_t reportID, uint8_t *report, CFIndex reportLength) {
  uint8_t output[49] = {
    0x01,
    0x00, 0x96, 0x00, 0x96, 0x00,
    0xff, 0x7e, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00
  };
  if (report[30] == 0x02) {
    output[27] = 0x03;
    output[28] = 0xeb;
    output[29] = 0x7d;
    output[30] = 0x7d;
  }
  if (report[30] == 0xee) {
    output[27] = 0x27;
    output[28] = 0x10;
    output[29] = 0x32;
    output[30] = 0x32;
  }
  IOHIDDeviceSetReport(sender, kIOHIDReportTypeOutput, output[0], output, reportLength);
  IOHIDDeviceRegisterInputReportCallback(sender, report, reportLength, NULL, NULL);
}

static void matched(void *context, IOReturn result, void *sender, IOHIDDeviceRef device) {
  uint8_t report[49];
  CFIndex reportLength = sizeof(report);
  IOHIDDeviceRegisterInputReportCallback(device, report, reportLength, parse_report, NULL);
}

static void removed(void *context, IOReturn result, void *sender, IOHIDDeviceRef device) {
  CFRunLoopStop(CFRunLoopGetMain());
}

int main(void) {
  IOHIDManagerRef manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
  const void *keys[] = {
    CFSTR(kIOHIDVendorIDKey),
    CFSTR(kIOHIDProductIDKey)
  };
  const uint32_t vendorID = 0x054c;
  const uint32_t productID = 0x0268;
  const void *values[] = {
    CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &vendorID),
    CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &productID)
  };
  CFDictionaryRef matching = CFDictionaryCreate(kCFAllocatorDefault, keys, values, 2, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  IOHIDManagerSetDeviceMatching(manager, matching);
  CFRelease(matching);
  CFRelease(values[0]);
  CFRelease(values[1]);
  IOHIDManagerRegisterDeviceMatchingCallback(manager, matched, NULL);
  IOHIDManagerRegisterDeviceRemovalCallback(manager, removed, NULL);
  IOHIDManagerScheduleWithRunLoop(manager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
  IOHIDManagerOpen(manager, kIOHIDOptionsTypeNone);
  CFRunLoopRun();
  return 0;
}
