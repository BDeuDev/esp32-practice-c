#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "driver/gpio.h"

#define BLINK_LED_GPIO GPIO_NUM_2

void app_main(void)
{
    gpio_reset_pin(BLINK_LED_GPIO);
    gpio_set_direction(BLINK_LED_GPIO, GPIO_MODE_OUTPUT);

    gpio_set_level(BLINK_LED_GPIO, 1);
    for (int i = 10; i >= 0; i--)
    {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    gpio_set_level(BLINK_LED_GPIO, 0);
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
