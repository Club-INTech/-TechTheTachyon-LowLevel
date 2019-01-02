//
// Created by trotfunky on 26/11/18.
//

#ifndef LL_MCSREBORN_H
#define LL_MCSREBORN_H

#include "Utils/Singleton.hpp"
#include "ControlSettings.h"
#include "RobotStatus.h"
#include "Motor.h"
#include "pid.hpp"
#include "SelfContainedPID.hpp"

// TODO : Tout docu

class MCS : public Singleton<MCS>
{

private:

    RobotStatus robotStatus;
    ControlSettings controlSettings;

    Motor leftMotor;
    Motor rightMotor;

    SelfContainedPID leftSpeedPID;
    SelfContainedPID rightSpeedPID;
    SelfContainedPID translationPID;
    SelfContainedPID rotationPID;

    uint32_t lastPositionUpdateTime;
    int16_t targetX;
    int16_t targetY;
    int16_t targetDistance;
    float targetAngle;

    float targetLeftSpeed;
    float targetRightSpeed;


public:
    MCS();

    void manageStop();
    void updatePosition(int32_t leftTicks, int32_t rightTicks);
    void control();
    void stop();

    void translate(int16_t);
    void rotate(float);
    void gotoPoint(int16_t,int16_t,bool);

    void toggleControl();
    void toggleTranslation();
    void toggleRotation();

    int16_t getX();
    int16_t getY();
    float getAngle();

    void setX(int16_t);
    void setY(int16_t);
    void setAngle(float);
};

#endif //LL_MCSREBORN_H
