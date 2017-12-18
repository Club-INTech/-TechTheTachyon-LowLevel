#include <Utils/utils.h>
#include <Utils/defines.h>
#include "Hook.h"


Hook::Hook(uint8_t id, uint32_t x, uint32_t y, uint32_t r, float alpha, float tolerance, const char* o) : hookID(id), zoneX(x), zoneY(y), zoneR(r),angleTarget(alpha),angleTolerance(tolerance), order(o)
{
	active = true;
	ready = false;
}



bool Hook::check(uint32_t currentX, uint32_t currentY, float currentAngle)
{
	Serial.println("On fait un check des hooks avec les positions suivantes");
	Serial.print("Angle actuel: ");
	Serial.println(currentAngle);
	Serial.print("Angle cible: ");
	Serial.println(angleTarget);
	Serial.println(ABS(currentAngle-angleTarget));
	Serial.println(angleTolerance);
	Serial.println("==================");
	return ((zoneX-currentX)*(zoneX-currentX) + (zoneY - currentY)*(zoneY - currentY) <= zoneR*zoneR
			&& (ABS(currentAngle-angleTarget) <= angleTolerance));
}

const char* Hook::getOrder()
{
	return order;
}

void Hook::setReady() {
	ready = true;
	
}

bool Hook::isReady() {
	return ready;
}

void Hook::setActive(bool state) {
	active = state;
}

bool Hook::isActive() {
	return active;
}