#include "IceButtonBoard.h"

IceButtonBoard::IceButtonBoard()
{

}

IceButtonBoard::~IceButtonBoard()
{
	if (_mcpController != NULL)
	{
		delete _mcpController;
		_mcpController = NULL;
	}
}


void IceButtonBoard::Init(uint8_t mcp23017Address, int interruptPin)
{
	_mcpController = new MCP23017(mcp23017Address);
	_mcInterruptPin = interruptPin;

	_mcpController->init();
	_mcpController->portMode(MCP23017Port::A, 0b11111111, 0b11111111);
	_mcpController->portMode(MCP23017Port::B, 0b11111111, 0b11111111);

	_mcpController->interruptMode(MCP23017InterruptMode::Or);
	_mcpController->interrupt(MCP23017Port::A, CHANGE);
	_mcpController->interrupt(MCP23017Port::B, CHANGE);

	_mcpController->writeRegister(MCP23017Register::IPOL_A, 0x00);
	_mcpController->writeRegister(MCP23017Register::IPOL_B, 0x00);


	pinMode(_mcInterruptPin, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(_mcInterruptPin), McpButtonInterruptFunction, FALLING);
	_mcpController->clearInterrupts();
}

IceButtonEvent IceButtonBoard::GetLastButtonPressed()
{
	IceButtonEvent buttonEvent;
	int pin = getLastInterruptPin();
	uint8_t action = getLastInterruptPinValue();
	_mcpController->clearInterrupts();
	switch (action) {
	case IBB_BUTTON_PRESSED:
		if (_buttons[pin].Ready() &&
			(_buttons[pin].Action() == ICEBUTTON_DOWN || _buttons[pin].Action() == ICEBUTTON_BOTH))
		{
			buttonEvent.buttonID = pin;
			buttonEvent.action = ICEBUTTON_DOWN;
			return buttonEvent;
		}
		break;
	case IBB_BUTTON_RELEASED:
		if (_buttons[pin].Ready() &&
			(_buttons[pin].Action() == IBB_BUTTON_RELEASED || _buttons[pin].Action() == ICEBUTTON_BOTH))
		{
			buttonEvent.buttonID = pin;
			buttonEvent.action = ICEBUTTON_UP;
			return buttonEvent;
		}
		break;
	default:
		return IceButtonEvent();
	}

	return IceButtonEvent();
}



int8_t IceButtonBoard::getLastInterruptPin()
{
	uint8_t intf;

	// try port A
	intf = _mcpController->readRegister(MCP23017Register::INTF_A);
	for (int i = 0; i < 8; i++) {
		if (bitRead(intf, i)) {
			return i;
		}
	}

	// try port B
	intf = _mcpController->readRegister(MCP23017Register::INTF_B);
	for (int i = 0; i < 8; i++) {
		if (bitRead(intf, i)) {
			return i + 8;
		}
	}

	return MCP23017_INT_ERR;
}

uint8_t IceButtonBoard::getLastInterruptPinValue()
{
	uint8_t intPin = getLastInterruptPin();
	if (intPin != MCP23017_INT_ERR) {
		uint8_t intcapreg = regForPin(intPin, MCP23017_INTCAPA, MCP23017_INTCAPB);
		uint8_t bit = bitForPin(intPin);
		return (_mcpController->readRegister(intcapreg) >> bit) & (0x01);
	}

	return MCP23017_INT_ERR;
}
uint8_t IceButtonBoard::bitForPin(uint8_t pin)
{
	return pin % 8;
}

uint8_t IceButtonBoard::regForPin(uint8_t pin, uint8_t portAaddr, uint8_t portBaddr)
{
	return (pin < 8) ? portAaddr : portBaddr;
}



IceButtonEvent IceButtonBoard::Process()
{
	if (g_mcpButtonController_Interrupted)
	{
		g_mcpButtonController_Interrupted = false;
		return GetLastButtonPressed();
	}
	return IceButtonEvent();
}

void IceButtonBoard::ConfigureButton(int btn, uint16_t debounceTime, IceButtonAction action) 
{
	if (btn < 0 || btn > 15) {
		return;
	}

	_buttons[btn].Configure(debounceTime, action);
}