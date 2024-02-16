//
// Created by Ivan Kishchenko on 15/10/2023.
//


#pragma once

enum UserServiceId {
    Service_User_Gamepad,
    Service_User_DCMotorLeft,
    Service_User_DCMotorRight,
};

enum UserPropId {
    Props_User_Gamepad,
};

enum UserTimerId {
    UserTid_Gamepad = SysTid_TimerMaxId,
};


enum UserMessageId {
    UserMsgId_DC,
};