#ifndef Dispatch_h
#define Dispatch_h
 
class Dispatch  {
	
	public:
		Dispatch();
		 
		void update(unsigned long);
		void setCallback(void (*callback)(unsigned long,int), unsigned long, unsigned long, int);
		
		float percentComplete;
		
		bool isRunning;
		
		int getDispatchMillis();
		void reset();
		void abort();
		
	private:
		unsigned long _startMillis;
		unsigned long _dispatchInMillis;
		int _value;
		void (*_callback)(unsigned long,int);	
		bool _hasCallback;
		
};

#endif