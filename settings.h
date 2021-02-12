//settings:
#define FAR_MM				1850	//1' = 305mm
#define CLOSEST_MM			305

#define FAR_MM_SQUARED		3422500	//mm^2 where laser is off
#define CLOSE_MM_SQUARED 	93025 	//mm^2 where laser is full on

#define GLITCH_BRIGHTNESS	100		//0-255
#define BRIGHTNESS_MAX		255		//brightness when you're up in its business
#define BRIGHTNESS_MIN		20		//brightness when you're 6 feet away

#define BREATH_MAX			0.4		//rate when you're on it
#define BREATH_MIN			1.0		//how fast the breathing is when you're 6 feet away

#define MIN_LURING_MS		6000	//always do this amount even if the person steps out

#define LAST_GASP_LENGTH	1200	//milliseconds of its post-glitch fade up

#define YAWN_LENGTH			4200	//milliseconds of its post-glitch fade up

#define AVG 				10		//rolling average of last x readings (helps smooth transition)
#define LASER_PIN 			11		//what pin is the laser ttl on? //11
