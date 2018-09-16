#pragma once
#include "os/os.h"

class PowerSwitch {
private:
	int nsecs_left;
	int percent_left;
	OS::PowerState power_state;

	bool GetPowerInfo_Switch();

public:
	PowerSwitch();
	virtual ~PowerSwitch();

	OS::PowerState get_power_state();
	int get_power_seconds_left();
	int get_power_percent_left();
};