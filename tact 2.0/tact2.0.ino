/*
Pressure Sensing & Logging Matrix Code
parses through a pressure sensor matri grid by switching individual 
rows/columns to be HIGH, LOW, or INPUT (high impedance) to detect 
location and pressure. Logs each sensor value to .csv file by 
microseconds and indicates 'touch' when sensor value < threshold
modified from code found at: 
>> https://www.kobakant.at/DIY/?p=7443
and dataLogger example from SdFat library by Bill Greimen
*/

#include <SPI.h>
#include <SdFat.h>

#define numRows 16
#define numCols 1
#define sensorPoints numRows*numCols

int rows[] = {A15, A14, A13, A12, A11, A10, A9, A8, A7, A6, A5, A4, A3, A2, A1, A0}; 
//A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15
//A15, A14, A13, A12, A11, A10, A9, A8, A7, A6, A5, A4, A3, A2, A1, A0
int cols[] = {1};
int incomingValues[sensorPoints] = {};


unsigned long then = 0;
unsigned long now = 0;
unsigned long between = 0;

const uint8_t chipSelect = 10;
//10 for at mega 2560
// 10 for elegoo 2560

// Interval between data records in milliseconds.
// The interval must be greater than the maximum SD write latency plus the
// time to acquire and write data to the SD to avoid overrun errors.
const uint32_t SAMPLE_INTERVAL_MS = 150;

// Log file base name.  Must be six characters or less.
#define FILE_BASE_NAME "Right"

SdFat sd; // file system object 
SdFile file; //log file 

uint32_t logTime; // time in micros for next data record. 
const uint8_t ANALOG_COUNT = numRows;

//------------------------------------------------------------------------------
// Write data header.
void writeHeader() {
  file.print(F("micros"));
  for (uint8_t i = 0; i < ANALOG_COUNT; i++) {
    file.print(F(",adc"));
    file.print(i, DEC);
  }
  file.println();
}
//------------------------------------------------------------------------------
// Log a data record.
void logData() {

  
  uint16_t data[ANALOG_COUNT];

  // for (int i = 0; i < numRows; i++) { 
  //     pinMode(rows[i], INPUT_PULLUP);  
  //   }

  // for (int i = 0; i < numCols; i++) { 
  //   pinMode(cols[i], INPUT); 
  // }

  // Read all channels to avoid SD write latency between readings.
  for (uint8_t i = 0; i < ANALOG_COUNT; i++) {
    data[i] = analogRead(i);
  }

  // // Write data to file.  Start with log time in micros.
  file.print(logTime);

  // Write ADC data to CSV record.
  for (uint8_t i = 0; i < ANALOG_COUNT; i++) {
    file.write(',');
    file.print(data[i]);
    if (incomingValues[i] < 1005){
    file.print(" touch ");
    }
  }
  file.println();
  }




//==============================================================================
// Error messages stored in flash.
#define error(msg) sd.errorHalt(F(msg))
//==============================================================================


void setup() {

  const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
  char left[13] = FILE_BASE_NAME "00.csv";

  // set all rows and columns to INPUT (high impedance):
  for (int i = 0; i < numRows; i++) { // iterates over each row of the pressure sensor; i starts at 0 and goes up to numRows-1
    pinMode(rows[i], INPUT_PULLUP); //calls current row; INPUT_PULLUP configures the pin as an input and enables internal pull-up resistor. 
    //When no external device connected, pin is at level of microcontroller (reads HIGH by default, unless external device pulls LOW)
  }

  for (int i = 0; i < numCols; i++) { //iterates over each column of the pressure sensor; i starts at 0 and goes up to numCols-1
    pinMode(cols[i], INPUT); //column pin as input 
  }
  Serial.begin(9600);

  if (!sd.begin(chipSelect, SPI_FULL_SPEED)){
    sd.initErrorHalt();
  }

  // Find an unused file name.
  if (BASE_NAME_SIZE > 6) {
    error("FILE_BASE_NAME too long");
  }
  while (sd.exists(left)) {
    if (left[BASE_NAME_SIZE + 1] != '9') {
      left[BASE_NAME_SIZE + 1]++;
    } else if (left[BASE_NAME_SIZE] != '9') {
      left[BASE_NAME_SIZE + 1] = '0';
      left[BASE_NAME_SIZE]++;
    } else {
      error("Can't create file name");
    }
  }
  if (!file.open(left, O_WRONLY | O_CREAT | O_EXCL)) {
    error("file.open");
  }

  // Read any Serial data.
  do {
    delay(10);
  } while (Serial.available() && Serial.read() >= 0);

  Serial.print(F("Logging to: "));
  Serial.println(left);
  Serial.println(F("Type any character to stop"));

  // Write data header.
  writeHeader();

  // Start on a multiple of the sample interval.
  logTime = micros()/(1000UL*SAMPLE_INTERVAL_MS) + 1;
  logTime *= 1000UL*SAMPLE_INTERVAL_MS;
  
}

  

