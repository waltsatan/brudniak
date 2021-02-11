#include "Dispatch.h"

Dispatch::Dispatch()  {
	 isRunning=false;
	 
}

void Dispatch::update(unsigned long millis) {
	
	if ( isRunning ) {
	
		if ( _startMillis > millis ) _startMillis = millis;
	
	
		percentComplete=(millis-_startMillis)/(float)_dispatchInMillis;
	
		if ( _hasCallback && millis-_startMillis > _dispatchInMillis) {
			_hasCallback=false;
			isRunning=false;
			_callback(_dispatchInMillis,_value);
		
		}
	}
}

void Dispatch::abort() {
	_hasCallback=false;
	isRunning=false;
}


void Dispatch::setCallback(void (*callback)(unsigned long,int), unsigned long millis, unsigned long now, int value) {
	_startMillis=now;
	_dispatchInMillis=millis;
	_value=value;
	_callback = callback;
	_hasCallback=true;
	isRunning=true;
}

/*float Dispatch::getPercentComplete(unsigned long now) {
	if ( _hasCallback ) {
		if ( now > _startMillis + _dispatchInMillis ) return(1);
		
		return( (_startMillis + _dispatchInMillis)/(float)now);
		 
	
	} else {
		return(0);
	}

}*/