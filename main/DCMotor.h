//
// Created by Ivan Kishchenko on 14/02/2024.
//

#pragma once

#include "core/Core.h"
#include "UserService.h"

#include <driver/ledc.h>
#include <rom/gpio.h>

template <gpio_num_t en, gpio_num_t in1, gpio_num_t in2, ledc_timer_t timer, ledc_channel_t ch>
class DCMotor : public TService<DCMotor<en, in1, in2, timer, ch>, Service_User_DCMotor, Sys_User> {
public:
    explicit DCMotor(Registry &registry) : TService<DCMotor<en, in1, in2, timer, ch>, Service_User_DCMotor, Sys_User>(registry) {}

    void setup() {
        gpio_reset_pin(en);
        gpio_set_direction(en, GPIO_MODE_OUTPUT);
        gpio_reset_pin(in1);
        gpio_set_direction(in1, GPIO_MODE_OUTPUT);
        gpio_set_pull_mode(in1, GPIO_FLOATING);
        gpio_reset_pin(in2);
        gpio_set_direction(in2, GPIO_MODE_OUTPUT);
        gpio_set_pull_mode(in2, GPIO_FLOATING);

        // Prepare and then apply the LEDC PWM timer configuration
        ledc_timer_config_t ledc_timer = {
                .speed_mode       = LEDC_LOW_SPEED_MODE,
                .duty_resolution  = LEDC_TIMER_8_BIT,
                .timer_num        = timer,
                .freq_hz          = 5000,
                .clk_cfg          = LEDC_AUTO_CLK
        };
        ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

        // Prepare and then apply the LEDC PWM channel configuration
        ledc_channel_config_t ledc_channel = {
                .gpio_num       = en,
                .speed_mode     = LEDC_LOW_SPEED_MODE,
                .channel        = ch,
                .intr_type      = LEDC_INTR_DISABLE,
                .timer_sel      = timer,
                .duty           = 255, // Set duty to 0%
                .hpoint         = 0,
                .flags {
                    .output_invert = 0,
                }

        };
        ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

        gpio_set_level(in1, 0);
        gpio_set_level(in2, 1);
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, ch, 255));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, ch));
        vTaskDelay(pdMS_TO_TICKS(10000));

        gpio_set_level(in1, 1);
        gpio_set_level(in2, 0);
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, ch, 255));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, ch));
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
};
