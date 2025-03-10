#include <ArduinoBLE.h>
#include <Arduino_BMI270_BMM150.h>


BLEService bleService("19b10000-e8f2-537e-4f6c-d104768a1214");
BLEStringCharacteristic bleCharacteristic("19b10010-e8f2-537e-4f6c-d104768a1214", BLERead | BLENotify, 50);


void setup() {
	Serial.begin(115200);
	while (!Serial);

	if (!BLE.begin()) {
		Serial.println("Failed to initialize BLE!");
		while (1);
	}

	if (!IMU.begin()) {
		Serial.println("Failed to initialize IMU!");
		while (1);
	}

	BLE.setLocalName("IMU Remote");
	BLE.setAdvertisedService(bleService);
	bleService.addCharacteristic(bleCharacteristic);
	BLE.addService(bleService);
	bleCharacteristic.writeValue("0");
	BLE.advertise();

	Serial.println("Bluetooth device active, waiting for connections...");
}


void loop() {
	BLEDevice central = BLE.central();

	if (central) {
		Serial.print("Connected to central: ");
		Serial.println(central.address());

		while (central.connected()) {
			String bleString = "Whirled peas";
			Serial.println(bleString);
			bleCharacteristic.writeValue(bleString);
		}

		Serial.print("Disconnected from central: ");
		Serial.println(central.address());
	}
}
