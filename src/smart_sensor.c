#include "smart_sensor.h"
#include <string.h>

int smart_sensor_init(smart_sensor_t* _smart_sensor)
{
    if (!_smart_sensor) return -1;

    _smart_sensor->device = NULL;
    _smart_sensor->date = NULL;
    _smart_sensor->temperature = 0.0f;
    _smart_sensor->on_data_updated_cb = NULL;

    return 0;
}

int smart_sensor_init_ptr(smart_sensor_t** _smart_sensor)
{
    if (!_smart_sensor) return -1;

    *_smart_sensor = malloc(sizeof(smart_sensor_t));
    if (!*_smart_sensor) return -1;

    smart_sensor_init(*_smart_sensor);

    return 0;
}

int smart_sensor_set_on_data_updated(smart_sensor_t *_smart_sensor, void (*_callback)())
{
    if (!_smart_sensor) return -1;
    _smart_sensor->on_data_updated_cb = _callback;
    return 0;
}

int smart_sensor_refresh(smart_sensor_t* _smart_sensor)
{
    if (!_smart_sensor) return -1;

    _smart_sensor->device = NULL;
    _smart_sensor->date = NULL;
    _smart_sensor->temperature = 0.0f;

    tcp_client_t client;
    if (tcp_client_init(&client) != 0) {
        return -1;
    }

    if (tcp_client_connect(&client, "httpbin.org", "80") != 0) {
        tcp_client_dispose(&client);
        return -1;
    }

    const char* json_body = 
        "{\n"
        "  \"device\": \"UUID\",\n"
        "  \"time\": \"2025-10-29T10:00:00Z\",\n"
        "  \"temperature\": \"22.5°C\"\n"
        "}";

    char* http_request;
    http_build_post("httpbin.org", "/post", "application/json", json_body, &http_request);

    if (tcp_client_write(&client, (const uint8_t*)http_request, strlen(http_request)) < 0) {
        printf("Failed to send HTTP request.\n");
        tcp_client_dispose(&client);
        return -1;
    }

    uint8_t buffer[4096];
    int received = tcp_client_read(&client, buffer, sizeof(buffer) - 1);
    if (received < 0) {
        printf("Failed to read HTTP response.\n");
        tcp_client_dispose(&client);
        return -1;
    }
    buffer[received] = '\0';

    http_response_t* resp = NULL;
    if (http_parse_response((const char*)buffer, &resp) == 0) {
        http_response_dispose(resp);
    } else {
        printf("Failed to parse HTTP response.\n");
    }

    char* needle;

    needle = strstr((const char*)buffer, "\"device\":");
    char device[64] = {0};
    if (needle) {
        sscanf(needle, "\"device\": \"%63[^\"]\"", device);
    } else {
        strcpy(device, "N/A");
    }

    needle = strstr((const char*)buffer, "\"temperature\":");
    char temperature[16] = {0};
    if (needle) {
        sscanf(needle, "\"temperature\": \"%15[^\"]\"", temperature);
    } else {
        strcpy(temperature, "0.0");
    }

    needle = strstr((const char*)buffer, "\"time\":");
    char date[64] = {0};
    if (needle) {
        sscanf(needle, "\"time\": \"%63[^\"]\"", date);
    } else {
        strcpy(date, "N/A");
    }

    _smart_sensor->device = strdup(device);
    _smart_sensor->temperature = strtof(temperature, NULL);
    _smart_sensor->date = strdup(date);

    tcp_client_dispose(&client);

    _smart_sensor->on_data_updated_cb();

    return 0;
}

int smart_sensor_read_device(smart_sensor_t* _smart_sensor, char* _out_device)
{
    if (!_smart_sensor || !_out_device) return -1;
    strcpy(_out_device, _smart_sensor->device);
    return 0;
}

int smart_sensor_read_temperature(smart_sensor_t* _smart_sensor, float* _out_temperature)
{
    if (!_smart_sensor || !_out_temperature) return -1;
    *_out_temperature = _smart_sensor->temperature;
    return 0;
}

int smart_sensor_read_date(smart_sensor_t* _smart_sensor, char* _out_date)
{
    if (!_smart_sensor || !_out_date) return -1;
    strcpy(_out_date, _smart_sensor->date);
    return 0;
}

void smart_sensor_dispose(smart_sensor_t* _smart_sensor)
{
    if (!_smart_sensor) return;

    free(_smart_sensor->device);
    free(_smart_sensor->date);

    _smart_sensor->device = NULL;
    _smart_sensor->date = NULL;
    _smart_sensor->temperature = 0.0f;
    _smart_sensor->on_data_updated_cb = NULL;
}

void smart_sensor_dispose_ptr(smart_sensor_t** _smart_sensor)
{
    if (!_smart_sensor || !*_smart_sensor) return;

    smart_sensor_dispose(*_smart_sensor);
    free(*_smart_sensor);
    *_smart_sensor = NULL;
}
