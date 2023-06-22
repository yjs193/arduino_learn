#include "DFRobot_ESP32_Radio.h"

#include <stdlib.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_log.h"

#define RADIO_CHANNEL_DEFAULT 13
#define RADIO_QUEUE_SIZE_DEFUALT 5
#define MAX 5

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE_MQTT 0
#else
#define ARDUINO_RUNNING_CORE_MQTT 1
#endif

onDataStrPacketReceivedCb onDataStrPacketReceived = NULL;
onDataStrPacketReceivedCb1 strPacketReceived[MAX] = {NULL, NULL, NULL, NULL, NULL};
String temp[MAX];
static QueueHandle_t radio_recv_queue = NULL;
static uint8_t radio_channel = RADIO_CHANNEL_DEFAULT;
String data1 = "";
static uint8_t cnt = 0;
static const uint8_t broadcast_mac[ESP_NOW_ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static bool radio_inited = false;
bool wifi_started = false;

typedef struct
{
    uint8_t mac_address[ESP_NOW_ETH_ALEN];
    uint8_t *data;
    uint8_t data_len;
} radio_recv_event_t;

void radio_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len)
{
    radio_recv_event_t evt;
    memcpy(evt.mac_address, mac_addr, ESP_NOW_ETH_ALEN);
    evt.data = (uint8_t *)malloc(len);
    if (evt.data != NULL)
    {
        memcpy(evt.data, data, len);
        evt.data_len = len;
        if (xQueueSend(radio_recv_queue, &evt, 0) == pdFALSE)
        {
            free(evt.data);
        }
    }
}

static void radioTask(void *param)
{
    while (1)
    {
        if (radio_recv_queue != 0)
        {
            radio_recv_event_t *p = (radio_recv_event_t *)malloc(sizeof(radio_recv_event_t));
            if (xQueueReceive(radio_recv_queue, p, 0) == pdTRUE)
            {
                char *aa = (char *)malloc(p->data_len + 1);
                for (uint8_t i = 0; i < p->data_len; i++)
                {
                    aa[i] = p->data[i];
                }
                aa[p->data_len] = '\0';
                String buf = (String)aa;
                data1 = buf;
                if (onDataStrPacketReceived != NULL)
                {
                    onDataStrPacketReceived(String(buf));
                }
                for (uint8_t j = 0; j < MAX; j++)
                {
                    if (strPacketReceived[j] != NULL && buf == temp[j])
                    {
                        strPacketReceived[j]();
                    }
                }
                free(aa);
            }
            free(p);
        }
        yield();
    }
}

DFRobot_ESP32Radio::DFRobot_ESP32Radio()
{
    radio_channel = RADIO_CHANNEL_DEFAULT;
    wifi_started = false;
    radio_inited = false;
}

void DFRobot_ESP32Radio::setGroup(uint8_t channel)
{
    if (!(0 <= channel && channel <= 14))
    {
        return;
    }
    radio_channel = channel;
    esp_wifi_set_channel(channel, (wifi_second_chan_t)0);
}

void DFRobot_ESP32Radio::send(const char *msg)
{
    if (radio_inited)
    {
        esp_now_send(broadcast_mac, (const uint8_t *)msg, strlen(msg));
    }
}
void DFRobot_ESP32Radio::send(String msg)
{
    if (radio_inited)
    {
        esp_now_send(broadcast_mac, (const uint8_t *)msg.c_str(), msg.length());
    }
}

void DFRobot_ESP32Radio::send(int32_t msg)
{
    char buffer[32];
    memset(buffer, 0, 32);
    itoa(msg, buffer, 10);
    if (radio_inited)
    {
        esp_now_send(broadcast_mac, (const uint8_t *)buffer, strlen(buffer));
    }
}

void DFRobot_ESP32Radio::send(uint32_t msg)
{
    char buffer[32];
    memset(buffer, 0, 32);
    itoa(msg, buffer, 10);
    if (radio_inited)
    {
        esp_now_send(broadcast_mac, (const uint8_t *)buffer, strlen(buffer));
    }
}

#define MAX_PRECISION (10)

static const double rounders[MAX_PRECISION + 1] = {
    0.5,          // 0
    0.05,         // 1
    0.005,        // 2
    0.0005,       // 3
    0.00005,      // 4
    0.000005,     // 5
    0.0000005,    // 6
    0.00000005,   // 7
    0.000000005,  // 8
    0.0000000005, // 9
    0.00000000005 // 10
};

