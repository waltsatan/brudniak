#include "Laser.h"

extern float waves[6][120];

Laser::Laser(uint8_t p) {
	_state=0;
	_pin=p;
	pinMode(_pin, OUTPUT);
	
	_blinkState=Blink_State::BLINK_OFF;
	set(Laser::TURN_OFF);
	
	
	
}

void Laser::PWM(uint8_t v) {
	_state=v;
	analogWrite(_pin,v*255);
	
}
void Laser::off() {
	set(Laser::TURN_OFF);

}

void Laser::on() {
	set(Laser::TURN_ON);

}


void Laser::set(byte effect, unsigned long ms, uint16_t v1, uint16_t v2) {
	PWM(1);
	_v1=v1;
	_v2=v2;
	 
	_effect=effect;
	_ms=ms;
	update(ms);
}

bool Laser::isOn() {
	if (_effect != Laser::TURN_OFF && _effect != Laser::NOTHING) {
		return(true);
	} else {
		return(false);
	}
	
}

byte Laser::get() {
	return(_effect);

}
void Laser::set(byte effect, unsigned long ms, uint16_t v1, uint16_t v2, uint16_t v3) {
	PWM(1);
	_lastMillis=ms; 
	_v1=v1;
	_v2=v2;
	_v3=v3;
	_effect=effect;
	_ms=ms;
	_perc=0;
	//setRate(1);
	
	update(ms);
}

void Laser::set(byte effect) {
	_effect=effect;
	_ms=0;
	_perc=0;
	_lastMillis=_ms;
	update(0);
	
}

void Laser::endBlink() {
	switch (_effect) {
		case Laser::BLINK_X_TIMES_ON: {
			_effect=Laser::TURN_ON;	
			break;
		}
		case Laser::BLINK_X_TIMES: {	
			_effect=Laser::TURN_OFF;
			break;
		}
	}
}

bool Laser::setRate(float r) {
	bool changed=false;
	if ( _rate != r ) changed=true;
	_rate=r;
	return(changed);
//	rate=1;
}


