//
// Created by Ivan Kishchenko on 15/10/2023.
//

#pragma once

#include "core/Core.h"
#include "UserService.h"
#include "bluetooth/BTConfig.h"

struct GamepadProperties : TProperties<Props_User_Gamepad, System::Sys_User> {
    std::string type;
    std::string bda;
};

[[maybe_unused]] void fromJson(cJSON *json, GamepadProperties &props);

class Gamepad : public TService<Gamepad, Service_User_Gamepad, Sys_User>,
    public TPropertiesConsumer<Gamepad, GamepadProperties>,
    public TEventSubscriber<Gamepad, BTHidConnected, BTHidDisconnected, BTHidInput, TimerEvent<UserTid_Gamepad>> {
    EspTimer _timer{"gamepad-timer"};

    GamepadProperties _props;
public:
    explicit Gamepad(Registry &registry);

    void apply(const GamepadProperties &props);

    void onEvent(const BTHidConnected& msg);

    void onEvent(const BTHidInput& msg);

    void onEvent(const BTHidDisconnected& msg);

    void onEvent(const TimerEvent<UserTid_Gamepad>& msg);
};
