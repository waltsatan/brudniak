#ifndef LASER_h
#define LASER_h

#include "Arduino.h"


class Laser {
 
	public:
		Laser(uint8_t);
		
		static enum Laser_state {
			OFF,
			ON
		} Laser_state; 
		
		static enum Laser_effect {
			NOTHING,
			TURN_OFF,
			TURN_ON,
			FADE_ON,
			BLINK,
			BLINK_X_TIMES,
			BLINK_X_TIMES_ON,
			GLITCH_OFF,
			GLITCH_ON,
			BREATH
		} Laser_effect; 
		
		typedef enum {
			BLINK_ON,
			BLINK_OFF
		} Blink_State; 
		
		void updateBlinkSpeed(unsigned long);
		
		void off();
		void on();
		bool isOn();
		
		void endBlink();
		
		void set(byte, unsigned long, uint16_t, uint16_t, uint16_t);
		void set(byte, unsigned long, uint16_t, uint16_t=0);
		void set(byte);
		void update(unsigned long);
		bool getState();
		void PWM(uint8_t);
		byte get();
		bool setRate(float);
	private:
		unsigned long _lastMillis;
		uint8_t _pin;
		unsigned long _ms;
		byte _effect;
		float _perc;
		unsigned long _v1;
		unsigned long _v2;
		unsigned long _v3;
		float _rate;
		
		bool _state;
		Blink_State _blinkState;
		
};


#endif