#include <PololuG2.h>
#include <PS2X_lib.h>
#include <NetworkTable.h>
#include <AStar32U4.h>
#include <HDrive.h>
//#include <LSMHeadless.h>

// Construct Drive Chassis
PololuG2 motorL = PololuG2(6, 9, 7, true);
PololuG2 motorM = PololuG2(4, 10, 8, true);
PololuG2 motorR = PololuG2(12, 5, 11, true);

HDrive chassis = HDrive(motorL, motorR, motorM);

// Network & Contoller
PS2X ps2x;
NetworkTable network = NetworkTable(10, 10);
PacketSerial myPacketSerial;

// Safety
bool armMotors = true;

// Timers
unsigned long last_update;

void setup()
{
	Serial.begin(115200);
	Serial1.begin(115200);
	myPacketSerial.setStream(&Serial1);
	myPacketSerial.setPacketHandler([](const uint8_t* buffer, size_t size) {
		network.processPacketFromSender(myPacketSerial, buffer, size);
	});

	ps2x.config_gamepad();
	ps2x.read_gamepad();
	network.setPS2(ps2x);
	chassis.reverseMotor(1, true);
	chassis.reverseMotor(2, true);
	
	delay(1500);
}

void ledService()
{
}

void loop()
{
	myPacketSerial.update();
	if (millis() - last_update > 9)
	{
		// Button Variables
		bool L1 = ps2x.Button(PSB_L1);
		bool L2 = ps2x.Button(PSB_L2);
		bool R1 = ps2x.Button(PSB_R1);
		bool R2 = ps2x.Button(PSB_R2);
		bool Triangle = ps2x.ButtonPressed(PSB_TRIANGLE);
		bool Square = ps2x.ButtonPressed(PSB_SQUARE);
		bool Cross = ps2x.ButtonPressed(PSB_CROSS);
		bool Circle = ps2x.ButtonPressed(PSB_CIRCLE);
		bool PAD_Up = ps2x.Button(PSB_PAD_UP);
		bool PAD_Down = ps2x.Button(PSB_PAD_DOWN);
		bool R1_Pressed = ps2x.ButtonPressed(PSB_R1);
		bool R1_Released = ps2x.ButtonReleased(PSB_R1);
		bool L1_Pressed = ps2x.ButtonPressed(PSB_L1);
		bool L1_Released = ps2x.ButtonReleased(PSB_L1);
		bool R2_Pressed = ps2x.ButtonPressed(PSB_R2);
		bool R2_Released = ps2x.ButtonReleased(PSB_R2);
		bool L2_Pressed = ps2x.ButtonPressed(PSB_L2);
		bool L2_Released = ps2x.ButtonReleased(PSB_L2);
		bool Select_Pressed = ps2x.ButtonPressed(PSB_SELECT);
		bool Start_Pressed = ps2x.ButtonPressed(PSB_START);
		// Reverse Motors Buttons
		if(Square)
			chassis.reverseMotor(chassis.HDRIVE_MOTOR::LEFT, !chassis.getReverseMotorValue(chassis.HDRIVE_MOTOR::LEFT));
		if(Triangle)
			chassis.reverseMotor(chassis.HDRIVE_MOTOR::MIDDLE, !chassis.getReverseMotorValue(chassis.HDRIVE_MOTOR::MIDDLE));
		if(Circle)
			chassis.reverseMotor(chassis.HDRIVE_MOTOR::RIGHT, !chassis.getReverseMotorValue(chassis.HDRIVE_MOTOR::RIGHT));
			

		// Chassis Control
		float strafeFactor = 0;
		if(L1)
			strafeFactor = 1;
		if(R1)
			strafeFactor = -1;
		chassis.drive(ps2x.JoyStick(PSS_LY), -ps2x.JoyStick(PSS_RY), strafeFactor);
		// Reset Button Flags
		ps2x.read_gamepad(); // clear release&pressed flags.
	}
	PololuG2::iterate();
	if (network.getLastPS2PacketTime() > 500 || !armMotors)
	{
		digitalWrite(6, LOW);
		digitalWrite(4, LOW);
		digitalWrite(12, LOW);
	} else {
		digitalWrite(6, HIGH);
		digitalWrite(4, HIGH);
		digitalWrite(12, HIGH);
	}
}
