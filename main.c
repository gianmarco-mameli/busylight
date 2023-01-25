#include <stdio.h>
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

#include "bsp/board.h"
#include "tusb.h"

#include "usb_descriptors.h"

#include "pico/cyw43_arch.h"
#include <private.h>

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

#define LED_1_RED_GPIO 5
#define LED_1_GREEN_GPIO 7
#define LED_1_BLUE_GPIO 6

#define PWM_COUNT_TOP 100

#include "lwip/apps/mqtt.h"
#include "mqtt.c"

#define MQTT_IP "192.168.1.1"
mqtt_client_t* client;

// static int inpub_id;

// static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
// {
//   printf("Incoming publish at topic %s with total length %u\n", topic, (unsigned int)tot_len);

//   /* Decode topic string into a user defined reference */
//   if(strcmp(topic, "print_payload") == 0) {
//     inpub_id = 0;
//   } else if(topic[0] == 'A') {
//     /* All topics starting with 'A' might be handled at the same way */
//     inpub_id = 1;
//   } else {
//     /* For all other topics */
//     inpub_id = 2;
//   }
// }

// static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
// {
//   printf("Incoming publish payload with length %d, flags %u\n", len, (unsigned int)flags);

//   if(flags & MQTT_DATA_FLAG_LAST) {
//     /* Last fragment of payload received (or whole part if payload fits receive buffer
//        See MQTT_VAR_HEADER_BUFFER_LEN)  */

//     /* Call function or do action depending on reference, in this case inpub_id */
//     if(inpub_id == 0) {
//       /* Don't trust the publisher, check zero termination */
//       if(data[len-1] == 0) {
//         printf("mqtt_incoming_data_cb: %s\n", (const char *)data);
//       }
//     } else if(inpub_id == 1) {
//       /* Call an 'A' function... */
//     } else {
//       printf("mqtt_incoming_data_cb: Ignoring payload...\n");
//     }
//   } else {
//     /* Handle fragmented payload, store in buffer, write to file or whatever */
//   }
// }

// static void mqtt_sub_request_cb(void *arg, err_t result)
// {
//   /* Just print the result code here for simplicity,
//      normal behaviour would be to take some action if subscribe fails like
//      notifying user, retry subscribe or disconnect from server */
//   printf("Subscribe result: %d\n", result);
// }

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
  // err_t err;
  // if(status == MQTT_CONNECT_ACCEPTED) {
  //   printf("mqtt_connection_cb: Successfully connected\n");

  //   /* Setup callback for incoming publish requests */
  //   // mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, arg);

  //   /* Subscribe to a topic named "subtopic" with QoS level 1, call mqtt_sub_request_cb with result */
  //   // err = mqtt_subscribe(client, "subtopic", 1, mqtt_sub_request_cb, arg);



  //   // if(err != ERR_OK) {
  //   //   printf("mqtt_subscribe return: %d\n", err);
  //   // }
  // } else {
  //   printf("mqtt_connection_cb: Disconnected, reason: %d\n", status);

  //   /* Its more nice to be connected, so try to reconnect */
  //   // mqtt_init(&client);
  // }
}

/* The idea is to demultiplex topic and create some reference to be used in data callbacks
   Example here uses a global variable, better would be to use a member in arg
   If RAM and CPU budget allows it, the easiest implementation might be to just take a copy of
   the topic string and use it in mqtt_incoming_data_cb
*/

/* Called when publish is complete either with sucess or failure */
static void mqtt_pub_request_cb(void *arg, err_t result)
{
  if(result != ERR_OK) {
    printf("Publish result: %d\n", result);
  }
}

void publish(mqtt_client_t *client, const char *topic, const char *payload, void *arg)
{
  err_t err;
  u8_t qos = 1; /* 0 1 or 2, see MQTT specification */
  u8_t retain = 0; /* No don't retain such crappy payload... */
  err = mqtt_publish(client, topic, payload, strlen(payload), qos, retain, mqtt_pub_request_cb, arg);
  if(err != ERR_OK) {
    printf("Publish err: %d\n", err);
  }
}

