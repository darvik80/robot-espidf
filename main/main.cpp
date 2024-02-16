#include <core/Core.h>
#include <core/system/storage/NvsStorage.h>
#include <core/system/telemetry/TelemetryService.h>
#include <esp_heap_caps_init.h>

#ifndef CONFIG_IDF_TARGET_LINUX
#include <core/system/wifi/WifiService.h>
#include <core/system/mqtt/MqttService.h>
#include <core/system/console/Console.h>
#include <bluetooth/BTManager.h>
#include <bluetooth/BleDiscovery.h>
#include <bluetooth/BTHidDevice.h>
#include "Gamepad.h"
#include "DCMotor.h"
#endif

class Robot : public Application<Robot>, public TEventSubscriber<Robot, BTHidInput> {
public:
    Robot() = default;

protected:
    void userSetup() override {
        getRegistry().getEventBus().subscribe(shared_from_this());
        getRegistry().create<NvsStorage>();
        getRegistry().create<TelemetryService>();
#ifndef CONFIG_IDF_TARGET_LINUX
        getRegistry().create<WifiService>();
        auto &mqtt = getRegistry().create<MqttService>();
        mqtt.addJsonProcessor<Telemetry>("/telemetry");

        getRegistry().create<UartConsoleService>();
        getRegistry().create<BTManager>();
        getRegistry().create<BleDiscovery>();
        getRegistry().create<BTHidDevice>();

        getRegistry().create<Gamepad>();

        getRegistry().create<DCMotor<Service_User_DCMotorLeft, GPIO_NUM_1, GPIO_NUM_2, GPIO_NUM_3, LEDC_TIMER_0, LEDC_CHANNEL_0>>();
        getRegistry().create<DCMotor<Service_User_DCMotorRight, GPIO_NUM_6, GPIO_NUM_4, GPIO_NUM_5, LEDC_TIMER_1, LEDC_CHANNEL_1>>();
#endif
    }
public:
    void onEvent(const BTHidInput &msg) {
        if (msg.usage == ESP_HID_USAGE_GAMEPAD) {
            auto *gamepad = (HidGamePad *) msg.data;
            DCControl leftControl{.serviceId = Service_User_DCMotorLeft};
            if (gamepad->leftAxisY <= 128) {
                leftControl.direction = DCControl::FORWARD;
            } else {
                leftControl.direction = DCControl::BACKWARD;
            }
            leftControl.speed = std::abs((int16_t)gamepad->leftAxisY-128)*8;
            getRegistry().getEventBus().post(leftControl);

            DCControl rightControl{.serviceId = Service_User_DCMotorRight};
            //esp_logi(app, "left-speed: %d", leftControl.speed);
            if (gamepad->rightAxisY <=128) {
                rightControl.direction = DCControl::FORWARD;
            } else {
                rightControl.direction = DCControl::BACKWARD;
            }
            rightControl.speed = std::abs((int16_t)gamepad->rightAxisY-128)*8;
            getRegistry().getEventBus().post(rightControl);
            //esp_logi(app, "right-speed: %d", rightControl.speed);
        }

    }
};

std::shared_ptr<Robot> app;

extern "C" void app_main() {
    std_error_check(bus_error::ok);
    size_t free = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    size_t total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    esp_logi(app, "heap: %zu/%zu", free, total);

    app = std::make_shared<Robot>();
    app->setup();
}
