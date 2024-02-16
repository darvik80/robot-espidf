//
// Created by Ivan Kishchenko on 14/02/2024.
//

#pragma once

#include "core/Core.h"
#include "UserService.h"

#include <driver/ledc.h>
#include <rom/gpio.h>

struct DCControl : TEvent<UserMsgId_DC, Sys_User> {
    enum Direction {
        FORWARD,
        BACKWARD,
    };
    ServiceId serviceId{};
    Direction direction{};
    uint16_t speed{};
};

template<SystemId sysId, gpio_num_t en, gpio_num_t in1, gpio_num_t in2, ledc_timer_t timer, ledc_channel_t ch>
class DCMotor : public TService<DCMotor<sysId, en, in1, in2, timer, ch>, sysId, Sys_User>,
                public TEventSubscriber<DCMotor<sysId, en, in1, in2, timer, ch>, DCControl> {
    DCControl _lastMsg;
public:
    explicit DCMotor(Registry &registry) : TService<DCMotor<sysId, en, in1, in2, timer, ch>, sysId, Sys_User>(
            registry) {}

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
                .duty_resolution  = LEDC_TIMER_10_BIT,
                .timer_num        = timer,
                .freq_hz          = 2000,
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
                .duty           = 0, // Set duty to 0%
                .hpoint         = 0,
                .flags {
                        .output_invert = 0,
                }

        };
        ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

//        // back
//        gpio_set_level(in1, 0);
//        gpio_set_level(in2, 1);
//        ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, ch, 512));
//        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, ch));
//        vTaskDelay(pdMS_TO_TICKS(5000));
//
//        // forward
//        gpio_set_level(in1, 1);
//        gpio_set_level(in2, 0);
//        ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, ch, 1023));
//        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, ch));
//        vTaskDelay(pdMS_TO_TICKS(5000));

//        // stop
//        gpio_set_level(in1, 0);
//        gpio_set_level(in2, 0);
    }

public:
    void onEvent(const DCControl &msg) {
        //esp_logi(dc, "handle event: %d:%d", msg.serviceId, (uint8_t)this->getServiceId());
        if (msg.serviceId == this->getServiceId()) {
            if (_lastMsg.direction != msg.direction || _lastMsg.speed != msg.speed) {
                if (msg.direction == DCControl::FORWARD) {
                    gpio_set_level(in1, 1);
                    gpio_set_level(in2, 0);
                } else if (msg.direction == DCControl::BACKWARD) {
                    gpio_set_level(in1, 0);
                    gpio_set_level(in2, 1);
                }

                ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, ch, msg.speed));
                ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, ch));

                _lastMsg = msg;
            }
        }
    }
};
