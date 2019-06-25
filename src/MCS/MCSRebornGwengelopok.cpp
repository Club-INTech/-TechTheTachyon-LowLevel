//
// Created by jglrxavpok aka Coin-Coin Ier <3 (27/02) on 20/12/18.
//

#include "MCSReborn.h"


MCS::MCS(): leftMotor(Side::LEFT), rightMotor(Side::RIGHT)  {

    encoderLeft = new Encoder(ENCODER_LEFT_B,ENCODER_LEFT_A);
    encoderRight = new Encoder(ENCODER_RIGHT_B,ENCODER_RIGHT_A);

    initSettings();
    initStatus();
    robotStatus.controlled = true;
    robotStatus.controlledRotation = true;
    robotStatus.controlledTranslation = true;
    robotStatus.controlledP2P = false;
    robotStatus.movement = MOVEMENT::NONE;


    /*leftSpeedPID.setTunings(1.65, 0.005, 40, 0);
    leftSpeedPID.enableAWU(false);
    rightSpeedPID.setTunings(1.35, 0.005, 40, 0);
    rightSpeedPID.enableAWU(false);*/

    leftSpeedPID.setTunings(1.65, 0.005, 50, 0);
    leftSpeedPID.enableAWU(false);
    rightSpeedPID.setTunings(1.35, 0.005, 50, 0);
    rightSpeedPID.enableAWU(false);

    translationPID.setTunings(4.35,0.000001,0,0);
    translationPID.enableAWU(false);
//    rotationPID180.setTunings(6.5,0.0001,0,0);
//    rotationPID.setTunings(8.75,0.000001,0,0);
  //  rotationPID.setTunings(18,0.000001,0,0);
    rotationPID.setTunings(3.5,0.000001,0,0);
//    rotationPID90.setTunings(10.3,0.0001,12,0);
//    rotationPID180.enableAWU(false);
//    rotationPID90.enableAWU(false);
    leftMotor.init();
    rightMotor.init();

    expectedWallImpact = false;
}

void MCS::initSettings() {
    robotStatus.controlledP2P = false;
    robotStatus.movement = MOVEMENT::NONE;


    /* mm/s/MCS_PERIOD */
    controlSettings.maxAcceleration = 1;//2;
    controlSettings.maxDeceleration = 1;//2;

    /* rad/s */
    controlSettings.maxRotationSpeed = 2*PI;


    /* mm/s */
    controlSettings.maxTranslationSpeed = 500;
    controlSettings.tolerancySpeed = 100;

    /* rad */
    controlSettings.tolerancyAngle = 0.0005;
    //controlSettings.tolerancyAngle = 0.0005;

    /* mm */
    controlSettings.tolerancyTranslation = 1;


    /* ms */
    controlSettings.stopDelay = 25;

    /* mm/s */
    //controlSettings.tolerancyDerivative = 0;
    controlSettings.tolerancyDerivative = 7;

    /* patate */
    controlSettings.tolerancyDifferenceSpeed = 500*2;
}

void MCS::initStatus() {
    robotStatus.movement = MOVEMENT::NONE;
    robotStatus.moving = false;
    robotStatus.controlledP2P = false;
    robotStatus.controlled = true;
    robotStatus.controlledRotation = true;
    robotStatus.controlledTranslation = true;
    previousLeftSpeedGoal = 0;
    previousRightSpeedGoal = 0;
    previousLeftTicks = 0;
    previousRightTicks = 0;
}

void MCS::updatePositionOrientation() {

    int32_t leftDistance = leftTicks * TICK_TO_MM;
    int32_t rightDistance = rightTicks * TICK_TO_MM;

    robotStatus.orientation = (rightTicks - leftTicks) / 2.0 * TICK_TO_RADIAN + angleOffset;

    float cos = cosf(getAngle());
    float sin = sinf(getAngle());

    // somme des résultantes
    int32_t distance = (leftDistance+rightDistance)/2;

    float distanceTravelled = ((rightTicks-previousRightTicks) + (leftTicks-previousLeftTicks))*TICK_TO_MM/2.0f;
    robotStatus.x += distanceTravelled * cos;
    robotStatus.y += distanceTravelled * sin;

    currentDistance = distance;

 //   Serial.printf("%f \n", currentRotation);
}

