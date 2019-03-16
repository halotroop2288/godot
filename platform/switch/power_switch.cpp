#include "switch_wrapper.h"
#include "power_switch.h"

PowerSwitch::PowerSwitch()
{
	nsecs_left = 0;
	percent_left = 0;
	power_state = OS::PowerState::POWERSTATE_ON_BATTERY;
}

PowerSwitch::~PowerSwitch()
{}

OS::PowerState PowerSwitch::get_power_state()
{
	return power_state;
}

int PowerSwitch::get_power_seconds_left()
{
	return nsecs_left / 1000000;
}

int PowerSwitch::get_power_percent_left()
{
	return percent_left;
}
