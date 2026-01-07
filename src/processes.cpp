#include <Arduino.h>
#include "processes.h"

void backgroundUpdate(void *pvParameters) {
    (void) pvParameters;

    for(;;) {
        vTaskDelay(100);
    }
}
