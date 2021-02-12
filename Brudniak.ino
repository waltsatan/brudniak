//settings:
#include "settings.h"


//code:
#include <Wire.h>
#include "src/VL53L1X/VL53L1X.h"
#include "src/RunningAverage/RunningAverage.h"
#include "Laser.h"
#include "Dispatch.h"


VL53L1X sensor;
RunningAverage avg=RunningAverage(AVG);
Laser laser(LASER_PIN);

//timing vars
int updateSensorMillis=0;
unsigned long lastSensorMillis;

uint8_t updateSpeedMillis=0;//1000./60;
unsigned long lastUpdateMillis=0;

//dispatcher
Dispatch dispatch=Dispatch();

//states
unsigned long msInState;
int numInLoop;
typedef enum State {
	RESET,
	IDLE,
	GLITCH_ON,
	PREWAKE,
	WAKE,
	WAKE_RUN,
	AWOKE,
	LURING,
	LAST_GASP_WAIT,
	LAST_GASP,
	GLITCH_OFF,
	BREATH,
	BREATH_RUN,
	RUN

} State;
 
State state=RESET;
State lastState;



void setup() {
	delay(500);
	Serial.begin(9600);
	delay(250);
	Wire.begin();
	Wire.setClock(400000); // use 400 kHz I2C
	randomSeed(analogRead(A0));
	

	sensor.setTimeout(500);
	
	
	if (!sensor.init()) {
		Serial.println("Failed to detect and initialize sensor!");
		while (1);
	}
  
	// Use long distance mode and allow up to 50000 us (50 ms) for a measurement.
	// You can change these settings to adjust the performance of the sensor, but
	// the minimum timing budget is 20 ms for short distance mode and 33 ms for
	// medium and long distance modes. See the VL53L1X datasheet for more
	// information on range and timing limits.
	sensor.setDistanceMode(VL53L1X::Long);
	sensor.setMeasurementTimingBudget(50000);

	// Start continuous readings at a rate of one measurement every 50 ms (the
	// inter-measurement period). This period should be at least as long as the
	// timing budget.
	sensor.startContinuous(50);
	delay(250);
	Serial.begin("Lasers ----x Written for Steve Brudniak by Alan Watts");
	
	//laser.set(Laser::GLITCH_ON, millis(), 4000);
	
	//laser.setRate(0.5);
	
	setState(RESET,(char *)"RESET");
}

bool setState(State s, char *stateString) {
	if ( s != lastState ) {
		state=s;
		Serial.print("State change: ");
		Serial.print(stateString);
		Serial.print(" (");
		Serial.print(s);
		Serial.println(")");
		msInState=millis();
		numInLoop=0;
		lastState=s;
		return(true);
	} else {
		return(false);
	}
		
}

