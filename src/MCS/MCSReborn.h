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
#include "PointToPointTrajectory.h"

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
    int32_t leftTicks;
    int32_t rightTicks;
    int32_t previousLeftTicks;
    int32_t previousRightTicks;
    int16_t targetDistance;
    float targetAngle;
    float angleOffset;

    float targetLeftSpeed;
    float targetRightSpeed;
    bool sequentialMovement;
    PointToPointTrajectory trajectory;


public:
    MCS();

    void manageStop();
    void updatePosition(int32_t leftTicks, int32_t rightTicks);
    void control();
    void stop();

    void translate(int16_t);
    void rotate(float);
    void gotoPoint(int16_t,int16_t,bool);
    void followTrajectory(const double* xTable, const double* yTable, int count);

    void toggleControl();
    void toggleTranslation();
    void toggleRotation();

    int16_t getX();
    int16_t getY();
    float getAngle();
    int32_t getLeftTicks();
    int32_t getRightTicks();

    /**
     * Permet de définir une rotation à la fin d'un mouvement (au lieu de devoir attendre la fin du mouvement et de donner un ordre de rotation)
     * /!\\ Cette valeur est réinitialisée dès la fin du mouvement!!! (Histoire de pas se décaler avec les mouvements suivants)
     * @param offset l'angle, en radians, duquel le robot doit tourner à la fin du mouvement
     */
    void setAngleOffset(float offset);

    /**
     * Annule le suivi de trajectoire courant
     */
    void disableP2P();

    void setX(int16_t);
    void setY(int16_t);
    void setAngle(float);
};

#endif //LL_MCSREBORN_H
