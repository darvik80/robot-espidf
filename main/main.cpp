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
#else

#include "HidGamepad.h"

#endif

#include "core/Task.h"

enum UserMessageId {
    UM_MsgId_Test,
    UM_MsgId_Message,
    UM_MsgId_Trivial,
    UM_MsgId_NonTrivial,
};

struct TestEvent : TMessage<UM_MsgId_Test, Sys_User> {
    char message[32] = "hello";
};

struct TestCEvent : CMessage<UM_MsgId_Message, Sys_User> {
    explicit TestCEvent(std::string_view message) : message(message) {}

    std::string message = "complex";

    ~TestCEvent() override {
        esp_logi(test, "~TestCEvent");
    }
};

struct Trivial : TMessage<UM_MsgId_Trivial, Sys_User> {
    Trivial() = default;
    explicit Trivial(std::string_view message) : message(message) {}

    std::string message = "trivial";

    ~Trivial() {
        esp_logi(test, "~Trivial");
    }
};

struct NonTrivial : CMessage<UM_MsgId_NonTrivial, Sys_User> {
    NonTrivial() = default;
    explicit NonTrivial(std::string_view message) : message(message) {}

    std::string message = "not trivial";

    ~NonTrivial() override {
        esp_logi(test, "~NonTrivial");
    }
};

class Robot
        : public Application<Robot>,
          public TMessageSubscriber<Robot, TestEvent, TestCEvent, Trivial, NonTrivial, TimerEvent<SysTid_User>> {
    FreeRTOSEventBus<4> _bus;

    EspEventBus _espBus;

    FreeRTOSTimer _timer;
    FreeRTOSTimer _fire;

    FreeRTOSTask _task;
public:
    Robot()
            : _bus{{.queueSize = 4, .stackSize = 3096, .name = "freertos-bus"}},
              _espBus{{.queueSize = 4, .stackSize = 3096, .name = "esp-bus"}} {
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
#else
        //getRegistry().create<HidGamepad>();
#endif
        //getRegistry().getMessageBus().subscribe(shared_from_this());
        TestEvent event;
        strncpy(event.message, "hello", sizeof(event.message));
        getRegistry().getEventBus().post(event);

        getRegistry().getEventBus().post(TestCEvent{"hello complex"});

        _timer.attach(20000, true, []() {
            esp_logi(timer, "timer 20000");
        });

        _fire.fire<SysTid_User>(30000, true);

        _task = FreeRTOSTask::submit([]() {
            esp_logi(task, "task1");
        });
        FreeRTOSTask::execute([]() {
            esp_logi(task, "task2");
        });
    }

public:
    void handle(const TestEvent &event) {
        esp_logi(app, "handle bus: %s, msg: %s", pcTaskGetName(nullptr), event.message);
        getRegistry().getEventBus().send(NonTrivial{});
    }

    void handle(const TestCEvent &event) {
        esp_logi(app, "handle bus: %s, msg: %s", pcTaskGetName(nullptr), event.message.c_str());
        getRegistry().getEventBus().send(Trivial{});
    }
    void handle(const Trivial &event) {
        esp_logi(app, "handle bus: %s, msg: %s", pcTaskGetName(nullptr), event.message.c_str());
    }
    void handle(const NonTrivial &event) {
        esp_logi(app, "handle bus: %s, msg: %s", pcTaskGetName(nullptr), event.message.c_str());
    }

    void handle(const TimerEvent<SysTid_User> &event) {
        esp_logi(app, "handle timer: %s, msg: %d", pcTaskGetName(nullptr), event.TimerId);
    }
};

std::shared_ptr<Robot> app;

extern "C" void app_main() {

    size_t free = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    size_t total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    esp_logi(app, "heap: %zu/%zu", free, total);

    app = std::make_shared<Robot>();
    app->setup();
}
