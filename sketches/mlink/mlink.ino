#include "mLink.h"

mLink mLink;

#define I2C_ADD BPAD_I2C_ADD

void setup() {
	Serial.begin(115200);
	mLink.init();
}

String boolToString(bool value) {
	return value ? "true" : "false";
}

void loop() {
	bool left = mLink.bPad_DownState(I2C_ADD);
	bool right = mLink.bPad_UpState(I2C_ADD);
	bool up = mLink.bPad_LeftState(I2C_ADD);
	bool down = mLink.bPad_RightState(I2C_ADD);
	bool select = mLink.bPad_SelectState(I2C_ADD);
	bool back = mLink.bPad_BackState(I2C_ADD);

	String json =
		"{\"up\"" + boolToString(up) +
		",\"left\":" + boolToString(left) +
		",\"right\":" + boolToString(right) +
		",\"down\":" + boolToString(down) +
		",\"select\":" + boolToString(select) +
		",\"back\":" + boolToString(back) +
		"}";
	Serial.println(json);
}