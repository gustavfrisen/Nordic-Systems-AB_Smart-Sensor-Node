#ifndef _SMART_SENSOR_H
#define _SMART_SENSOR_H

#include "tcp_client.h"
#include "http.h"

typedef struct smart_sensor_t {
    char* device;
    float temperature;
    char* date;
    void (*on_data_updated_cb)();
} smart_sensor_t;

int smart_sensor_init(smart_sensor_t* _smart_sensor);
int smart_sensor_init_ptr(smart_sensor_t** _smart_sensor);
int smart_sensor_set_on_data_updated(smart_sensor_t* _smart_sensor, void (*_callback)());

int smart_sensor_refresh(smart_sensor_t* _smart_sensor);
int smart_sensor_read_device(smart_sensor_t* _smart_sensor, char* _out_device);
int smart_sensor_read_temperature(smart_sensor_t* _smart_sensor, float* _out_temperature);
int smart_sensor_read_date(smart_sensor_t* _smart_sensor, char* _out_date);

void smart_sensor_dispose(smart_sensor_t* _smart_sensor);
void smart_sensor_dispose_ptr(smart_sensor_t** _smart_sensor);

#endif