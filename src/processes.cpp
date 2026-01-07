#include <Arduino.h>
#include "processes.h"

void backgroundUpdate(void *pvParameters) {
    (void) pvParameters;

    for(;;) {
        lv_task_handler();
        vTaskDelay(100);
    }
}
