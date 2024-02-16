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

#include "core/Task.h"

class Robot : public Application<Robot>
#ifndef CONFIG_IDF_TARGET_LINUX
        , public TMessageSubscriber<Robot, BTHidInput>
#endif
{
public:
    Robot() = default;

protected:
    void userSetup() override {
#ifndef CONFIG_IDF_TARGET_LINUX
        getRegistry().getEventBus().subscribe(shared_from_this());
#endif
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

        getRegistry().create<DCMotor<Service_User_DCMotorLeft>>(DCMotorOptions {
                .en = GPIO_NUM_1,
                .in1 = GPIO_NUM_2,
                .in2 = GPIO_NUM_3,
                .timer = LEDC_TIMER_0,
                .ch = LEDC_CHANNEL_0
        });
        getRegistry().create<DCMotor<Service_User_DCMotorRight>>(DCMotorOptions{
                .en = GPIO_NUM_6,
                .in1 = GPIO_NUM_5,
                .in2 = GPIO_NUM_4,
                .timer = LEDC_TIMER_1,
                .ch = LEDC_CHANNEL_1
        });
#else
        //getRegistry().create<HidGamepad>();
#endif
    }

public:
#ifndef CONFIG_IDF_TARGET_LINUX

    void handle(const BTHidInput &msg) {
        if (msg.usage == ESP_HID_USAGE_GAMEPAD) {
            auto *gamepad = (HidGamePad *) msg.data;
            DCControl<Service_User_DCMotorLeft> leftControl{
                    .direction = gamepad->leftAxisY <= 128 ? FORWARD : BACKWARD,
                    .speed = (uint16_t) (std::abs((int16_t) gamepad->leftAxisY - 128) * 8),
            };
            getRegistry().getEventBus().post(leftControl);

            DCControl<Service_User_DCMotorRight> rightControl{
                    .direction = gamepad->rightAxisY <= 128 ? FORWARD : BACKWARD,
                    .speed = (uint16_t) (std::abs((int16_t) gamepad->rightAxisY - 128) * 8),
            };
            getRegistry().getEventBus().post(rightControl);
        }
    }

#endif
};

std::shared_ptr<Robot> app;

extern "C" void app_main() {

    size_t free = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    size_t total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    esp_logi(app, "heap: %zu/%zu", free, total);

    app = std::make_shared<Robot>();
    app->setup();
}
