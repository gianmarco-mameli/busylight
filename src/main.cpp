#include <Arduino.h>
// #include <stdio.h>
// #include <stdio.h>
// #include <string.h>

// #include "pico/stdlib.h"
// #include "hardware/gpio.h"
// #include "hardware/pwm.h"

// #include "bsp/board.h"
// #ifdef USE_TINYUSB
// #include <Adafruit_TinyUSB.h>
// #endif
// #include <Adafruit_TinyUSB.h>
#include "tusb.h"
#ifdef CFG_TUSB_CONFIG_FILE
    #include CFG_TUSB_CONFIG_FILE
#else
    #include "cust_tusb_config.h"
#endif

#include "usb_descriptors.h"

#include "overrides.h"

// #define LED_1_RED_GPIO 5
// #define LED_1_GREEN_GPIO 7
// #define LED_1_BLUE_GPIO 6

// #define PWM_COUNT_TOP 100

//Function Prototypes
void btn_callback(uint gpio, uint32_t events);
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
// Invoked when received control request with VENDOR TYPE
// bool tud_vendor_control_request_cb(uint8_t rhport, tusb_control_request_t const * request) {
    bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const* request) {
    printf("DEBUG: tud_vendor_control_request_cb triggered\n");
    (void) rhport;
    (void) request;

    return 0;
}
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


void setup(void) {
    // stdio_init_all(); //Initialize debug interface
    // printf("DEBUG: starting up buildcomics HID device...\n");
    // Serial.begin(115200);
    // board_init();
    // board_init();
    tusb_init();

    // if(TinyUSBDevice.mounted()){
    //     digitalWrite(LED_BUILTIN, HIGH);
    // }
    // printf("DEBUG: HID Device initialized\n");

    pinMode(LED_BUILTIN, OUTPUT);

    //setup LED PWM
    // pwm_config cfg = pwm_get_default_config();
    // pwm_config_set_wrap(&cfg, PWM_COUNT_TOP);
    // pwm_init(pwm_gpio_to_slice_num(LED_1_RED_GPIO), &cfg, true);
    // pwm_init(pwm_gpio_to_slice_num(LED_1_GREEN_GPIO), &cfg, true);
    // pwm_init(pwm_gpio_to_slice_num(LED_1_BLUE_GPIO), &cfg, true);

    // gpio_set_function(LED_1_RED_GPIO, GPIO_FUNC_PWM);
    // gpio_set_function(LED_1_GREEN_GPIO, GPIO_FUNC_PWM);
    // gpio_set_function(LED_1_BLUE_GPIO, GPIO_FUNC_PWM);

    //Cycle through led colours as part of startup test
    // pwm_set_gpio_level(LED_1_RED_GPIO, 1 * (PWM_COUNT_TOP + 1));
    // sleep_ms(500);
    // pwm_set_gpio_level(LED_1_RED_GPIO, 0 );
    // pwm_set_gpio_level(LED_1_GREEN_GPIO, 1 * (PWM_COUNT_TOP + 1));
    // sleep_ms(500);
    // pwm_set_gpio_level(LED_1_GREEN_GPIO, 0 );
    // pwm_set_gpio_level(LED_1_BLUE_GPIO, 1 * (PWM_COUNT_TOP + 1));
    // MAIN LOOP

}

void loop() {
        hid_task();
        tud_task(); // tinyusb device task
        printf("DEBUG: HID Device initialized\n");
        // digitalWrite(LED_BUILTIN, HIGH);
        // delay(1000);
        // digitalWrite(LED_BUILTIN, LOW);
        // delay(1000);
}