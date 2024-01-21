#include <core/Core.h>
#include <core/system/storage/NvsStorage.h>
#include <core/system/telemetry/TelemetryService.h>
#ifndef CONFIG_IDF_TARGET_LINUX
#include <core/system/wifi/WifiService.h>
#include <core/system/mqtt/MqttService.h>
#include <core/system/console/Console.h>
#include <bluetooth/BTManager.h>
#include <bluetooth/BleDiscovery.h>
#include <bluetooth/BTHidDevice.h>
#include "Gamepad.h"
#endif

enum UserMessageId {
    UM_MsgId_Test
};

struct TestEvent : TEvent<UM_MsgId_Test, Sys_User> {
    char message[32] = "hello";
};

class Robot : public Application<Robot>, public TEventSubscriber<Robot, TestEvent> {
    FreeRTOSEventBus<4> _bus;

    EspEventBus _espBus;

public:
    Robot()
        : _bus{withName("bus"), withQueueSize(4), withStackSize(3096)}
          , _espBus{withName("esp-bus"), withQueueSize(4), withStackSize(3096), withSystemQueue(true)} {
    }

protected:
    void userSetup() override {
        getDefaultEventBus().subscribe(shared_from_this());
        _bus.subscribe(shared_from_this());
        _espBus.subscribe(shared_from_this());
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
#endif


        getDefaultEventBus().post(TestEvent{});
        getDefaultEventBus().post(TestEvent{.message = "non-copiable"});


        _bus.post(TestEvent{.message = "custom bus"});
        _espBus.post(TestEvent{.message = "esp event bus"});
    }

public:
    void onEvent(const TestEvent&event) {
        esp_logi(app, "handle bus: %s, msg: %s", pcTaskGetName(nullptr), event.message);
    }
};

std::shared_ptr<Robot> app;

extern "C" void app_main() {
#ifndef CONFIG_IDF_TARGET_LINUX
    size_t free = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    size_t total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    esp_logi(app, "heap: %d/%d", free, total);
#endif
    app = std::make_shared<Robot>();
    app->setup();
}
