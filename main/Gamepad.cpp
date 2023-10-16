//
// Created by Ivan Kishchenko on 15/10/2023.
//

#include "Gamepad.h"
#include "bluetooth/BTHidScanner.h"

void fromJson(cJSON *json, GamepadProperties &props) {
    cJSON *item = json->child;
    while (item) {
        if (!strcmp(item->string, "type") && item->type == cJSON_String) {
            props.type = item->valuestring;
        } else if (!strcmp(item->string, "bda") && item->type == cJSON_String) {
            props.bda = item->valuestring;
        }

        item = item->next;
    }
}

Gamepad::Gamepad(Registry &registry) : TService(registry) {
    registry.getPropsLoader().addReader("gamepad", defaultPropertiesReader<GamepadProperties>);
    registry.getPropsLoader().addConsumer(this);
}

void Gamepad::apply(const GamepadProperties &props) {
    _props = props;
    _timer.fire<UserTid_Gamepad>(5000, true);
    esp_logi(gamepad, "apply permissions");
}

void Gamepad::onEvent(const BTHidConnected &) {
    esp_logi(gamepad, "connected, stop timer");
    _timer.detach();
}

void Gamepad::onEvent(const BTHidInput &msg) {
    auto *gamepad = (HidGamePad *) msg.data;
    esp_logi(gamepad, "input:");
    esp_logi(gamepad, "\tleftAxis: %02d:%02d", gamepad->leftAxisX, gamepad->leftAxisY);
    esp_logi(gamepad, "\tRightAxis: %02d:%02d", gamepad->rightAxisX, gamepad->rightAxisY);
    esp_logi(gamepad, "\tleftAxis: %d: rightAxis: %d", gamepad->keys2.leftAxis, gamepad->keys2.rightAxis);
    esp_logi(gamepad, "\tlb: %d: rb: %d", gamepad->keys1.lb, gamepad->keys1.rb);
    esp_logi(gamepad, "\tlt: %d: rt: %d", gamepad->lt, gamepad->rt);
    if (gamepad->keys1.a) {
        esp_logi(gamepad, "\tbtnA: pushed");
    }
    if (gamepad->keys1.b) {
        esp_logi(gamepad, "\tbtnB: pushed");
    }
    if (gamepad->keys1.x) {
        esp_logi(gamepad, "\tbtnX: pushed");
    }
    if (gamepad->keys1.y) {
        esp_logi(gamepad, "\tbtnY: pushed");
    }
}

void Gamepad::onEvent(const BTHidDisconnected &) {
    esp_logi(gamepad, "disconnected, start timer");
    _timer.fire<UserTid_Gamepad>(5000, true);
}

void Gamepad::onEvent(const TimerEvent<UserTid_Gamepad> &) {
    BTHidConnRequest cmd{
            .transport = ESP_HID_TRANSPORT_BLE,
    };
    strncpy(cmd.bdAddr, _props.bda.c_str(), std::min((size_t) 18, _props.bda.size()));
    getDefaultEventBus().post(cmd);

    esp_logi(gamepad, "on timer");
}