void MCS::updateSpeed()
{
    averageLeftSpeed.add((leftTicks - previousLeftTicks) * TICK_TO_MM * MCS_FREQ);
    averageRightSpeed.add((rightTicks - previousRightTicks) * TICK_TO_MM  * MCS_FREQ);
    robotStatus.speedLeftWheel = averageLeftSpeed.value();
    robotStatus.speedRightWheel = averageRightSpeed.value();

    bool controlled = ! robotStatus.forcedMovement;
    if(robotStatus.controlledTranslation)
    {
        robotStatus.speedTranslation = translationPID.compute(currentDistance);
    }
    else if(controlled)
    {
        robotStatus.speedTranslation = 0.0f;
    }

    if(robotStatus.controlledRotation && !expectedWallImpact)
    {
        robotStatus.speedRotation = rotationPID.compute(robotStatus.orientation);
    }
    else if(controlled)
    {
        robotStatus.speedRotation = 0.0f;
    }


    robotStatus.speedTranslation = MAX(-controlSettings.maxTranslationSpeed, MIN(controlSettings.maxTranslationSpeed, robotStatus.speedTranslation));
    robotStatus.speedRotation = MAX(-controlSettings.maxRotationSpeed, MIN(controlSettings.maxRotationSpeed, robotStatus.speedRotation)) * DISTANCE_COD_GAUCHE_CENTRE;

    leftSpeedPID.setGoal(robotStatus.speedTranslation-robotStatus.speedRotation);
    rightSpeedPID.setGoal(robotStatus.speedTranslation+robotStatus.speedRotation);

    if( leftSpeedPID.getCurrentGoal() - previousLeftSpeedGoal > controlSettings.maxAcceleration ) {
        leftSpeedPID.setGoal( previousLeftSpeedGoal + controlSettings.maxAcceleration );
    }
    if( previousLeftSpeedGoal - leftSpeedPID.getCurrentGoal() > controlSettings.maxDeceleration && !robotStatus.stuck) {
        leftSpeedPID.setGoal( previousLeftSpeedGoal - controlSettings.maxDeceleration );
    }

    if( rightSpeedPID.getCurrentGoal() - previousRightSpeedGoal > controlSettings.maxAcceleration ) {
        rightSpeedPID.setGoal( previousRightSpeedGoal + controlSettings.maxAcceleration );
    }
    if( previousRightSpeedGoal - rightSpeedPID.getCurrentGoal() > controlSettings.maxDeceleration && !robotStatus.stuck) {
        rightSpeedPID.setGoal( previousRightSpeedGoal - controlSettings.maxDeceleration );
    }

    previousLeftSpeedGoal = leftSpeedPID.getCurrentGoal();
    previousRightSpeedGoal = rightSpeedPID.getCurrentGoal();
}
void MCS::control()
{
    if(!robotStatus.controlled)
        return;

    leftTicks = encoderLeft->read();
    rightTicks = encoderRight->read();

    updatePositionOrientation();

    updateSpeed();

    int32_t leftPWM = leftSpeedPID.compute(robotStatus.speedLeftWheel);
    int32_t rightPWM = rightSpeedPID.compute(robotStatus.speedRightWheel);
    leftMotor.run(leftPWM);
    rightMotor.run(rightPWM);

    previousLeftTicks = leftTicks;
    previousRightTicks = rightTicks;
    /*digitalWrite(LED1,robotStatus.controlledP2P);
    digitalWrite(LED4,rotationPID.getDerivativeError()==0);
    digitalWrite(LED3,robotStatus.Lbooly);*/
    digitalWrite(LED3, robotStatus.moving);
    //averageRotationDerivativeError.add(rotationPID.getDerivativeError());
    if(gotoTimer > 0)
        gotoTimer--;
    if(robotStatus.controlledP2P && !robotStatus.moving && gotoTimer == 0) {//ABS(averageRotationDerivativeError.value()) <= controlSettings.tolerancyDerivative && ABS(rotationPID.getError())<=controlSettings.tolerancyAngle){
        float dx = (targetX-robotStatus.x);
        float dy = (targetY-robotStatus.y);
        float target = sqrtf(dx*dx+dy*dy);
        char str[128];
        sprintf(str, "Goto position: dx=%f dy=%f target=%f current orientation=%f", dx, dy, target, robotStatus.orientation);
        InterruptStackPrint::Instance().push(DEBUG_HEADER, str);
        //digitalWrite(LED2,HIGH);
        robotStatus.controlledP2P = false;
        translate((uint16_t) round(target));

        // Serial.printf("Target is %f current angle is %f (dx=%f dy=%f) (x=%f y=%f)\n", target, getAngle(), dx, dy, robotStatus.x, robotStatus.y);
    }

}

