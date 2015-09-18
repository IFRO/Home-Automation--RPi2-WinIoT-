﻿#include <Wire.h>  

/* Arduino's I2C Slave Address */
#define SLAVE_ADDRESS 0x40

/* PIN DECLARATION */
int Pin_AmbientLight_LDR = A0;
int Pin_PassiveIR = 2;
int Pin_Temperature = A1;

/* Global Variable */
short Value_AmbientLight_LDR, Value_Temperature;
bool Value_PassiveIR;

/* Protocol Variable */
byte Mode, Pin, Value;
byte Response[14];


void setup()
{
	pinMode(0, OUTPUT);
	pinMode(1, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(4, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(6, OUTPUT);
	pinMode(7, OUTPUT);
	pinMode(8, OUTPUT);
	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);
	pinMode(12, OUTPUT);
	pinMode(13, OUTPUT);
	pinMode(A2, OUTPUT);
	pinMode(A3, OUTPUT);

# ifdef _DEBUG_
	Serial.begin(9600);
#endif

	Wire.begin(SLAVE_ADDRESS);
	Wire.onRequest(SendData);
	Wire.onReceive(ReceiveData);
}

void loop()
{
	// Read LDR
	//         Arduino supports 10-bit Analog Read.
	//             Thus we need to convert it into 8-bit.
	//         Next thing is we have connected LDR in
	//             Pull-Up mode. So that we need to invert
	//             value. To do so, map function is used.
	Value_AmbientLight_LDR = analogRead(Pin_AmbientLight_LDR);
	Value_AmbientLight_LDR = map(Value_AmbientLight_LDR, 0, 1023, 255, 0);

	// Read PassiveIR value
	Value_PassiveIR = (digitalRead(Pin_PassiveIR) == HIGH) ? true : false;

	// Read Temperature Sensor and Convert Voltage into Celsius
	Value_Temperature = analogRead(Pin_Temperature);
	Value_Temperature = Value_Temperature * 0.48828125;

	// Wait for 100 ms
	delay(100);
}

// callback for received data
void ReceiveData(int byteCount)
{
	Mode = Wire.read();
	Pin = Wire.read();
	Value = Wire.read();

	if (Mode == 2)
	{
		digitalWrite(Pin, Value);
	}

# ifdef _DEBUG_
	Serial.print(Mode);
	Serial.print(" ");
	Serial.print(Pin);
	Serial.print(" ");
	Serial.println(Value);
#endif
}

void SendData()
{
	switch (Mode)
	{
	case 0: // Mode: Read Sensor
		Response[0] = (byte)Value_AmbientLight_LDR;

		// Value_PassiveIR is boolean so that we need to convert it into byte
		Response[1] = (byte)((Value_PassiveIR == true) ? 1 : 0);

		// Response[2] byte is Sign byte for Temperature
		//         0 - +ve Temperature
		//         1 - -ve Temperature
		Response[2] = (byte)((Value_Temperature < 0) ? 0 : 1);

		// -ve Temperature can't be sent in byte. Convert it into +ve equivalent
		Response[3] = (byte)((Value_Temperature < 0) ? (Value_Temperature*(-1)) : Value_Temperature);
		break;
	case 1: // Mode: Read Devices State
		Response[0] = (digitalRead(0) == HIGH) ? 1 : 0;
		Response[1] = (digitalRead(1) == HIGH) ? 1 : 0;
		Response[2] = (digitalRead(3) == HIGH) ? 1 : 0;
		Response[3] = (digitalRead(4) == HIGH) ? 1 : 0;
		Response[4] = (digitalRead(5) == HIGH) ? 1 : 0;
		Response[5] = (digitalRead(6) == HIGH) ? 1 : 0;
		Response[6] = (digitalRead(7) == HIGH) ? 1 : 0;
		Response[7] = (digitalRead(8) == HIGH) ? 1 : 0;
		Response[8] = (digitalRead(9) == HIGH) ? 1 : 0;
		Response[9] = (digitalRead(10) == HIGH) ? 1 : 0;
		Response[10] = (digitalRead(11) == HIGH) ? 1 : 0;
		Response[11] = (digitalRead(12) == HIGH) ? 1 : 0;
		Response[12] = (digitalRead(A2) == HIGH) ? 1 : 0;
		Response[13] = (digitalRead(13) == HIGH) ? 1 : 0;
		break;
	case 2: // Mode: Set Device State
		Response[0] = (digitalRead(Pin) == HIGH) ? 1 : 0;
		break;
	default:
		break;
	}
	Wire.write(Response, 14);
}