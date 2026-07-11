/*
Pressure Sensor Matrix Code
parsing through a pressure sensor matrix grid by switching individual
rows/columns to be HIGH, LOW or INPUT (high impedance) to detect
location and pressure.
>> https://www.kobakant.at/DIY/?p=7443
*/
#define numRows 2
#define numCols 1
#define sensorPoints numRows*numCols

int rows[] = { A9, A7};
int cols[] = {1};
int incomingValues[sensorPoints] = {};

int elapsedTime; 
int touch[] = {0,0};

void setup() {
  // set all rows and columns to INPUT (high impedance):
  for (int i = 0; i < numRows; i++) { // iterates over each row of the pressure sensor; i starts at 0 and goes up to numRows-1
  pinMode(rows[i], INPUT_PULLUP); //calls current row; INPUT_PULLUP configures the pin as an input and enables internal pull-up resistor. 
  //When no external device connected, pin is at level of microcontroller (reads HIGH by default, unless external device pulls LOW)
  }

  for (int i = 0; i < numCols; i++) { //iterates over each column of the pressure sensor; i starts at 0 and goes up to numCols-1
  pinMode(cols[i], INPUT); //column pin as input 
  }
  Serial.begin(9600);
  }

void loop() {
  for (int colCount = 0; colCount < numCols; colCount++) { //iterating over each column of pressure sensor 
  pinMode(cols[colCount], OUTPUT); // calls for the current column, sets as OUTPUT to control the state of column when scanning sensor 
  digitalWrite(cols[colCount], LOW); // sets initial output as LOW by default 


  for (int rowCount = 0; rowCount < numRows; rowCount++) {
  incomingValues[colCount * numRows + rowCount] = analogRead(rows[rowCount]); // iterates over each row of sensor, analogREAD() reads voltage from spec. analog input pin and converts to digital value btwn 0-1023
  //digital value is stored in incomingValues array at correct index
  }// end rowCount

  pinMode(cols[colCount], INPUT); // end colCount and set them back to INPUT 
  }

  // for (int i = 0; i < incomingValues; i++){
  //   touch[0] = touch[1];
  //   touch[1] = millis();
  //   elapsedTime = touch[1] - touch[0];
  //   if (elapsedTime < 1800 && elapsedTime > 500 && incomingValues[i] < 1020){
  //     Serial.println("some touch");
  //   }
  }

  //Print the incoming values of the grid:
  for (int i = 0; i < sensorPoints; i++) {
  Serial.print(incomingValues[i]);
  if (i < sensorPoints - 1) Serial.print("\t");
  }

  Serial.println();
  delay(30);
  }