void MCS::manageStop() {
    static int timeCounter =0;

    averageRotationDerivativeError.add(rotationPID.getDerivativeError());
    averageTranslationDerivativeError.add(translationPID.getDerivativeError());
    bool controlled = ! robotStatus.forcedMovement;
    if(controlled && robotStatus.moving && ABS(translationPID.getDerivativeError())<= controlSettings.tolerancyDerivative && ABS(translationPID.getCurrentState()-translationPID.getCurrentGoal())<=controlSettings.tolerancyTranslation && ABS(rotationPID.getDerivativeError())<=controlSettings.tolerancyDerivative && ABS(rotationPID.getCurrentState()-rotationPID.getCurrentGoal())<=controlSettings.tolerancyAngle){
        leftMotor.setDirection(Direction::NONE);
        rightMotor.setDirection(Direction::NONE);
        robotStatus.Lbooly = robotStatus.controlledP2P;
        if(robotStatus.controlledP2P) {
            gotoTimer = MIN_TIME_BETWEEN_GOTO_TR_ROT;
        }
        stop();
        robotStatus.controlledP2P = robotStatus.Lbooly;
//        digitalWrite(LED1,HIGH);
    }
  //  digitalWrite(LED2,(ABS(leftSpeedPID.getCurrentState())<=0.25*controlSettings.tolerancySpeed));
   // digitalWrite(LED1,(ABS(rightSpeedPID.getCurrentState())<=0.25*controlSettings.tolerancySpeed));

    if((ABS(leftSpeedPID.getCurrentState())<0.4*ABS(leftSpeedPID.getCurrentGoal())) && ABS((rightSpeedPID.getCurrentState())<0.4*ABS(rightSpeedPID.getCurrentGoal())) && robotStatus.moving && expectedWallImpact){          //si robot a les deux roues bloquées
        if (timeCounter==100) {
            robotStatus.controlledRotation = true;

            leftMotor.setDirection(Direction::NONE);
            rightMotor.setDirection(Direction::NONE);
            expectedWallImpact = false;
            timeCounter = 0;
            robotStatus.stuck = true;
            InterruptStackPrint::Instance().push("blocage symétrique");
            stop();
        }
        timeCounter++;
    }
    else {
        timeCounter=0;
    }

//    digitalWrite(LED3,robotStatus.moving);
    if(ABS(ABS(leftSpeedPID.getCurrentState())-ABS(rightSpeedPID.getCurrentState()))>controlSettings.tolerancyDifferenceSpeed && robotStatus.moving){          //si le robot a une seule roue bloquée
        leftMotor.setDirection(Direction::NONE);
        rightMotor.setDirection(Direction::NONE);
        stop();
        robotStatus.stuck=true;
        digitalWrite(LED4,HIGH);

    }
    /*if(translationPID.getDerivativeError()==0 && ABS(translationPID.getCurrentOutput()-translationPID.getCurrentGoal())<=controlSettings.tolerancyTranslation && rotationPID.getDerivativeError()==0 && ABS(rotationPID.getCurrentOutput()-rotationPID.getCurrentGoal())<=controlSettings.tolerancyAngle){
        leftMotor.setDirection(Direction::NONE);
        rightMotor.setDirection(Direction::NONE);
        digitalWrite(LED1,HIGH);
    }*/
}

