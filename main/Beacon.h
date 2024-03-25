//
// Created by Ivan Kishchenko on 05/03/2024.
//

#pragma once


#include "core/Core.h"
#include "UserService.h"
#include "bluetooth/BTConfig.h"
#include "bluetooth/BleDiscovery.h"

#pragma pack(1)

struct BeaconServiceData {
    uint16_t uuid;
    uint16_t major;
    uint16_t minor;
    uint16_t battery;
    uint8_t productModel;
};


struct BeaconManufacturerType {
    uint16_t companyId;
    uint8_t id;
    uint8_t length;
    uint8_t uuid128[ESP_UUID_LEN_128];
    uint16_t major;
    uint16_t minor;
    int8_t rssi1m;
};

#pragma pack()

struct BeaconInfo {
    std::string bda;
    std::string name;
    int rssi;
    int rssi1m;
    double distance;
};

struct LocationTagReport : CMessage<UserMsgId_LocationTagReport, Sys_User> {
    std::string bda;
    std::list<BeaconInfo> beacons;
};

inline void toJson(cJSON *json, const BeaconInfo &msg) {
    cJSON_AddStringToObject(json, "bda", msg.bda.c_str());
    if (!msg.name.empty()) {
        cJSON_AddStringToObject(json, "bda", msg.name.c_str());
    }
    cJSON_AddNumberToObject(json, "rssi", msg.rssi);
    cJSON_AddNumberToObject(json, "rssi1m", msg.rssi1m);
}

inline void toJson(cJSON *json, const LocationTagReport &msg) {
    cJSON_AddStringToObject(json, "bda", msg.bda.c_str());
    cJSON *beacons = cJSON_AddArrayToObject(json, "beacons");
    for (const auto &beacon : msg.beacons) {
        cJSON *beaconJson = cJSON_CreateObject();
        toJson(beaconJson, beacon);
        cJSON_AddItemToArray(beacons, beaconJson);
    }
}

class Beacon
        : public TService<Beacon, Service_User_Beacon, Sys_User>,
          public TMessageSubscriber<Beacon, BleScanResult, BleScanDone, TimerEvent<UserTid_Beacon>> {
    std::unordered_map<std::string, BeaconInfo> _beacons;
    static const uint8_t S_BEACON_UUID[ESP_UUID_LEN_128];

    FreeRTOSTimer _timer;
public:
    explicit Beacon(Registry &registry);

    [[nodiscard]] std::string_view getServiceName() const override {
        return "beacon";
    }

    void handle(const BleScanResult &msg);

    void handle(const BleScanDone &msg);
    void handle(const TimerEvent<UserTid_Beacon> &msg);

    static const BleDiscoveryFilter getFilter();
};
