#ifndef PID_v1_h
#define PID_v1_h
#define LIBRARY_VERSION	1.0.0

class PID
{


  public:

  //Constants used in some of the functions below
  #define AUTOMATIC	1
  #define MANUAL	0
  #define DIRECT  0
  #define REVERSE  1

  //commonly used functions **************************************************************************
    PID(short*, short*, short*,        // * constructor.  links the PID to the Input, Output, and 
        short, short, short, short);        //   Setpoint.  Initial tuning parameters are also set here
	
    void SetMode(short Mode);               // * sets PID to either Manual (0) or Auto (non-0)

    bool Compute();                       // * performs the PID calculation.  it should be
                                          //   called every time loop() cycles. ON/OFF and
                                          //   calculation frequency can be set using SetMode
                                          //   SetSampleTime respectively

    void SetOutputLimits(short, short); //clamps the output to a specific range. 0-255 by default, but
										  //it's likely the user will want to change this depending on
										  //the application
	


  //available but not commonly used functions ********************************************************
    void SetTunings(short, short,       // * While most users will set the tunings once in the 
                    short);         	  //   constructor, this function gives the user the option
                                          //   of changing tunings during runtime for Adaptive control
	void SetControllerDirection(short);	  // * Sets the Direction, or "Action" of the controller. DIRECT
										  //   means the output will increase when error is positive. REVERSE
										  //   means the opposite.  it's very unlikely that this will be needed
										  //   once it is set in the constructor.
    void SetSampleTime(short);              // * sets the frequency, in Milliseconds, with which 
                                          //   the PID calculation is performed.  default is 100
										  
										  
										  
  //Display functions ****************************************************************
	short GetKp();						  // These functions query the pid for interal values.
	short GetKi();						  //  they were created mainly for the pid front-end,
	short GetKd();						  // where it's important to know what is actually 
	short GetMode();						  //  inside the PID.
	short GetDirection();					  //

  private:
	void Initialize();
	
	short dispKp;				// * we'll hold on to the tuning parameters in user-entered 
	short dispKi;				//   format for display purposes
	short dispKd;				//
    
	short kp;                  // * (P)roportional Tuning Parameter
    short ki;                  // * (I)ntegral Tuning Parameter
    short kd;                  // * (D)erivative Tuning Parameter

	short controllerDirection;

    short *myInput;              // * Pointers to the Input, Output, and Setpoint variables
    short *myOutput;             //   This creates a hard link between the variables and the 
    short *mySetpoint;           //   PID, freeing the user from having to constantly tell us
                                  //   what these values are.  with pointers we'll just know.
			  
	unsigned long lastTime;
	short ITerm, lastInput;

	unsigned long SampleTime;
	short outMin, outMax;
	bool inAuto;
};
#endif

