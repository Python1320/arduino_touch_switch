#include <CapacitiveSensor.h>
// Uncomment to enable serial debug output
#define DBG

// Around 1-20 Megaohm resistor between pins 7 and 8, pin 8 is sensor to which to connect foil
CapacitiveSensor   cs = CapacitiveSensor(7, 8);

//TODO: Get rid of floats and delay() calls
//TODO: Examine better hysteresis technique

float thres; // threshold for triggering
float const thres_add = 90.0; // ADJUST THIS! The value added to baseline measure on start of device over which to start measuring filter_time

float q = 0.95; // must be tweaked based on delay(), smoothness of measures over time

long const filter_time = 200; // milliseconds to wait before toggling

void setup()
{

#ifdef DBG
	Serial.begin(9600);
#endif
	pinMode(12, OUTPUT);
	
	cs.set_CS_AutocaL_Millis(0xFFFFFFFF); // disable automatic calib, we do not need it
	
	
	// Try to measure an initial threshold value to use for the rest of the time
	
	for (int i = 1; i<200; i++) {
		cs.capacitiveSensorRaw(11);
		delay(22);
	}
	
	thres = ((float)(cs.capacitiveSensorRaw(11))) + thres_add;

#ifdef DBG
	Serial.print("thres: ");
	Serial.println(thres);
	delay(4000);
#endif
}

void loop()
{
	float c = (float)cs.capacitiveSensorRaw(11);
	static float a = c;
	static float startt = 0;
	static bool toggled = false;
	static bool toggle = false;

	//TODO: dc offset compensation
	// thres = thres*
	
	// Very simple "filter"
	a = a*q + ((float)c)*(1.0f - q);
	// wait for thres to go over measured value and start counting time and toggle pin after timeout and wait for measuer to go below threshold
	if (a>thres) 
	{
		long now = millis();
		if (startt == 0) 
		{
			startt = now;
		}
		else if ((now - startt) > filter_time && !toggled) 
		{
			toggled = true;
			toggle = !toggle;

			digitalWrite(12, toggle ? HIGH : LOW);

		}

	}
	else  if (startt>0)
	{
		startt = 0;
		toggled = false;
	}
#ifdef DBG
	Serial.print(toggle ? "X " : "_ ");
	Serial.print(toggled ? "X " : "_ ");
	Serial.print(startt);
	Serial.print("\t");
	Serial.println(a);
	delay(10);
#else
	delay(5);
#endif

}
