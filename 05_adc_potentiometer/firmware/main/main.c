#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

#define SEG_A GPIO_NUM_23
#define SEG_B GPIO_NUM_21
#define SEG_C GPIO_NUM_18
#define SEG_D GPIO_NUM_4
#define SEG_E GPIO_NUM_2
#define SEG_F GPIO_NUM_22
#define SEG_G GPIO_NUM_19
#define SEG_DP GPIO_NUM_5

#define DIGIT0 GPIO_NUM_14
#define DIGIT1 GPIO_NUM_27
#define DIGIT2 GPIO_NUM_26
#define DIGIT3 GPIO_NUM_25

volatile int display_value = 0;

adc_oneshot_unit_handle_t adc_handle;

const uint8_t numbers[10][7] =
{
    {1,1,1,1,1,1,0},
    {0,1,1,0,0,0,0},
    {1,1,0,1,1,0,1},
    {1,1,1,1,0,0,1},
    {0,1,1,0,0,1,1},
    {1,0,1,1,0,1,1},
    {1,0,1,1,1,1,1},
    {1,1,1,0,0,0,0},
    {1,1,1,1,1,1,1},
    {1,1,1,1,0,1,1}
};

gpio_num_t segmentPins[7] =
{
    SEG_A,
    SEG_B,
    SEG_C,
    SEG_D,
    SEG_E,
    SEG_F,
    SEG_G
};

gpio_num_t digitPins[4] =
{
    DIGIT0,
    DIGIT1,
    DIGIT2,
    DIGIT3
};

void display_digit(int digit, int number)
{
    for (int i = 0; i < 4; i++)
        gpio_set_level(digitPins[i], 1);

    for (int i = 0; i < 7; i++)
        gpio_set_level(segmentPins[i], numbers[number][i]);

    gpio_set_level(SEG_DP, 0);

    gpio_set_level(digitPins[digit], 0);

    vTaskDelay(pdMS_TO_TICKS(2));

    gpio_set_level(digitPins[digit], 1);
}

void display_number(int value)
{
    if(value > 9999)
        value = 9999;

    if(value < 0)
        value = 0;

    display_digit(0, (value / 1000) % 10);
    display_digit(1, (value / 100) % 10);
    display_digit(2, (value / 10) % 10);
    display_digit(3, value % 10);
}

void display_task(void *arg)
{
    while (1)
    {
        display_number(display_value);
    }
}

void adc_task(void *arg)
{
    int value;

    while (1)
    {
        adc_oneshot_read(adc_handle, ADC_CHANNEL_6, &value);

        if (abs(value - display_value) >= 10)
        {
            display_value = value;

            float voltage = (value / 4095.0f) * 3.3f;

            printf("ADC: %d  Voltage: %.2fV\n", value, voltage);
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void app_main(void)
{
    gpio_config_t io_conf =
    {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask =
            (1ULL << SEG_A) |
            (1ULL << SEG_B) |
            (1ULL << SEG_C) |
            (1ULL << SEG_D) |
            (1ULL << SEG_E) |
            (1ULL << SEG_F) |
            (1ULL << SEG_G) |
            (1ULL << SEG_DP) |
            (1ULL << DIGIT0) |
            (1ULL << DIGIT1) |
            (1ULL << DIGIT2) |
            (1ULL << DIGIT3)
    };

    gpio_config(&io_conf);

    adc_oneshot_unit_init_cfg_t init_config =
    {
        .unit_id = ADC_UNIT_1,
    };

    adc_oneshot_new_unit(&init_config, &adc_handle);

    adc_oneshot_chan_cfg_t config =
    {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12,
    };

    adc_oneshot_config_channel(
        adc_handle,
        ADC_CHANNEL_6,
        &config
    );

    xTaskCreate(
        display_task,
        "display_task",
        2048,
        NULL,
        0,
        NULL
    );

    xTaskCreate(
        adc_task,
        "adc_task",
        2048,
        NULL,
        1,
        NULL
    );
}