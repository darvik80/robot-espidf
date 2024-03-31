//
// Created by Ivan Kishchenko on 25/3/24.
//

#include "DistanceSensor.h"

DistanceSensor::DistanceSensor(Registry &registry, const DistanceOptions &options) : TService(registry),
                                                                                     _options(options) {}

void DistanceSensor::setup() {
    FreeRTOSTask::execute([this]() {
        entry();
    });
}

void DistanceSensor::entry() {
    while (true) {
//        auto distance = _options.sensor->read();
//        _options.callback(distance);
//        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
