//
// Created by Ivan Kishchenko on 15/10/2023.
//


#pragma once

enum UserServiceId {
    Service_User_Gamepad,
    Service_User_DCMotor,
};

enum UserPropId {
    Props_User_Gamepad,
};

enum UserTimerId {
    UserTid_Gamepad = SysTid_TimerMaxId,
};
