#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "driver/gpio.h"

#define LED_RED_GPIO GPIO_NUM_4
#define LED_YELLOW_GPIO GPIO_NUM_2
#define LED_GREEN_GPIO GPIO_NUM_15
#define LEDS 3

void configure_gpio(gpio_num_t *gpio)
{
    for (size_t i = 0; i < LEDS; i++)
    {
        gpio_reset_pin(gpio[i]);
        gpio_set_direction(gpio[i], GPIO_MODE_OUTPUT);
    }
}

void toggle_led(gpio_num_t gpio)
{
    int level = gpio_get_level(gpio);

    if (level)
    {
        gpio_set_level(gpio, 0);
    }
    else
    {
        gpio_set_level(gpio, 1);
    }
}

void led_task(void *arg)
{
    gpio_num_t *pins = (gpio_num_t *)arg;

    while (1)
    {
        gpio_set_level(pins[0], 1);
        gpio_set_level(pins[1], 0);
        gpio_set_level(pins[2], 0);
        vTaskDelay(pdMS_TO_TICKS(5000));

        gpio_set_level(pins[0], 0);
        gpio_set_level(pins[1], 1);
        gpio_set_level(pins[2], 0);
        vTaskDelay(pdMS_TO_TICKS(2000));

        gpio_set_level(pins[0], 0);
        gpio_set_level(pins[1], 0);
        gpio_set_level(pins[2], 1);
        vTaskDelay(pdMS_TO_TICKS(5000));

        gpio_set_level(pins[0], 0);
        gpio_set_level(pins[1], 1);
        gpio_set_level(pins[2], 0);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

static gpio_num_t pins[] = {
    LED_RED_GPIO,
    LED_YELLOW_GPIO,
    LED_GREEN_GPIO};

void app_main(void)
{
    configure_gpio(pins);

    xTaskCreate(
        led_task,
        "led_task",
        2048,
        pins,
        1,
        NULL);
}
