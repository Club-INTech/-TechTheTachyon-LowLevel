// SensorMgr.h

#ifndef _SENSORMGR_h
#define _SENSORMGR_h

#include <Arduino.h>
#include <array>
#include <algorithm>
#include <i2c_t3.h>
//#include <Wire.h>

#include "Utils/Median.h"
#include "Utils/Singleton.hpp"
#include "Utils/pin_mapping.h"
#include "MCS/MCSReborn.h"
#include "Utils/pin_mapping.h"
#include "COM/ComMgr.h"
#include "SRF10.h"
#include "VL6180X.h"
#include "AbstractSensorUS.h"
#include "MCS/RobotStatus.h"


class SensorMgr : public Singleton<SensorMgr>
{


private:

	ComMgr& highLevel;

	SRF10* US[NBR_OF_US_SENSOR];
	VL6180X sensorCubeAV;
	VL6180X sensorCubeAR;

	std::array<Median<uint16_t ,MEDIAN_US_SIZE>,NBR_OF_US_SENSOR> distances;
	uint8_t currentMeasuringUS = 0;
	bool isMeasuring = false;
	bool firstMeasure = true;
	bool sendRequest= false;
	MOVEMENT measure_direction;

	bool jumperPlugged;
	bool basicBlocked;

	float meanAmbiantLight = CUBE_AVAR_DETECTION_AMBIANT_LUX;
public:

	SensorMgr();
	void init();
	void refreshUS(MOVEMENT dir);
	void checkCubeAV();
	void checkCubeAR();
	void measureMeanAmbientLight();
	void sendUS();

	bool isJumperEngaged();
	bool isReadyToGo();
	bool isCont1Engaged();

};

#endif
