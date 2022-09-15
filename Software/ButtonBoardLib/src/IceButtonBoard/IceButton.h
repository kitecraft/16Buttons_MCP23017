#pragma once
#include <Arduino.h>

#define IBB_DEFAULT_DEBOUNCE_TIME 25

enum IceButtonAction {
	ICEBUTTON_DOWN,
	ICEBUTTON_UP,
	ICEBUTTON_BOTH
};

typedef struct {
	int buttonID = -1;
	IceButtonAction action;
}IceButtonEvent;

class IceButton
{
private:
	unsigned long _lastDebounceTime = 0;  // the last time the output pin was toggled
	uint16_t _debounceTime = IBB_DEFAULT_DEBOUNCE_TIME;
	IceButtonAction _action = ICEBUTTON_DOWN;

public:
	IceButton() {};
	IceButton(uint16_t debounceTime, IceButtonAction action)
	{ 
		_debounceTime = debounceTime;
		_action = action;
	}

	IceButtonAction Action() { return _action; }
	void Configure(uint16_t debounceTime, IceButtonAction action)
	{
		_debounceTime = debounceTime;
		_action = action;
	}

	bool Ready() {
		if ((millis() - _lastDebounceTime) > _debounceTime)
		{
			_lastDebounceTime = millis();
			return true;
		}
		return false;
	}
};