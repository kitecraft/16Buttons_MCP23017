#pragma once
#include <Arduino.h>

#include <MCP23017.h>
#include "IceButton.h"

#define IBB_NUMBER_BUTTONS 16
#define IBB_BUTTON_PRESSED 0
#define IBB_BUTTON_RELEASED 1

#define MCP23017_INT_ERR 255 //!< Interrupt error
#define MCP23017_INTCAPA 0x10  //!< Interrupt captured value for port register A
#define MCP23017_INTCAPB 0x11  //!< Interrupt captured value for port register B

#define IBB_NO_BTN -1

static volatile bool g_mcpButtonController_Interrupted = false;
static void IRAM_ATTR McpButtonInterruptFunction()
{
	g_mcpButtonController_Interrupted = true;
}

class IceButtonBoard
{
	IceButton _buttons[IBB_NUMBER_BUTTONS];
	int _mcInterruptPin = -1;

	MCP23017* _mcpController = NULL;
	int8_t getLastInterruptPin();
	uint8_t getLastInterruptPinValue();
	uint8_t regForPin(uint8_t pin, uint8_t portAaddr, uint8_t portBaddr);
	uint8_t bitForPin(uint8_t pin);

	IceButtonEvent GetLastButtonPressed();

public:
	IceButtonBoard();
	~IceButtonBoard();
	void Init(uint8_t mcp23017Address, int interruptPin);
	void ConfigureButton(int btn, uint16_t debounceTime, IceButtonAction action);

	// Returns the ID of the button
	// IBB_NO_BTN (-1) if no pin
	IceButtonEvent Process();
};

