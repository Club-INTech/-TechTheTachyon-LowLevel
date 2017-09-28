/*
*	Classe mod�lisant un hook (action automatique effectu�e quand des conditions de position sont r�alis�es)
*/

#ifndef _HOOK_h
#define _HOOK_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "wprogram.h"
#else
	#include "WProgram.h"
#endif

#include "ActuatorsMgr.hpp"
#include "MotionControlSystem.h"

class Hook
{
public:
	enum HookType {
		TEST
	};
	Hook(float x, float y, float o, float delta, HookType type);

private:
	MotionControlSystem& motionControlSystem;
	ActuatorsMgr& actuatorsMgr;

	HookType type;
};

#endif