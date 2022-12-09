#ifndef __DELAY__
#define __DELAY__

#include "esp_spi_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define delay_us(A) ets_delay_us(A)
#define delay_ms(A) vTaskDelay(A / portTICK_PERIOD_MS)

#endif