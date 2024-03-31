//
// Created by Ivan Kishchenko on 15/10/2023.
//


#pragma once

#include "core/system/SystemEvent.h"

enum UserServiceId {
    Service_User_Gamepad,
    Service_User_DCMotorLeft,
    Service_User_DCMotorRight,
    Service_User_ServoMotor,
    Service_User_Distance,
    Service_User_Beacon,
    Service_User_LinuxHidGamepad,
};

enum UserPropId {
    Props_User_Gamepad,
};

enum UserTimerId {
    UserTid_Gamepad = SysTid_TimerMaxId,
    UserTid_Beacon,
};


enum UserMessageId {
    UserMsgId_DC,
    UserMsgId_Gamepad,
    UserMsgId_LocationTagReport,
};