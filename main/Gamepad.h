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

struct GamepadInput :TMessage<UserMsgId_Gamepad, Sys_User> {
    struct Axis {
        int16_t x;
        int16_t y;
        bool btn;
    };
    Axis leftAxis;
    Axis rightAxis;

    struct {
        bool a: 1;
        bool b: 1;
        bool x: 1;
        bool y: 1;
        bool select: 1;
        bool start: 1;
        bool lb: 1;
        bool rb: 1;
    } keys;
    int lt;
    int rt;
};

class Gamepad : public TService<Gamepad, Service_User_Gamepad, Sys_User>,
    public TPropertiesConsumer<Gamepad, GamepadProperties>,
    public TMessageSubscriber<Gamepad, BTHidConnected, BTHidDisconnected, BTHidInput, TimerEvent<UserTid_Gamepad>> {
    EspTimer _timer{"gamepad-timer"};

    GamepadProperties _props;
public:
    explicit Gamepad(Registry &registry);

    [[nodiscard]] std::string_view getServiceName() const override {
        return "gamepad";
    }

    void apply(const GamepadProperties &props);

    void handle(const BTHidConnected& msg);

    void handle(const BTHidInput& msg);

    void handle(const BTHidDisconnected& msg);

    void handle(const TimerEvent<UserTid_Gamepad>& msg);
};
