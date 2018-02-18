#include <PololuG2.h>
#include <PS2X_lib.h>
#include <NetworkTable.h>
#include <AStar32U4.h>
#include <Ddrive.h>
//#include <LSMHeadless.h>

// Construct Drive Chassis
PololuG2 motorL = PololuG2(2, 9, 7, true);
PololuG2 motorM = PololuG2(4, 10, 8, true);
PololuG2 motorR = PololuG2(3, 5, 11, true);

Ddrive chassis = Ddrive(motorL, motorR, motorM);

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
		
		if(Select_Pressed)
		{
			armMotors = !armMotors;
		}

		if(Square)
		{
			chassis.reverseMotor(1, !chassis.getReverseValue(1));
		}
		if(Triangle)
		{
			chassis.reverseMotor(3, !chassis.getReverseValue(3));
		}
		if(Circle)
		{
			chassis.reverseMotor(2, !chassis.getReverseValue(2));
		}

		// Chassis Control
		Vec2 vec = Vec2(ps2x.JoyStick(PSS_LX), -ps2x.JoyStick(PSS_LY));
		chassis.drive(Vec2::angle(vec), Vec2::magnitude(vec), -ps2x.JoyStick(PSS_RX));
	}
	PololuG2::iterate();
	if (network.getLastPS2PacketTime() > 500 || !armMotors)
	{
		digitalWrite(2, LOW);
		digitalWrite(4, LOW);
		digitalWrite(3, LOW);
	} else {
		digitalWrite(2, HIGH);
		digitalWrite(4, HIGH);
		digitalWrite(3, HIGH);
	}
}
