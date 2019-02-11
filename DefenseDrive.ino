#include <PololuG2.h>
#include <PS2X_lib.h>
#include <NetworkTable.h>
#include <AStar32U4.h>
#include <TankDrive.h>
//#include <LSMHeadless.h>

// Construct Drive Chassis
PololuG2 motorL = PololuG2(6, 9, 7, true);// pins
//PololuG2 motorL = PololuG2(12, 5, 11, true);//shield2
PololuG2 motorR = PololuG2(4, 10, 8, true);//shield1

// Drop Lift Subsystem Motors
//PololuG2 motorDropL = PololuG2(13, 11, 12, true);
//PololuG2 motorDropR = PololuG2(5, 3, 2, true);
PololuG2 motorDropL = PololuG2(5, 3, 2, true);
PololuG2 motorDropR = PololuG2(13, 11, 12, true);
TankDrive chassis = TankDrive(motorL, motorR);

// Network & Contoller
PS2X ps2x;
NetworkTable network = NetworkTable(10, 10);
PacketSerial myPacketSerial;

// Safety
bool armMotors = true;

// Timers
unsigned long last_update;

// Reverses
bool reverseLeft;
bool reverseRight;

void setup()
{
	Serial.begin(115200);
	Serial1.begin(115200);
	myPacketSerial.setStream(&Serial1);
	myPacketSerial.setPacketHandler([](const uint8_t* buffer, size_t size) {
		network.processPacketFromSender(myPacketSerial, buffer, size);
	});
	chassis.reverseLeftMotors(reverseLeft = !reverseLeft);

	ps2x.config_gamepad();
	ps2x.read_gamepad();
	network.setPS2(ps2x);
	
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
		bool Triangle_Pressed= ps2x.ButtonPressed(PSB_TRIANGLE);
		bool Triangle_Released= ps2x.ButtonReleased(PSB_TRIANGLE);
		bool Cross_Pressed = ps2x.ButtonPressed(PSB_CROSS);
		bool Cross_Released = ps2x.ButtonReleased(PSB_CROSS);

		bool Square = ps2x.ButtonPressed(PSB_SQUARE);
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
			chassis.reverseLeftMotors(reverseLeft = !reverseLeft);
		if(Circle)
			chassis.reverseRightMotors(reverseRight = !reverseRight);

		// Drop Subsystem
		if(R1_Pressed)
			motorDropR.setPower(0.7);

		if(R2_Pressed)
			motorDropR.setPower(-0.7);

		if(L1_Pressed)
			motorDropL.setPower(-0.7);

		if(L2_Pressed)
			motorDropL.setPower(0.7);

		if(Triangle_Pressed)
		{
			motorDropR.setPower(0.7);
			motorDropL.setPower(-0.7);
		}

		if(Cross_Pressed)
		{
			motorDropR.setPower(-0.7);
			motorDropL.setPower(0.7);
		}

		if(R1_Released || R2_Released || L1_Released || L2_Released || Cross_Released || Triangle_Released)
		{
			motorDropL.setPower(0);
			motorDropR.setPower(0);
		}



			
			

		// Chassis Control
		chassis.drive(ps2x);
		// Reset Button Flags
		ps2x.read_gamepad(); // clear release&pressed flags.
	}
	PololuG2::iterate();
	if (network.getLastPS2PacketTime() > 500 || !armMotors)
	{
		digitalWrite(6, LOW);
		digitalWrite(4, LOW);
		digitalWrite(13, LOW);
		digitalWrite(5, LOW);
	} else {
		digitalWrite(6, HIGH);
		digitalWrite(4, HIGH);
		digitalWrite(13, HIGH);
		digitalWrite(5, HIGH);
	}
}
