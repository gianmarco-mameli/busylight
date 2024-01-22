// #include <Adafruit_TinyUSB.h>
#include "tusb.h"
#include "overrides.h"
#include "usb_descriptors.h"

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
    // TODO not Implemented
    printf("DEBUG: tud_hid_get_report_cb triggered\n");
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}
// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
    printf("DEBUG: tud_hid_set_report_cb triggered\n");
    printf("DEBUG: report_id: %X\n", report_id);
    printf("DEBUG: report_type: %X\n", report_type);
    printf("DEBUG: bufsize: %d\n", bufsize);
    const char setup_request_string[] = {
        0x8f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x06, 0x04, 0x55, 0xff, 0xff, 0xff, 0x03, 0xeb
    };

    if (strcmp((char*)buffer, setup_request_string) == 0) {
        printf("DEBUG: Matching setup request string, answering\n");
        const char setup_request_return[] = {
            0x30, 0x30, 0x30, 0x31, 0x50, 0x4c, 0x45, 0x4e,
            0x4f, 0x4d, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31,
            0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31,
            0x44, 0x41, 0x53, 0x41, 0x4e, 0x30, 0x30, 0x30,
            0x32, 0x30, 0x31, 0x35, 0x30, 0x35, 0x32, 0x38,
            0x30, 0x32, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30,
            0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
            0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30
        };
        tud_hid_report(0, &setup_request_return, sizeof(setup_request_return));
    }
    else if(bufsize == 64 && (buffer[0] == 0x10 || buffer[0] == 0x11)) {
        printf("DEBUG light command received:\n");
        printf("DEBUG RED: %d \n", buffer[2]); //2 = red pwm value
        printf("DEBUG GREEN: %d \n", buffer[3]); //3 = green pwm value
        printf("DEBUG BLUE: %d \n", buffer[4]); //4 = blue pwm value
        // pwm_set_gpio_level(LED_1_RED_GPIO, buffer[2]);
        // pwm_set_gpio_level(LED_1_GREEN_GPIO, buffer[3]);
        // pwm_set_gpio_level(LED_1_BLUE_GPIO, buffer[4]);
    }
    else {
        printf("DEBUG, not matching setup string BUFFER CONTENT:\n");
        for (int i = 0; i < bufsize; i++) {
            printf("%02X ", buffer[i]);
        }
        printf("\n - End \n");
    }

    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) bufsize;
}