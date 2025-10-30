#include <stdio.h>
#include "smart_sensor.h"

smart_sensor_t* smart_sensor;

void print_data() {
    printf("\n--Smart Sensor Data--\n");
    printf("Device: \t%s\n", smart_sensor->device);
    printf("Temperature: \t%.2f°C\n", smart_sensor->temperature);
    printf("Date: \t\t%s\n", smart_sensor->date);
    printf("---------------------\n\n");

    if (strcmp(smart_sensor->device, "N/A") == 0) {
        printf("Device ID is not available. Please try refreshing again.\n\n");
    }
}

int main()
{
    smart_sensor_init_ptr(&smart_sensor);
    smart_sensor_set_on_data_updated(smart_sensor, print_data);

    if (!smart_sensor) {
        printf("Failed to initialize smart sensor.\n");
        return -1;
    }

    printf("\nRefreshing smart sensor data... This may take a few seconds.\n");

    if (smart_sensor_refresh(smart_sensor) != 0) {
        printf("Failed to refresh smart sensor data.\n");
        smart_sensor_dispose(smart_sensor);
        return -1;
    }
    
    smart_sensor_dispose_ptr(&smart_sensor);

    return 0;
}
