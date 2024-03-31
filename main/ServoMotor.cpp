//
// Created by Ivan Kishchenko on 25/3/24.
//

#include "ServoMotor.h"
#include "driver/mcpwm_prelude.h"

// Please consult the datasheet of your servo before changing the following parameters
#define SERVO_MIN_PULSEWIDTH_US 500  // Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH_US 2500  // Maximum pulse width in microsecond
#define SERVO_MIN_DEGREE        (-90)   // Minimum angle
#define SERVO_MAX_DEGREE        90    // Maximum angle

#define SERVO_PULSE_GPIO             0        // GPIO connects to the PWM signal line
#define SERVO_TIMEBASE_RESOLUTION_HZ 1000000  // 1MHz, 1us per tick
#define SERVO_TIMEBASE_PERIOD        20000    // 20000 ticks, 20ms

static inline uint32_t example_angle_to_compare(int angle) {
    return (angle - SERVO_MIN_DEGREE) * (SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) /
           (SERVO_MAX_DEGREE - SERVO_MIN_DEGREE) + SERVO_MIN_PULSEWIDTH_US;
}

ServoMotor::ServoMotor(Registry &registry, const ServoMotorOptions &options)
        : TService(registry), _options(options), _bus([this](const ServoControl &msg) {
                                                          handle(msg);
                                                          vTaskDelay(pdMS_TO_TICKS(500));
                                                      }, {.name = "servo-bus"}
) {}

void ServoMotor::setup() {
    core_logi(servo, "Create timer and operator");
    mcpwm_timer_config_t timer_config = {
            .group_id = 0,
            .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
            .resolution_hz = SERVO_TIMEBASE_RESOLUTION_HZ,
            .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
            .period_ticks = SERVO_TIMEBASE_PERIOD,
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &_timer));

    mcpwm_operator_config_t operator_config = {
            .group_id = 0, // operator must be in the same group to the timer
    };
    ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &_operator));

    core_logi(servo, "Connect timer and operator");
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(_operator, _timer));

    core_logi(servo, "Create comparator and generator from the operator");
    mcpwm_comparator_config_t comparator_config = {
            .flags {
                    .update_cmp_on_tez = true,
            },
    };
    ESP_ERROR_CHECK(mcpwm_new_comparator(_operator, &comparator_config, &_comparator));

    mcpwm_generator_config_t generator_config = {
            .gen_gpio_num = SERVO_PULSE_GPIO,
    };
    ESP_ERROR_CHECK(mcpwm_new_generator(_operator, &generator_config, &_generator));

    // set the initial compare value, so that the servo will spin to the center position
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(_comparator, example_angle_to_compare(0)));

    core_logi(servo, "Set generator action on timer and compare event");
    // go high on counter empty
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(_generator,
                                                              MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP,
                                                                                           MCPWM_TIMER_EVENT_EMPTY,
                                                                                           MCPWM_GEN_ACTION_HIGH)));
    // go low on compare threshold
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(_generator,
                                                                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP,
                                                                                               _comparator,
                                                                                               MCPWM_GEN_ACTION_LOW)));

    core_logi(servo, "Enable and start timer");
    ESP_ERROR_CHECK(mcpwm_timer_enable(_timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(_timer, MCPWM_TIMER_START_NO_STOP));
}

void ServoMotor::handle(const ServoControl &msg) {
    core_logi(servo, "Angle of rotation: %d", msg.angle);
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(_comparator, example_angle_to_compare(msg.angle)));
    //Add delay, since it takes time for servo to rotate, usually 200ms/60degree rotation under 5V power supply
    vTaskDelay(pdMS_TO_TICKS(500));
}
