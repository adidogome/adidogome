#ifndef Touch_h
#define Touch_h


#include "Arduino.h"
#include "CapacitiveSensor.h"

#define detectmode 1 //damping detection


class Touch { 
  
  private:
  
    int sendpin, receivepin[5]; 
    CapacitiveSensor row0 = CapacitiveSensor(sendpin, receivepin[0]);
    CapacitiveSensor row1 = CapacitiveSensor(sendpin, receivepin[1]);
    CapacitiveSensor row2 = CapacitiveSensor(sendpin, receivepin[2]);
    CapacitiveSensor row3 = CapacitiveSensor(sendpin, receivepin[3]);
    CapacitiveSensor row4 = CapacitiveSensor(sendpin, receivepin[4]);

    int type, tempdetectedinput;
 
    int detectionState = 0;
    int tempdiff = 0;
    int testsignalsize = 30;
    int samplecounter = 0, humpsamplecounter = 0;
    unsigned int floatclickspeed, speed_noise = 3, speed_normal = 3;
    unsigned int singletapthresh, shortpressthresh, longpressthresh;
	int haptics_pin = 0, haptics_duration = 0, haptics_strength = 0, haptics_ontime, haptics_offtime, haptics_state;
    
    unsigned long spikereleasetime = 1000, spikedetecttime = 0;
    unsigned long debounce = 5;
    unsigned long time_touched = 0, master_time_touched = 0;

    bool touchSpikeDetected = false;
    bool interruptValue_aquisition = false;
    bool returnTouchType = false;
    bool floatclick, adaptsensitivity = false;
    bool longpresshaptics = false, shortpresshaptics = false, extralongpresshaptics = false;
    bool repopulating = false;
    bool startWith_interrupt = true;

    float minsense = 5, maxsense = 100;
    float maxtouchrange = 100;
    float threshBase = 0;
    float prevsignalsample = 0, prevhumpsample = 0;
    float sampleValues[30];
    float sensitivity = 1;  
    float detectionThreshold[5] = { 0.1,0.1,0.1,0.1,0.1 };
    float rejectionThreshold[5] = {0,0,0,0,0};
    float changeValue[5] = { 0,0,0,0,0 }, prevChangeValue[5] = { 0,0,0,0,0 }, baseValue[5] = { 0,0,0,0,0 }, maxSample[5] = {99999,99999,99999,99999,99999}, minSample[5];  
    float tempread;
    float S_alpha = 0.4, S_alpha2 = 0.001, smallvaluerange = 15, diff = 0;
    
  

  public:
    Touch();
    Touch(bool useint);

    void initializeTouch(int totalRows);
    void set_inputTypeThresholds(int stp, int spr, int lpr, int dcls);
	void set_touchThresholds(int rowNum, bool act);
    void setSensitivity(int rowNum, float mastersensitivity, bool act);
    void setSensitivityRange(int totalRows, float min, float max);
    void setPins(int sp, int r0, int r1, int r2, int r3, int r4);

    void update_basevalueFromNoise(int rowNum);
    void update_basevalueSmooth(int rowNum);

    void resetvalues();

    void set_haptics(int pin, int duration, int strength);
    void haptics(int state);

    void addSample(int rowNum, float value);

    bool checkTouch(int rowNum); 
    bool checkRelease(int rowNum);
	bool repopulateSample(int rowNum);

    float read_valueFromNoise(int rowNum);
    float read_value(int rowNum);
      
    int detect_touchFromNoise(int rowNum);
              

};

#endif