void loop() {
  for (int colCount = 0; colCount < numCols; colCount++) { 
    pinMode(cols[colCount], OUTPUT); 
    digitalWrite(cols[colCount], LOW); 
  
  for (int rowCount = 0; rowCount < numRows; rowCount++) {
    incomingValues[colCount * numRows + rowCount] = analogRead(rows[rowCount]); 
  }// end rowCount

  pinMode(cols[colCount], INPUT); // set back to INPUT! 
  // end colCount

  for (int i = 0; i < sensorPoints; i++) { 
      if (incomingValues[i] < 1005){ //touch
        now = millis();
        between = now -then;
        Serial.print(i);
        Serial.print(" touched; last touch: ");
        Serial.print(between);
        Serial.print("\t");
        Serial.println();
        }
      }
      then = now;
      delay(10);
  }


  // Time for next record.
  logTime += 1000UL*SAMPLE_INTERVAL_MS;

  // Wait for log time.
  int32_t diff;
  do {
    diff = micros() - logTime;
  } while (diff < 0);

  // Check for data rate too high.
  if (diff > 10) {
    error("Missed data record");
  }

  logData();

  // Force data to SD and update the directory entry to avoid data loss.
  if (!file.sync() || file.getWriteError()) {
    error("write error");
  }

  if (Serial.available()) {
    // Close file and stop.
    file.close();
    Serial.println(F("Done"));
    while (true) {}
  }
}


  //   for (int i = 0; i < sensorPoints; i++) {  
  //   if (incomingValues[i] <= 1018){ //touch
  //     now = millis();
  //     between = now -then;
  //     Serial.print(i);
  //     Serial.print(" tapped; last tap: ");
  //     Serial.print(between);
  //     Serial.print("\t");
  //   }
  // }
  // }
  //   then = now;
  //   Serial.println(); 
  //   delay(20); 
    
  // }


  // for (int i = 0; i < sensorPoints; i++) {  
  //   if (incomingValues[i] != 1023){ //touch
  //     now = millis();
  //     if(now - then <= 200){
  //       then = now;
  //       oneTouch = true;
  //       Serial.print(i);
  //       Serial.print(" tapped "); //short touch
  //       Serial.print("\t");
  //     } 
  //     else if (now - then > 200 && now - then < 1000 ){
  //       then = now;
  //       oneTouch = false;
  //       Serial.print(i);
  //       Serial.print(" double tapped ");
  //       Serial.print("\t");
  //     }
  //     else if (now - then > 1000 && !oneTouch){
  //       then = now;
  //       Serial.print(i);
  //       Serial.print(" press"); //long touch
  //       Serial.print("\t");
  //     }
  //   }
  // }
  // } 
  //     oneTouch = false;
  //     Serial.println();
  //     delay(30); 
    
  // }

  // for (int i = 0; i < sensorPoints; i++) {  
  //   if (incomingValues[i] != 1023){ //touch
  //     now = millis();
  //     if(now - then <= 500){
  //       then = now;
  //       bool oneTouch = true;
  //       Serial.print(i);
  //       Serial.print(" tapped "); //short touch
  //       Serial.print("\t");
  //     } 
  //     else if (now - then > 500){
  //       then = now;
  //       oneTouch = false;
  //       Serial.print(i);
  //       Serial.print(" double tapped ");
  //       Serial.print("\t");
  //     }
  //   }
  // }
  // }
  //     Serial.println();
  //     delay(30); 
    
  // }


  // for (int i = 0; i < sensorPoints; i++) {  
  //   if (incomingValues[i] != 1023){ //touch
  //     Serial.print(i);
  //     Serial.print(" tapped ");
  //     Serial.print("\t");
  //   }
  // }
  // }
  //   Serial.println(); 
  //   delay(10); 
    
  // }


//   for (int i = 0; i < sensorPoints; i++) { 
//     Serial.print(incomingValues[i]); 
//     if (i < sensorPoints - 1) Serial.print("\t");
//     } 
//     Serial.println(); 
//     delay(10); 
//   }
// }