void DFRobot_ESP32Radio::send(double f, int precision)
{
    char buf[80];
    char *ptr = buf;
    char *p = ptr;
    char *p1;
    char c;
    long intPart;

    if (precision > MAX_PRECISION)
        precision = MAX_PRECISION;
    if (f < 0)
    {
        f = -f;
        *ptr++ = '-';
    }
    if (precision > 0)
    { // negative precision == automatic precision guess
        if (f < 1.0)
            precision = 6;
        else if (f < 10.0)
            precision = 5;
        else if (f < 100.0)
            precision = 4;
        else if (f < 1000.0)
            precision = 3;
        else if (f < 10000.0)
            precision = 2;
        else if (f < 100000.0)
            precision = 1;
        else
            precision = 0;
    }
    if (precision)
        f += rounders[precision];
    intPart = f;
    f -= intPart;
    if (!intPart)
        *ptr++ = '0';
    else
    {
        p = ptr;
        while (intPart)
        {
            *p++ = '0' + intPart % 10;
            intPart /= 10;
        }
        p1 = p;
        while (p > ptr)
        {
            c = *--p;
            *p = *ptr;
            *ptr++ = c;
        }
        ptr = p1;
    }
    if (precision)
    {
        *ptr++ = '.';
        while (precision--)
        {
            f *= 10.0;
            c = f;
            *ptr++ = '0' + c;
            f -= c;
        }
    }

    *ptr = 0;

    for (int i = strlen(buf) - 1; i >= 0; i--)
    {
        if (buf[i] == '.')
            break;
        if (buf[i] == '0')
        {
            buf[i] = '\0';
            if (buf[i - 1] == '.')
            {
                buf[i - 1] = '\0';
                break;
            }
        }
        else
            break;
    }
    
    String message = (String)buf;
    if (radio_inited)
    {
        esp_now_send(broadcast_mac, (const uint8_t *)message.c_str(), message.length());
    }
}

/*
static bool wifi_sta_connected = false;
static uint8_t wifi_sta_disconn_reason = 0;
bool wifi_sta_connect_requested = true;
*/
void DFRobot_ESP32Radio::turnOn(void)
{
    if (!radio_inited)
    {
        if (!wifi_started)
        {
            wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
            ESP_ERROR_CHECK(esp_wifi_init(&cfg));
            ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
            ESP_ERROR_CHECK(esp_wifi_start());
            wifi_started = true;
        }
        ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
        ESP_ERROR_CHECK(esp_wifi_set_channel(radio_channel, WIFI_SECOND_CHAN_NONE)); //bug

        // espnow init
        radio_recv_queue = xQueueCreate(RADIO_QUEUE_SIZE_DEFUALT, sizeof(radio_recv_event_t));
        if (radio_recv_queue == NULL)
        {
            return;
        }
        ESP_ERROR_CHECK(esp_now_init());
        ESP_ERROR_CHECK(esp_now_register_recv_cb(radio_recv_cb));
        //        ESP_ERROR_CHECK(esp_now_register_recv_cb(radio_recv_cb));
        ESP_ERROR_CHECK(esp_now_set_pmk((uint8_t *)"mpython-2019"));

        // add broadcast peer
        esp_now_peer_info_t *peer = (esp_now_peer_info_t *)malloc(sizeof(esp_now_peer_info_t));
        if (peer == NULL)
        {
            esp_now_deinit();
        }
        memset(peer, 0, sizeof(esp_now_peer_info_t));
        peer->channel = 0; // radio_channel;
        wifi_mode_t mode;
        esp_wifi_get_mode(&mode);
        peer->ifidx = (mode == WIFI_MODE_STA) ? ESP_IF_WIFI_STA : ESP_IF_WIFI_AP;
        peer->encrypt = false;
        memcpy(peer->peer_addr, broadcast_mac, ESP_NOW_ETH_ALEN);
        ESP_ERROR_CHECK(esp_now_add_peer(peer));
        free(peer);
        radio_inited = true;
        xTaskCreatePinnedToCore(radioTask, "radioTask", 2048, NULL, 1, NULL, ARDUINO_RUNNING_CORE_MQTT);
    }
}

void DFRobot_ESP32Radio::turnOff(void)
{
    if (radio_inited)
    {
        if (radio_recv_queue != NULL)
        {
            vQueueDelete(radio_recv_queue);
        }
        esp_now_deinit();
        esp_wifi_set_promiscuous(false);
        radio_inited = false;
    }
}

void DFRobot_ESP32Radio::setCallback(onDataStrPacketReceivedCb cb)
{
    onDataStrPacketReceived = cb;
}

void DFRobot_ESP32Radio::setCallback(String value, onDataStrPacketReceivedCb1 cb)
{
    strPacketReceived[cnt] = cb;
    temp[cnt] = value;
    cnt++;
}

String DFRobot_ESP32Radio::receive(void)
{
    String val = "";
    if (data1 != "")
    {
        val = data1;
        data1 = "";
    }
    return val;
}
