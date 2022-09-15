/*
 Name:		ButtonBoardLib.ino
 Created:	6/17/2022 7:45:32 PM
 Author:	Kitecraft
*/

#include "src/IceButtonBoard/IceButtonBoard.h"

#define MCP_BUTTON_CONTROLLER_ADDRESS 0x20
#define MCP_BUTTON_CONTROLLER_INTERUPT_PIN 36
IceButtonBoard g_buttonBoard;

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	Wire.begin();
	g_buttonBoard.Init(MCP_BUTTON_CONTROLLER_ADDRESS, MCP_BUTTON_CONTROLLER_INTERUPT_PIN);

	g_buttonBoard.ConfigureButton(0, 25, IceButtonAction::ICEBUTTON_UP);
	g_buttonBoard.ConfigureButton(1, 25, IceButtonAction::ICEBUTTON_BOTH);

	Serial.println("Setup end");
}

// the loop function runs over and over again until power down or reset
void loop() {
	
	IceButtonEvent btnEvent = g_buttonBoard.Process();
	if (btnEvent.buttonID != IBB_NO_BTN) {
		Serial.print("Button pressed: ");
		Serial.print(btnEvent.buttonID);
		Serial.print("  Action: ");
		Serial.println(btnEvent.action);
	}
}
