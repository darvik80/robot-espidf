//
// Created by Ivan Kishchenko on 14/02/2024.
//

#pragma once

#include "core/Core.h"
#include "UserService.h"

#include <driver/ledc.h>
#include <rom/gpio.h>

enum DCDirection {
    FORWARD,
    BACKWARD,
};

struct DCControl {
    DCDirection direction{};
    int speed{};
};

struct DCMotorOptions {
    gpio_num_t en;
    gpio_num_t in1;
    gpio_num_t in2;
    ledc_timer_t timer;
    ledc_channel_t ch;
};

template<SystemId sysId>
class DCMotor : public TService<DCMotor<sysId>, sysId, Sys_User> {
    DCControl _lastMsg;
    DCMotorOptions _options;

    FreeRTOSMessageBus<DCControl, 1> _bus;
public:
    explicit DCMotor(Registry &registry, const DCMotorOptions &options)
            : TService<DCMotor<sysId>, sysId, Sys_User>(registry), _options(options),
              _bus([this](const DCControl &msg) {
                       handle(msg);
                       vTaskDelay(pdMS_TO_TICKS(500));
                   },
                   {
                           .name = "dc-bus"
                   }
              ) {}

    [[nodiscard]] std::string_view getServiceName() const override {
        return "dc-motor";
    }

    void setup() {
        gpio_reset_pin(_options.en);
        gpio_set_direction(_options.en, GPIO_MODE_OUTPUT);
        gpio_reset_pin(_options.in1);
        gpio_set_direction(_options.in1, GPIO_MODE_OUTPUT);
        gpio_set_pull_mode(_options.in1, GPIO_FLOATING);
        gpio_reset_pin(_options.in2);
        gpio_set_direction(_options.in2, GPIO_MODE_OUTPUT);
        gpio_set_pull_mode(_options.in2, GPIO_FLOATING);

        // Prepare and then apply the LEDC PWM timer configuration
        ledc_timer_config_t ledc_timer = {
                .speed_mode       = LEDC_LOW_SPEED_MODE,
                .duty_resolution  = LEDC_TIMER_10_BIT,
                .timer_num        = _options.timer,
                .freq_hz          = 2000,
                .clk_cfg          = LEDC_AUTO_CLK
        };
        ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

        // Prepare and then apply the LEDC PWM channel configuration
        ledc_channel_config_t ledc_channel = {
                .gpio_num       = _options.en,
                .speed_mode     = LEDC_LOW_SPEED_MODE,
                .channel        = _options.ch,
                .intr_type      = LEDC_INTR_DISABLE,
                .timer_sel      = _options.timer,
                .duty           = 0, // Set duty to 0%
                .hpoint         = 0,
                .flags {
                        .output_invert = 0,
                }

        };
        ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    }

private:
    void handle(const DCControl &msg) {
        if (_lastMsg.direction != msg.direction || _lastMsg.speed != msg.speed) {
            if (msg.direction == FORWARD) {
                gpio_set_level(_options.in1, 1);
                gpio_set_level(_options.in2, 0);
            } else if (msg.direction == BACKWARD) {
                gpio_set_level(_options.in1, 0);
                gpio_set_level(_options.in2, 1);
            }

            ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, _options.ch, msg.speed));
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, _options.ch));

            _lastMsg = msg;
        }
    }

public:
    void move(DCDirection direction, int speed) {
        _bus.overwrite(DCControl{.direction = direction, .speed = speed});
    }
};

