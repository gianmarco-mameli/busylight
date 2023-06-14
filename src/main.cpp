#include "Arduino.h"
#include <Adafruit_TinyUSB.h>
#include "usb_descriptors.h"

void hid_task(void);

// Invoked when device is mounted
void tud_mount_cb(void) {
    printf("DEBUG: MOUNTED\n");
}

// Invoked when device is unmounted
void tud_umount_cb(void) {
    printf("DEBUG: Unmounted\n");
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
    (void) remote_wakeup_en;
    printf("DEBUG: Suspended\n");
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {
    printf("DEBUG: Resumed Mounted\n");
}

// USB HID main task
void hid_task(void) {
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if (millis() - start_ms < interval_ms) return; // not enough time
    start_ms += interval_ms;

    uint32_t const btn = 1;

    // Remote wakeup
    if (tud_suspended() && btn) {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    }
}

// // Invoked when received control request with VENDOR TYPE
// bool tud_vendor_control_request_cb(uint8_t rhport, tusb_control_request_t const * request) {
//     printf("DEBUG: tud_vendor_control_request_cb triggered\n");
//     (void) rhport;
//     (void) request;

//     return 0;
// }

// Invoked when vendor control request is complete
bool tud_vendor_control_complete_cb(uint8_t rhport, tusb_control_request_t const * request) {
    // TODO not Implemented
    printf("DEBUG: tud_vendor_control_complete_cb triggered\n");
    (void) rhport;
    (void) request;

    return 0;
}

//attempt to use:
// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
// uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
//     // TODO not Implemented
//     printf("DEBUG: tud_hid_get_report_cb triggered\n");
//     (void) report_id;
//     (void) report_type;
//     (void) buffer;
//     (void) reqlen;

//     return 0;
// }

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
// void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {    printf("DEBUG: tud_hid_set_report_cb triggered\n");
//     printf("DEBUG: report_id: %X\n", report_id);
//     printf("DEBUG: report_type: %X\n", report_type);
//     printf("DEBUG: bufsize: %d\n", bufsize);
//     const char setup_request_string[] = {
//         0x8f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//         0x06, 0x04, 0x55, 0xff, 0xff, 0xff, 0x03, 0xeb
//     };

//     if (strcmp((char *)buffer, setup_request_string) == 0) {
//         printf("DEBUG: Matching setup request string, answering\n");
//         const char setup_request_return[] = {
//             0x30, 0x30, 0x30, 0x31, 0x50, 0x4c, 0x45, 0x4e,
//             0x4f, 0x4d, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31,
//             0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31,
//             0x44, 0x41, 0x53, 0x41, 0x4e, 0x30, 0x30, 0x30,
//             0x32, 0x30, 0x31, 0x35, 0x30, 0x35, 0x32, 0x38,
//             0x30, 0x32, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30,
//             0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
//             0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30
//         };
//         tud_hid_report(0, &setup_request_return, sizeof(setup_request_return));
//     }
//     else if(bufsize == 64 && (buffer[0] == 0x10 || buffer[0] == 0x11)) {
//         printf("DEBUG light command received:\n");
//         // put_rgb((int)buffer[3], (int)buffer[2], (int)buffer[4]);
//     }
//     else {
//         printf("DEBUG, not matching setup string BUFFER CONTENT:\n");
//         for (int i = 0; i < bufsize; i++) {
//             printf("%02X ", buffer[i]);
//         }
//         printf("\n - End \n");
//     }

//     (void) report_id;
//     (void) report_type;
//     (void) buffer;
//     (void) bufsize;
// }

void setup()
{
  tusb_init();
  printf("DEBUG: HID Device initialized\n");
}

void loop()
{
      hid_task();
      tud_task(); // tinyusb device task
}