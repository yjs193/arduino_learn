#ifndef DF_ESP32_RADIO_H
#define DF_ESP32_RADIO_H
#include "Arduino.h"

#include <stdlib.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_log.h"

#define RADIO_QUEUE_SIZE_DEFUALT 5
#define RADIO_CHANNEL_DEFAULT 7

typedef void (*onDataStrPacketReceivedCb) (String message);
typedef void (*onDataStrPacketReceivedCb1) (void);

typedef int32_t mp_int_t; // must be pointer size
typedef uint32_t mp_uint_t; // must be pointer size
typedef long mp_off_t;


class DFRobot_ESP32Radio
{
  public:
    uint8_t radio_channel;
    bool radio_inited;
    bool wifi_started;

    DFRobot_ESP32Radio();
    void setGroup(uint8_t channel);
    void send(const char *msg);
    void send(String msg);
    void send(int32_t msg);
    void send(uint32_t msg);
    void send(double f, int precision=10);
    String receive(void);

//	void radio_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len);
    void turnOn(void);
    void turnOff(void);
    void setCallback(onDataStrPacketReceivedCb cb);
	void setCallback(String value, onDataStrPacketReceivedCb1 cb);
  protected:

};

#endif
