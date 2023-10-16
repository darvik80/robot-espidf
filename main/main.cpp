#include <core/Core.h>
#include <core/system/storage/NvsStorage.h>
#include <core/system/telemetry/TelemetryService.h>
#include <core/system/wifi/WifiService.h>
#include <core/system/mqtt/MqttService.h>
#include <core/system/console/Console.h>
#include <bluetooth/BTManager.h>
#include <bluetooth/BleDiscovery.h>
#include <bluetooth/BTHidScanner.h>
#include "Gamepad.h"

class Robot : public Application<Robot> {
public:
protected:
    void userSetup() override {
        getRegistry().create<NvsStorage>();
        getRegistry().create<TelemetryService>();
        getRegistry().create<WifiService>();
        auto &mqtt = getRegistry().create<MqttService>();
        mqtt.addJsonProcessor<Telemetry>("/telemetry");

        getRegistry().create<UartConsoleService>();
        getRegistry().create<BTManager>();
        getRegistry().create<BleDiscovery>();
        getRegistry().create<BTHidScanner>();

        getRegistry().create<Gamepad>();
    }
};

std::shared_ptr<Robot> app;

extern "C" void app_main() {
    app = std::make_shared<Robot>();
    app->setup();
}
