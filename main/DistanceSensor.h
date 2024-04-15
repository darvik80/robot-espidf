//
// Created by Ivan Kishchenko on 25/3/24.
//

#pragma once

#include <soc/gpio_num.h>
#include "core/Core.h"
#include "UserService.h"

struct DistanceOptions {
    gpio_num_t pinOut;
    gpio_num_t pinIn;
};

class DistanceSensor : public TService<DistanceSensor, Service_User_Distance, Sys_User> {
    DistanceOptions _options;

private:
    void entry();
public:
    DistanceSensor() = delete;

    DistanceSensor(const DistanceSensor &) = delete;

    DistanceSensor &operator=(const DistanceSensor &) = delete;

    explicit DistanceSensor(Registry &registry, const DistanceOptions& options);

    void setup() override;
};
