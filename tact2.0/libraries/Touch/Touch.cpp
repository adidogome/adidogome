# include "Touch.h"



Touch::Touch(){
}

Touch::Touch(bool useint) {

	interruptValue_aquisition = useint;
}

void Touch::initializeTouch(int totalRows) {
 
  setSensitivity(5, 0.09, true);  //set touch sensitivity to adaptive (very helpful for noisy signals)--- (number of rows, sensitivity(0 to 1), turn-on/off(true or false))
  set_inputTypeThresholds(300, 1000, 2000, 250); // set the thresholds for the four input types  (singletap, shortpress, longpress, floattapspeed)
  

  // row0.set_CS_AutocaL_Millis(0x0FFFFFFFL);
  // row1.set_CS_AutocaL_Millis(0x0FFFFFFFL);
  // row2.set_CS_AutocaL_Millis(0x0FFFFFFFL);
  // row3.set_CS_AutocaL_Millis(0x0FFFFFFFL);
  // row4.set_CS_AutocaL_Millis(0x0FFFFFFFL);

  pinMode(sendpin, OUTPUT);

  for (int i = 0; i < totalRows; i++){
	pinMode(receivepin[i], INPUT);
  }
};


void Touch::set_inputTypeThresholds(int stp, int spr, int lpr, int dcls) {  //sets the thresholds for the four input types  (singletap, shortpress, longpress, floattapspeed)
  singletapthresh = stp;
  shortpressthresh = spr;
  longpressthresh = lpr;
  floatclickspeed = dcls;
};

void Touch::set_touchThresholds(int rowNum, bool act) {
	//read_valueFromNoise(rowNum);
	detectionThreshold[rowNum] = (maxSample[rowNum] - minSample[rowNum]);
	rejectionThreshold[rowNum] = detectionThreshold[rowNum];
	adaptsensitivity = act;
};

void Touch::setSensitivity(int rowNum, float mastersensitivity, bool act) {
  sensitivity = mastersensitivity;
  adaptsensitivity = act;      
};  


void Touch::setSensitivityRange(int totalRows, float min, float max){
  minsense = min;
  maxsense = max;
};


bool Touch::checkTouch(int rowNum) {  //check for valid capacitive touch

  if (baseValue[rowNum] == 0)
  {
	return false;
  }

  if (detectionState == 0) {
	if (changeValue[rowNum] > (baseValue[rowNum] + (sensitivity * detectionThreshold[rowNum]))) {
	  if (touchSpikeDetected){
		detectionState = 0;
		type = 0;
		returnTouchType = true;
		return false;
	  }
	  else {
		prevChangeValue[rowNum] += detectionThreshold[rowNum];
		return true;
	  }
	}
	else {
	  return false;
	}
  }
  else {
	if (changeValue[rowNum] > (threshBase + (sensitivity * detectionThreshold[rowNum]))) {
	  if (touchSpikeDetected){
		detectionState = 0;
		type = 0;
		returnTouchType = true;
		return false;
	  }
	  else {
		return true;
	  }
	}
	else  {
	  return false;
	}	
  }

};


bool Touch::checkRelease(int rowNum) {  //checks for valid capacitive touch

  if (touchSpikeDetected){
	detectionState = 0;
	type = 0;
	returnTouchType = true;
	return false;
  }
  else {
	if (changeValue[rowNum] < (threshBase - (sensitivity * detectionThreshold[rowNum]))) {
	  return true;
	}
	else {
	  return false;
	}
  }

};


//bool Touch::repopulateSample(int rowNum){
  // for (int i = 0; i < testsignalsize; i++){
	//sampleValues[i] = 9999999;
	 //repopulating = true;
  // }
//};


void Touch::addSample(int rowNum, float value){
  if (touchSpikeDetected){
	if ((millis() - spikedetecttime) > spikereleasetime) {
	touchSpikeDetected = false;
	}
  }

  if (samplecounter < testsignalsize) {
	prevsignalsample = sampleValues[samplecounter];
	sampleValues[samplecounter] = value;

	samplecounter += 1;
  }
  if (samplecounter >= testsignalsize) {
	samplecounter = 0;
  }

};


void Touch::setPins(int sp, int r0, int r1, int r2, int r3, int r4) {
  sendpin = sp;
  receivepin[0] = r0;
  receivepin[1] = r1;
  receivepin[2] = r2;
  receivepin[3] = r3;
  receivepin[4] = r4;
  row0 = CapacitiveSensor(sp, r0);
  row1 = CapacitiveSensor(sp, r1);
  row2 = CapacitiveSensor(sp, r2);
  row3 = CapacitiveSensor(sp, r3);
  row4 = CapacitiveSensor(sp, r4);

};   