void MCS::stop() {
    digitalWrite(LED2,HIGH);
    leftMotor.stop();
    rightMotor.stop();

    if (robotStatus.stuck) {
        robotStatus.controlledP2P = false;
        robotStatus.Lbooly = false;
        robotStatus.moving = false;
        InterruptStackPrint::Instance().push(EVENT_HEADER, "unableToMove");
    }

    if(!robotStatus.controlledP2P) {
        InterruptStackPrint::Instance().push(EVENT_HEADER, "stoppedMoving");
    }

    if(robotStatus.controlledP2P) {
        InterruptStackPrint::Instance().push(DEBUG_HEADER, "controlledP2P");
    } else {
        InterruptStackPrint::Instance().push(DEBUG_HEADER, "NOT controlledP2P !!!");
    }
    robotStatus.controlledP2P = false;
    trajectory.clear();
    translationPID.resetErrors();
    rotationPID.resetErrors();
    leftSpeedPID.resetErrors();
    rightSpeedPID.resetErrors();

    leftSpeedPID.setGoal(0);
    rightSpeedPID.setGoal(0);

    translationPID.resetOutput(0);
    rotationPID.resetOutput(0);
    translationPID.setGoal(currentDistance);
    rotationPID.setGoal(robotStatus.orientation);

    robotStatus.speedTranslation = 0.0f;
    robotStatus.speedRotation = 0.0f;

    InterruptStackPrint::Instance().push("[DEBUG] On s'arrête!!");
    //if(robotStatus.movement != MOVEMENT::NONE) {
    //}
    robotStatus.movement = MOVEMENT::NONE;
    robotStatus.moving = false;
    robotStatus.stuck = false;
}

void MCS::translate(int16_t amount) {
    if(!robotStatus.controlledTranslation)
        return;
    targetDistance = amount;
    translationPID.fullReset();
    if(amount == 0) {
        translationPID.setGoal(currentDistance);
        robotStatus.moving = true;
        return;
    }
    robotStatus.movement = amount > 0 ? MOVEMENT::FORWARD : MOVEMENT::BACKWARD;
    translationPID.setGoal(amount + currentDistance);
    robotStatus.moving = true;
    digitalWrite(LED2,LOW);
}

void MCS::rotate(float angle) {
    rotationPID.active = false;
    if(!robotStatus.controlledRotation){
        return;
    }
    targetAngle = angle;

    float differenceAngle = robotStatus.orientation-targetAngle;
    while(ABS(differenceAngle) > PI)
    {
        float signe = ABS(differenceAngle)/differenceAngle;
        float ratio = floor(ABS(differenceAngle)/PI);
        targetAngle += signe*2*PI*ratio;


        differenceAngle = robotStatus.orientation-targetAngle;
    }

    if(1.57<ABS(differenceAngle)) {
        rotationPID.setTunings(3.5,0.000001,0,0);
    }
    else if (0.75<ABS(differenceAngle) and ABS(differenceAngle)<=1.57) {
        rotationPID.setTunings(5.1,0.000001,0,0);
    }
    else {
        rotationPID.setTunings(9,0.000001,0,0);
    }
    if( ! rotationPID.active) {
        rotationPID.fullReset();
        rotationPID.active = true;
    }
    robotStatus.movement = (differenceAngle < PI && differenceAngle > - PI) ? MOVEMENT::TRIGO : MOVEMENT::ANTITRIGO;

    rotationPID.setGoal(targetAngle);
    robotStatus.moving = true;
    digitalWrite(LED2,LOW);
}

/*void MCS::gotoPoint(int16_t x, int16_t y, bool sequential) {
    targetX = x;
    targetY = y;
    robotStatus.controlledP2P = true;
    sequentialMovement = sequential;
    robotStatus.moving = true;
}*/

void MCS::gotoPoint2(int16_t x, int16_t y) {
    targetX = x;
    targetY = y;
//    digitalWrite(LED2,LOW);
    float dx = x-robotStatus.x;
    float dy = y-robotStatus.y;
    ComMgr::Instance().printfln(DEBUG_HEADER, "goto %i %i (diff is %f %f) x= %f; y= %f", x, y, dx, dy, robotStatus.x, robotStatus.y);
    float rotation = atan2f(dy, dx);
    ComMgr::Instance().printfln(DEBUG_HEADER, "Required angle: %f", rotation);

    rotate(rotation);
    robotStatus.moving = true;
    robotStatus.controlledP2P = true;
}

void MCS::followTrajectory(const double* xTable, const double* yTable, int count) {
    trajectory.set(xTable, yTable, count);
    if(count > 0) { // s'il y a bien des points dans cette trajectoire
        std::pair<double, double> point = trajectory.query();
        gotoPoint(point.first, point.second, false);
    }
}