void stateDispatch(unsigned long ms, int v) {
	setState((State)v, ">");
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


bool leaving=false;

void loop() {
	unsigned long ms=millis();	 
	
	//if ( (ms-lastSensorMillis) > updateSensorMillis ) {
	sensor.read();
	//	lastSensorMillis=ms;
	//}
	
	//if ( (ms-lastUpdateMillis) > updateSpeedMillis ) {
	dispatch.update(ms);
		
		//lastUpdateMillis=ms;
	//}
	
	avg.addValue(constrain(sensor.ranging_data.range_mm, CLOSEST_MM, FAR_MM));
	 
	unsigned long a=avg.getFastAverage();
	
	
	switch(state) {
		case RESET: {
			avg.clear();
			avg.fillValue(FAR_MM, AVG);
			leaving=false;
			
			laser.off();
			setState(IDLE,(char *)"IDLE");
			break;
			
		}	
		
		case IDLE: {
			//waiting for someone, did they get in my zone?
			if ( a < FAR_MM ) {
				setState(GLITCH_ON,(char *)"GLITCH_ON");
			}
			
			break;
			
		}	
		case GLITCH_ON: {
			int glitchLengh=random(600,1200);
			//laser.setBrightness( GLITCH_BRIGHTNESS );
			
			laser.set(Laser::GLITCH_ON, ms, glitchLengh,0,0);
			dispatch.setCallback(stateDispatch,glitchLengh,ms,PREWAKE);
			setState(RUN,(char *)"RUN");
			break;
		}
		
		case PREWAKE: {
			laser.off();
		 	 
			//dispatch.setCallback(stateDispatch,YAWN_LENGTH/5,ms,WAKE);
			setState(WAKE,(char *)"WAKE");
			break;
		}
		
		
		case WAKE: {
			float wakeSpeed = 1000/(mapFloat(a*a, CLOSE_MM_SQUARED, FAR_MM_SQUARED, BREATH_MAX, BREATH_MIN));
			
			laser.set(Laser::FADE_ON, ms, wakeSpeed, 0, 255);
			 
			dispatch.setCallback(stateDispatch,wakeSpeed+100,ms,AWOKE);
				
			setState(WAKE_RUN,(char *)"WAKE_RUN");
			break;
		}
		
		case WAKE_RUN: {
			laser.setBrightness( map(a*a, CLOSE_MM_SQUARED, FAR_MM_SQUARED, BRIGHTNESS_MAX, BRIGHTNESS_MIN));
			break;
			
		}
		case AWOKE: {
			//slow the breathing down (need rate)
			laser.set(Laser::BREATH, ms, YAWN_LENGTH,0,1);
			laser.setRate(0.5);
			
			//dispatch.setCallback(stateDispatch,6000,ms,GLITCH_OFF);
			
			setState(LURING,(char *)"LURING");
			break;
			
		}
		
		case LURING: {
			//normal running, get distance and change rate/brightness
			
			
			if ( a >= FAR_MM && ! dispatch.isRunning) {
				leaving=true;
				
				if ( ms-msInState > MIN_LURING_MS ) {
					setState(GLITCH_OFF,(char *)"GLITCH_OFF");
				} else {
					dispatch.setCallback(stateDispatch, MIN_LURING_MS-(ms-msInState), ms, GLITCH_OFF);
				}
			 
			}
			
			if ( !leaving ) {
			 
				laser.setBrightness( map(a*a, CLOSE_MM_SQUARED, FAR_MM_SQUARED, BRIGHTNESS_MAX, BRIGHTNESS_MIN));
				laser.setRate( mapFloat(a*a, CLOSE_MM_SQUARED, FAR_MM_SQUARED, BREATH_MAX, BREATH_MIN));
			}
			 
			break;
		}
		
		case LAST_GASP_WAIT: {
			
			//laser.setBrightness( map(a*a, CLOSE_MM_SQUARED, FAR_MM_SQUARED, BRIGHTNESS_MAX, BRIGHTNESS_MIN));
			//laser.setRate( mapFloat(a*a, CLOSE_MM_SQUARED, FAR_MM_SQUARED, BREATH_MAX, BREATH_MIN));
			
			
			//wait for it's final inhale, for slow exhale, then glitch off
			if ( laser.breathLevel == 0 ) {
				//do it!
				Serial.println("--");
				setState(LAST_GASP,(char *)"LAST_GASP");
			}
			
			break;
		}
		
		case LAST_GASP: {
			//laser.setBrightness( map(a*a, CLOSE_MM_SQUARED, FAR_MM_SQUARED, BRIGHTNESS_MAX, BRIGHTNESS_MIN));
			//laser.setRate( 1000./ LAST_GASP_LENGTH );
			laser.set(Laser::FADE_OFF, ms, LAST_GASP_LENGTH, 0, 255);
			
			
			dispatch.setCallback(stateDispatch, LAST_GASP_LENGTH, ms, GLITCH_OFF);
			setState(RUN,(char *)"RUN");
			break;
		}
		
		
		
		case GLITCH_OFF: {
			//normal running, get distance and change rate/brightness
			int glitchLengh=random(1600,2200);
			//laser.setBrightness( GLITCH_BRIGHTNESS );
			
			laser.set(Laser::GLITCH_OFF, ms, glitchLengh);
			dispatch.setCallback(stateDispatch,glitchLengh+100,ms,RESET);
			setState(RUN,(char *)"RUN");
			break;
			
		}
		
		
		case RUN: {
			//Serial.println("RUN");
			break;
		}
		
	}	 
	 
	 
	laser.update(ms);
	

	//if ( sensor.ranging_data.range_mm < 150 ) {
		 
	//	if (laser.isOn() ) {
			//Serial.println("*");
	//		laser.set(Laser::GLITCH_OFF, ms, 4000);
				
	//	} else {
	//		laser.set(Laser::GLITCH_ON, ms, 2000);
	//	}
	//}
	
	
	
	//unsigned long a=avg.getFastAverage();
	//unsigned long a2=a*a;
	 
	//analogWrite(LASER_PIN, map(a2, CLOSE_MM_SQUARED, FAR_MM_SQUARED, 255, 0));
  	
  
	/*
	Serial.print("range: ");
	Serial.print(sensor.ranging_data.range_mm);
	Serial.print("\tstatus: ");
	Serial.print(VL53L1X::rangeStatusToString(sensor.ranging_data.range_status));
	Serial.print("\tpeak signal: ");
	Serial.print(sensor.ranging_data.peak_signal_count_rate_MCPS);
	Serial.print("\tambient: ");
	Serial.print(sensor.ranging_data.ambient_count_rate_MCPS);

	Serial.println();*/
}