int Touch::detect_touchFromNoise(int rowNum) {  //touch type detection function
  if (detectionState == -10){
	read_valueFromNoise(rowNum);
	if (debounce < (millis() - time_touched)){
	  if (checkTouch(rowNum) == true){
		//haptics(1);   //turn on viberation motor to acknowledge touch
		type = 1;   //set input type as single tap but proceed to check for second touch
		detectionState = 1;
		threshBase = baseValue[rowNum];
		time_touched = millis();
		startWith_interrupt = false;
	  }
	}
  }
  if (detectionState == 0) {
	read_valueFromNoise(rowNum);
	if (checkTouch(rowNum) == true) {
	  detectionState = -10;
	  time_touched = millis();
	  master_time_touched = time_touched;
	}
  }
  else if (detectionState == 1) {
	read_valueFromNoise(rowNum);
	if (checkTouch(rowNum) == true) {   //check again

	  long temptime = (millis() - time_touched);
	  if (temptime < 4000) {
		haptics(0);
	  }
	  if (temptime > shortpressthresh && temptime < longpressthresh) {  //shortpress
		type = 3;
		if (!shortpresshaptics) {
		  haptics(1); //turn on viberation to indicate long pressthresh
		  shortpresshaptics = true;
		}
	  }
	  if (temptime > longpressthresh && temptime < 4000) {
		type = 4;
		if (!longpresshaptics) {
		  haptics(1); //turn on viberation to indicate long pressthresh
		  longpresshaptics = true;
		}
		//returnTouchType = true;
	  }
	  if (temptime >= 4000) {  //extra long touch input for special operations
		type = 5;
		if (!extralongpresshaptics) {
		  haptics(1); //turn on viberation to indicate long pressthresh
		  extralongpresshaptics = true;
		}
		if (checkRelease(rowNum)) {

		  returnTouchType = true;
		  haptics(0);
		}
		else {
		  if (temptime >= 5000) {
			type = 0;
			returnTouchType = true;
		  }
		}
	  }
	}
	else { //detected release
	  if ((millis() - time_touched) > floatclickspeed) {
		returnTouchType = true;
	  }
	  //Serial.println((millis() - time_touched));
	  detectionState = 2;
	  //changeValue[rowNum] = minSample[rowNum];
	}
  }

  else if (detectionState == 2) {
	if (type > 2) {  //already set to shortpress or longpress
	  returnTouchType = true;
	}
	else if (type == 2)//wait for release
	{
	  //haptics(0);
	  read_valueFromNoise(rowNum);
	  if (checkRelease(rowNum))
	  {
		returnTouchType = true;
	  }
	}
	else {
	  //haptics(0);
	  read_valueFromNoise(rowNum);
	  if (checkTouch(rowNum) == true) {
		  read_valueFromNoise(rowNum);
		if (checkTouch(rowNum) == true) {   //check again
		  if ((millis() - time_touched) > floatclickspeed) {
			haptics(1);
			type = 2;
			
		  }
		}
	  }
	  else {
		if ((millis() - time_touched) > singletapthresh) { //float tap timeout
		  returnTouchType = true;
		}
	  }
	}
  }

  if (returnTouchType == true) {
	longpresshaptics = false;
	shortpresshaptics = false;
	extralongpresshaptics = false;
	//samplecounter = testsignalsize;
	//changeValue[rowNum] = minSample[rowNum];  // avoid repititive detection
	detectionThreshold[rowNum] = 999999;
	returnTouchType = false;
	detectionState = 0;
	tempdetectedinput = type;
	samplecounter = testsignalsize - 1;
	
	update_basevalueFromNoise(rowNum);
	Serial.println((millis() - time_touched));
	startWith_interrupt = true;
	
	return tempdetectedinput;
  }
  else {
	if (detectionState != 0)
	{
	  if (detectionState == -10 && (millis() - master_time_touched) > singletapthresh){
		detectionState = 0;
		type = 0;
		master_time_touched = millis();
	  }
	  else if (type < 3) { //detection has started
		update_basevalueFromNoise(rowNum);
		return -1;
	  }
	}
	update_basevalueFromNoise(rowNum);
	return 0;
  }

};