void MCS::stopTranslation() {
    robotStatus.speedTranslation = 0.0f;
}

void MCS::stopRotation() {
    robotStatus.speedRotation = 0.0f;
}

void MCS::speedBasedMovement(MOVEMENT movement) {
    if(!robotStatus.controlled)
    {
        return;
    }

    robotStatus.moving = true;

    switch(movement)
    {
        case MOVEMENT::FORWARD:
            robotStatus.speedTranslation = controlSettings.maxTranslationSpeed;
            break;

        case MOVEMENT::BACKWARD:
            robotStatus.speedTranslation = -controlSettings.maxTranslationSpeed;
            break;

        case MOVEMENT::TRIGO:
            robotStatus.speedRotation = controlSettings.maxRotationSpeed;
            break;

        case MOVEMENT::ANTITRIGO:
            robotStatus.speedRotation = -controlSettings.maxRotationSpeed;
            break;

        case MOVEMENT::NONE:
        default:
            leftSpeedPID.setGoal(0);
            rightSpeedPID.setGoal(0);
            robotStatus.speedRotation = 0;
            robotStatus.speedTranslation = 0;
            robotStatus.movement = MOVEMENT::NONE;
            return;
    }
    robotStatus.movement = movement;
}

void MCS::sendPositionUpdate() {
    ComMgr::Instance().printfln(POSITION_HEADER, "%f %f %f %li", robotStatus.x, robotStatus.y, robotStatus.orientation, millis());
}

void MCS::resetEncoders() {
    leftTicks = 0;
    rightTicks = 0;
    encoderLeft->write(0);
    encoderRight->write(0);
    previousLeftTicks = 0;
    previousRightTicks = 0;
    currentDistance = 0;
    translationPID.setGoal(currentDistance);
    rotationPID.setGoal(robotStatus.orientation);
}

void MCS::disableP2P() {
    trajectory.clear();
    robotStatus.controlledP2P = false;
}

void MCS::setControl(bool b) {
    robotStatus.controlled = b;
}

void MCS::controlledTranslation(bool b) {
    robotStatus.controlledTranslation = b;
}

void MCS::controlledRotation(bool b) {
    robotStatus.controlledRotation = b;
}

void MCS::setForcedMovement(bool newState) {
    robotStatus.forcedMovement = newState;
}

void MCS::setTranslationSpeed(float speed) {
    robotStatus.speedTranslation = speed;
}

void MCS::setRotationSpeed(float speed) {
    robotStatus.speedRotation = speed;
}

void MCS::setMaxTranslationSpeed(float speed) {
    controlSettings.maxTranslationSpeed = speed;
}

void MCS::setMaxRotationSpeed(float speed) {
    controlSettings.maxRotationSpeed = speed;
}

int16_t MCS::getX() {
    return (int16_t) robotStatus.x;
}

int16_t MCS::getY() {
    return (int16_t) robotStatus.y;
}

float MCS::getAngle() {
    return robotStatus.orientation;
}

void MCS::setX(int16_t x) {
    robotStatus.x = x;
}

void MCS::setY(int16_t y) {
    robotStatus.y = y;
}

void MCS::setAngle(float angle) {
    robotStatus.orientation = angle;
}

void MCS::setAngleOffset(float offset) {
    angleOffset = offset;
}

int32_t MCS::getLeftTicks() {
    return leftTicks;
}

int32_t MCS::getRightTicks() {
    return rightTicks;
}

float MCS::getLeftSpeed() {
    return robotStatus.speedLeftWheel;
}

float MCS::getRightSpeed() {
    return robotStatus.speedRightWheel;
}

void MCS::getSpeedGoals(long &leftGoal, long &rightGoal) {
    leftGoal = leftSpeedPID.getCurrentGoal();
    rightGoal = rightSpeedPID.getCurrentGoal();
}

void MCS::expectWallImpact()
{
    expectedWallImpact = true;
}

bool MCS::sentMoveAbnormal() {
    return robotStatus.sentMoveAbnormal;
}

bool MCS::isMoveAbnormal() {
    return robotStatus.stuck;
}

void MCS::setMoveAbnormalSent(bool val) {
    robotStatus.sentMoveAbnormal = val;
}