void Laser::update(unsigned long ms) {
	if ( _ms > ms ) _ms = ms;
	float elapsed=(ms-_lastMillis)/1000.;
	
	switch (_effect) {
		case Laser::TURN_OFF: {
			PWM(0);
			break;
		}
		case Laser::TURN_ON: {
			PWM(1);
			break;
		}
		
		case Laser::FADE_ON: {
			unsigned long millisSinceStart=max(1,(ms-_ms));
			
			if ( millisSinceStart <= _v1 ) {
				
				float perc=(millisSinceStart%_ms)/(float)_v1;
				analogWrite(_pin,perc*255);
				
			} else {
				on();
			}
			
			break;
			
		
		}
		
		
		case Laser::BREATH:{

			if ( _v3 == 1 ) {
				_perc=(_perc + (_rate*elapsed));
				//_perc=_perc/1.5;
				 
			} else {
				_perc=(_perc - ((_rate*1.6)*elapsed));
				//_perc=_perc*1;
			}
	 
			if ( _perc > 1 ) {
				_v3 = 0;
				_perc = 1 ;
			} else if ( _perc < 0 ) {
				_v3 = 1;
				_perc=0;
			} 
	
			//			=((ms-_ms)%_ms)/(float)_ms;
				
		//	float sinp=sin(_perc/1.5*M_PI_2);
			float sinp=sin((_perc/2)*M_PI_2);
				
			 
			analogWrite(_pin,(1-sinp)*255);
		
			break;
		
		
		}
		case Laser::GLITCH_ON: {
			unsigned long millisSinceStart=max(1,(ms-_ms));
			if ( millisSinceStart <= _v1 ) {
				
				if ( _blinkState==BLINK_OFF ) {
					if ( ms-_v2 > random(60,_v1-millisSinceStart)) {
						float perc=millisSinceStart/(float)_v1;
						float r=random(0,__INT_MAX__)/(float)__INT_MAX__;

						if ( perc > r ) {
							PWM(1);
							_blinkState=BLINK_ON;
						}
						_v2=ms;
					}
				} else {
					//on, should we turn off yet?
					if ( ms-_v2 > random(30,70)) {
						PWM(0);
						_blinkState=BLINK_OFF;
						_v2=ms;
					}
				}
				
				
				//Serial.println();
				/*if ( _blinkState==BLINK_OFF ) {
					
					
					//Serial.println(ms-_v2);
					if ( ms-_v2 > 100) {
						PWM(0);
						 _blinkState=BLINK_ON;
						 //_v2=ms;
					}
					
					
						
				} else {
				
					float r=(random(0,(_v1-millisSinceStart)))/(float)_v1;
					Serial.println(_v1-millisSinceStart);
					if ( r > 0.67 ) {
						PWM(1);
						_blinkState=BLINK_OFF;	
						_v2=ms;
					}
					
				}
				
				*/
			 
				
						
			} else {
				on();
			}
			
				 
				/*if ( _blinkState==BLINK_OFF ) {
				
					if ( (ms-_v2 ) > (100+random(0,100) )) {
						PWM(0);
						_blinkState=BLINK_ON;
						_v2=ms;
					}
				} else {
					
					
					float r=(random(0,(_v1-millisSinceStart)))/(float)_v2;
					Serial.println(r);
					//Serial.println(millisSinceStart);
					if ( r > 0.5) {
						PWM(1);
						Serial.print("*");
						Serial.println(r);
						_blinkState=BLINK_OFF;
						_v2=ms;
					}
					*/
					
					/*float r=rand()/(float)__INT_MAX__;
					if ( (ms-_v2 ) > r * (_ms/2) )  {
						PWM(1);
						_blinkState=BLINK_OFF;
						_v2=ms;
					}*/
					
					
					// OUT
					/*if ( (ms-_v2 ) > r * (ms-_ms) )  {
						PWM(1);
						_blinkState=BLINK_OFF;
						_v2=ms;
					}*/
					 
				
		
					/*float r=rand()/(float)__INT_MAX__;
					Serial.print((ms-_v2));
					Serial.print(",");
					Serial.println(r*millisSinceStart*2);

					if (  (ms-_v2) < (r*millisSinceStart*2)   ) {//(ms-_v2) > r*(millisSinceStart/100.) )  {
						Serial.println("*"); 
						PWM(1);
						_blinkState=BLINK_OFF;
						_v2=ms;
					}*/
				//}
				
			//} else {
			//	on();
			//}
					
			break;
		}
		case Laser::GLITCH_OFF: {
 
			uint16_t millisSinceStart=max(1,(ms-_ms));
			
			
			if ( millisSinceStart <= _v1 ) {
				
				if ( _blinkState==BLINK_OFF ) {
				
					if ( (ms-_v2 ) > (50+random(0,50) )) {
						PWM(0);
						_blinkState=BLINK_ON;
						_v2=ms;
					}
				} else {
					float r=rand()/(float)__INT_MAX__;
					
					if ( (ms-_v2 ) > r * (ms-_ms) )  {
						PWM(1);
						_blinkState=BLINK_OFF;
						_v2=ms;
					}
					 
				 
				}
			} else {
				
				off();
			}				
			
			break;
			
		}
			
		case Laser::BLINK: {
			if ( _state == 1 ) { //is on
				if ( _ms + _v1 < ms ) {
					PWM(0);
					_ms=ms;
				}		
			} else { //is off
				if ( _ms + _v2 < ms ) {
					PWM(1);
					_ms=ms;
				}	
			}
			break;
		}
		case Laser::BLINK_X_TIMES_ON:
		case Laser::BLINK_X_TIMES: {
			if ( _state == 1 ) { //is on
				if ( _ms + _v2 < ms ) {
					PWM(0);
					_ms=ms;
					
					_v1--;
					
					if ( _v1 == 0 && _effect == Laser::BLINK_X_TIMES) { //all done;
						_effect=Laser::TURN_OFF;
					}
				}		
			} else { //is off
				if ( _ms + _v3 < ms ) {
					PWM(1);
					_ms=ms;
					if ( _v1 == 1 && _effect == Laser::BLINK_X_TIMES_ON ) {
						_effect=Laser::TURN_ON;	
					}
				}	
			}
			break;
		}
	}
	_lastMillis=ms;
}

void Laser::updateBlinkSpeed(unsigned long s) {
	_v1=s;
	_v2=s;
}
bool Laser::getState() {
	return(_state);
}
