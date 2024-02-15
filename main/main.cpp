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

enum UserMessageId {
    UM_MsgId_Test
};

struct TestEvent : TEvent<UM_MsgId_Test, Sys_User> {
    char message[32] = "hello";
};

class Robot : public Application<Robot> {
public:
    Robot() = default;

protected:
    void userSetup() override {
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

        //getRegistry().create<Gamepad>();

        getRegistry().create<DCMotor<GPIO_NUM_1, GPIO_NUM_2, GPIO_NUM_3, LEDC_TIMER_0, LEDC_CHANNEL_0>>();
        getRegistry().create<DCMotor<GPIO_NUM_6, GPIO_NUM_5, GPIO_NUM_4, LEDC_TIMER_1, LEDC_CHANNEL_1>>();
#endif
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