void mqtt_init(mqtt_client_t *client)
{
  ip_addr_t ip_addr;
  ip4_addr_set_u32(&ip_addr, ipaddr_addr(MQTT_IP));

  struct mqtt_connect_client_info_t ci;
  err_t err;

  /* Setup an empty client info structure */
  memset(&ci, 0, sizeof(ci));

  /* Minimal amount of information required is client identifier, so set it here */
  ci.client_id = "busylight";
  ci.will_topic = "busylight/status";
  ci.will_msg = "connected";
  ci.client_user = NULL;
  ci.client_pass = NULL;
  ci.keep_alive = 100;

  /* Initiate client and connect to server, if this fails immediately an error code is returned
     otherwise mqtt_connection_cb will be called with connection result after attempting
     to establish a connection with the server.
     For now MQTT version 3.1.1 is always used */

  // client = mqtt_client_new();

  // if (client != NULL){
    err = mqtt_client_connect(client, &ip_addr, MQTT_PORT, mqtt_connection_cb, LWIP_CONST_CAST(void*, &ci), &ci);
    while (err != ERR_OK)
    {
      sleep_ms(250);
      err = mqtt_client_connect(client, &ip_addr, MQTT_PORT, mqtt_connection_cb, LWIP_CONST_CAST(void*, &ci), &ci);
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    }
    while (!mqtt_client_is_connected(client)){
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
      // sleep_ms(250);
    }
  // }




  /* For now just print the result code if something goes wrong */
  // if(err != ERR_OK) {
  //   printf("mqtt_connect return %d\n", err);
  // }
}

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

    if (board_millis() - start_ms < interval_ms) return; // not enough time
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
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
    // TODO not Implemented
    printf("DEBUG: tud_hid_get_report_cb triggered\n");
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {    printf("DEBUG: tud_hid_set_report_cb triggered\n");
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

    if (strcmp(buffer, setup_request_string) == 0) {
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
        pwm_set_gpio_level(LED_1_RED_GPIO, buffer[2]);
        pwm_set_gpio_level(LED_1_GREEN_GPIO, buffer[3]);
        pwm_set_gpio_level(LED_1_BLUE_GPIO, buffer[4]);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        printf("on\n");
        publish(client,"busilight/r",buffer[2],"");
        publish(client,"busilight/g",buffer[3],"");
        publish(client,"busilight/b",buffer[4],"");
        sleep_ms(500);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        printf("off\n");
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









int main() {
  stdio_init_all(); //Initialize debug interface

  if (cyw43_arch_init_with_country(CYW43_COUNTRY_ITALY)) {
      printf("failed to initialise\n");
      return 1;
  }
  printf("initialised\n");

  cyw43_arch_enable_sta_mode();

  if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
      printf("failed to connect\n");
      return 1;
  }
  printf("connected\n");

  sleep_ms(1000);

  client = mqtt_client_new();

  mqtt_init(client);

  publish(client,"busylight/r","0","");
  publish(client,"busylight/g","0","");
  publish(client,"busylight/b","0","");

  printf("DEBUG: starting up buildcomics HID device...\n");

  board_init();
  tusb_init();
  printf("DEBUG: HID Device initialized\n");

  // setup LED PWM
  pwm_config cfg = pwm_get_default_config();
  pwm_config_set_wrap(&cfg, PWM_COUNT_TOP);
  pwm_init(pwm_gpio_to_slice_num(LED_1_RED_GPIO), &cfg, true);
  pwm_init(pwm_gpio_to_slice_num(LED_1_GREEN_GPIO), &cfg, true);
  pwm_init(pwm_gpio_to_slice_num(LED_1_BLUE_GPIO), &cfg, true);

  gpio_set_function(LED_1_RED_GPIO, GPIO_FUNC_PWM);
  gpio_set_function(LED_1_GREEN_GPIO, GPIO_FUNC_PWM);
  gpio_set_function(LED_1_BLUE_GPIO, GPIO_FUNC_PWM);

  //Cycle through led colours as part of startup test
  pwm_set_gpio_level(LED_1_RED_GPIO, 1 * (PWM_COUNT_TOP + 1));
  sleep_ms(500);
  pwm_set_gpio_level(LED_1_RED_GPIO, 0 );
  pwm_set_gpio_level(LED_1_GREEN_GPIO, 1 * (PWM_COUNT_TOP + 1));
  sleep_ms(500);
  pwm_set_gpio_level(LED_1_GREEN_GPIO, 0 );
  pwm_set_gpio_level(LED_1_BLUE_GPIO, 1 * (PWM_COUNT_TOP + 1));

  // MAIN LOOP
  while (true){
      hid_task();
      tud_task(); // tinyusb device task
  }
  return 0;
}