float Touch::read_value(int rowNum){
  if (rowNum == 0) {
	changeValue[rowNum] = row0.capacitiveSensor(speed_normal);
  }
  else if (rowNum == 1) {
	changeValue[rowNum] = row1.capacitiveSensor(speed_normal);
  }
  else if (rowNum == 2) {
	changeValue[rowNum] = row2.capacitiveSensor(speed_normal);
  }
  else if (rowNum == 3) {
	changeValue[rowNum] = row3.capacitiveSensor(speed_normal);
  }
  return changeValue[rowNum];
  
//   float changeValue[rowNum];      

//   for(int i = 0; i < rowNum; i++){
//     if(i == rowNum){
//       changeValue[i] = 0;
//     }
//     else{
//       CapacitiveSensor cs = CapacitiveSensor(sendpin, receivepin[speed_normal]);
//       long sensorValue = cs.capacitiveSensor(10);
//       changeValue[i] = sensorValue;
//     }        
//   }
//   float sum = 0;
//   for(int i = 0; i < rowNum; i++){
//     sum += changeValue[i];
//   }
//   return sum;
};    


float Touch::read_valueFromNoise(int rowNum) {   //returns only peak value from a small sample of the signal
  if (!interruptValue_aquisition || !startWith_interrupt){
	if (rowNum == 0) {
	  tempread = row0.capacitiveSensor(3);
	  //set  the changeValue(damping drastic changes)
	  diff = prevChangeValue[rowNum] - tempread;
	  if (diff < 0){
		diff *= -1;
	  }
	  if (diff < smallvaluerange){
		changeValue[rowNum] = (tempread * S_alpha2) + (prevChangeValue[rowNum] * (1 - S_alpha2));
		prevChangeValue[rowNum] = changeValue[rowNum];
	  }
	  else {
		changeValue[rowNum] = (tempread * S_alpha) + (prevChangeValue[rowNum] * (1 - S_alpha));
		prevChangeValue[rowNum] = changeValue[rowNum];
	  }
	  if (tempread < (baseValue[rowNum] + maxtouchrange)){
		addSample(rowNum, tempread);
	  }
	  else {
		addSample(rowNum, tempread);
		if (!touchSpikeDetected){
		  touchSpikeDetected = true;
		  spikedetecttime = millis();
		}
	  }
	}
  }
  return changeValue[rowNum];
};


void Touch::update_basevalueFromNoise(int rowNum) {  // keeps both the baseValue and the touch thresholds updated
  if (detectionState == 0) {
	haptics(0);
	update_basevalueSmooth(rowNum);
	resetvalues();
  }
};


void Touch::update_basevalueSmooth(int rowNum) {   //Function to gradually update baseValue
  if (samplecounter == (testsignalsize - 1)) { //find new max and min values
	maxSample[rowNum] = 0;
	minSample[rowNum] = 9999999;
	for (int i = 0; i < testsignalsize; i++) {
	  if (sampleValues[i] != 0) {  //ignore remaining samples if sample collection is terminated early
		if (sampleValues[i] > maxSample[rowNum]) {
		  maxSample[rowNum] = sampleValues[i];
		}
		if (sampleValues[i] < minSample[rowNum]) {
		  minSample[rowNum] = sampleValues[i];
		}

	  }

	}
	//update touch thresholds (adaptive touch sensitivity)
	if (adaptsensitivity == true) {
	  detectionThreshold[rowNum] = (maxSample[rowNum] - minSample[rowNum]);
	  rejectionThreshold[rowNum] = detectionThreshold[rowNum];
	}
	baseValue[rowNum] = maxSample[rowNum];
  }
};


void Touch::resetvalues() {
  if (detectionState == 0) {
	type = 0;
  }

}


void Touch::set_haptics(int pin, int duration, int strength) {  //use to set haptics variables (arduino pwm pin, duration of haptics(ms), strength from 0-255)
  haptics_pin = pin;
  haptics_duration = duration;
  haptics_strength = strength;
}


void Touch::haptics(int state) {  //vibration feedback
  if (state == 1) {
	analogWrite(haptics_pin, haptics_strength);
	haptics_ontime = millis();
	haptics_state = 1;
  }
  else if (state == 3) {
	analogWrite(haptics_pin, (haptics_strength * 0.7));
  }
  else {
	haptics_offtime = millis();
	if (((haptics_offtime - haptics_ontime) >= haptics_duration) && haptics_state == 1) {
	  analogWrite(haptics_pin, 0);
	  haptics_state = 0;
	}

  }
};
