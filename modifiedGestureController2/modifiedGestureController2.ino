int command;
#include<Wire.h>
#define IR_RX_TL A0                 // Top Left IR receiver photodiode on analog pin A0
#define IR_RX_TR A1                 // Top Right IR receiver photodiode on analog pin A1
#define IR_RX_BL A2                 // IR receiver photodiode on analog pin A2
#define IR_RX_BR A3                 // IR receiver photodiode on analog pin A3
#define IR_RX_L A4
#define IR_RX_R A5

#define IR_TX_TL 2                  // Top Left IR emitter LED on digital pin 2
#define IR_TX_TR 3                  // Top Right IR emitter LED on digital pin 3
#define IR_TX_BL 4                  // Bottom Left IR emitter LED on digital pin 4
#define IR_TX_BR 5                  // Bottom Right IR emitter LED on digital pin 5
#define IR_TX_L 6
#define IR_TX_R 7 

bool readGesture = true;            // Used to process the gesture

int ambientIR_TL;                   // Stores the ambient IR value read from the TL IR receiver
int ambientIR_TR;                   // Stores the ambient IR value read from the TR IR receiver
int ambientIR_BL;                   // Stores the ambient IR value read from the BL IR receiver
int ambientIR_BR;                   // Stores the ambient IR value read from the BR IR receiver
int ambientIR_L;
int ambientIR_R;
int obstacleIR_TL;                  // Stores the raw object IR value read from the TL IR receiver
int obstacleIR_TR;                  // Stores the raw object IR value read from the TR IR receiver
int obstacleIR_BL;                  // Stores the raw object IR value read from the BL IR receiver
int obstacleIR_BR;                  // Stores the raw object IR value read from the BR IR receiver
int obstacleIR_L;
int obstacleIR_R;
int value_TL[10];                   // Stores the IR value read from the TL IR receiver
int value_TR[10];                   // Stores the IR value read from the TR IR receiver
int value_BL[10];                   // Stores the IR value read from the BL IR receiver
int value_BR[10];                   // Stores the IR value read from the BR IR receiver
int value_L[10];
int value_R[10];
int distance_TL;                    // Stores the mapped IR value read from the TL IR receiver
int distance_TR;                    // Stores the mapped IR value read from the TR IR receiver
int distance_BL;                    // Stores the mapped IR value read from the BL IR receiver
int distance_BR;                    // Stores the mapped IR value read from the BR IR receiver
int distance_L;
int distance_R;
int calibration_TL;                 // Stores the raw IR value read from the TL receiver when the Arduino is powered
int calibration_TR;                 // Stores the raw IR value read from the TR receiver when the Arduino is powered
int calibration_BL;                 // Stores the raw IR value read from the BL receiver when the Arduino is powered
int calibration_BR;                 // Stores the raw IR value read from the BR receiver when the Arduino is powered
int calibration_L;
int calibration_R;
int lastDistanceHold_TL;            // The next variables are used to detect a "hold gesture" action
int lastDistanceHold_TR;
int lastDistanceHold_BL;
int lastDistanceHold_BR;
int lastDistanceHold_L;
int lastDistanceHold_R;
int holdMillis = 300;               // Used in a timer to determine a "hold gesture" action

uint8_t distanceArray_TL[100];      // Stores the last 100 IR values from the TL IR receiver to determine the gesture later
uint8_t distanceArray_TR[100];      // Stores the last 100 IR values from the TR IR receiver to determine the gesture later
uint8_t distanceArray_BL[100];      // Stores the last 100 IR values from the BL IR receiver to determine the gesture later
uint8_t distanceArray_BR[100];      // Stores the last 100 IR values from the BR IR receiver to determine the gesture
uint8_t distanceArray_L[100];
uint8_t distanceArray_R[100];
uint8_t distanceArrayCounter = 0;   // Used to access the previous array's content
uint8_t gesture = 0;                // Stores the gesture

uint32_t lastHoldMillis = 0;
void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
  pinMode(IR_TX_TL, OUTPUT);
  pinMode(IR_TX_TR, OUTPUT);
  pinMode(IR_TX_BL, OUTPUT);
  pinMode(IR_TX_BR, OUTPUT);
  pinMode(IR_TX_L, OUTPUT);
  pinMode(IR_TX_R, OUTPUT);
  digitalWrite(IR_TX_TL, LOW);
  digitalWrite(IR_TX_TR, LOW);
  digitalWrite(IR_TX_BL, LOW);
  digitalWrite(IR_TX_BR, LOW);
  digitalWrite(IR_TX_L, LOW);
  digitalWrite(IR_TX_R, LOW);
  calibrateIR(); 
  Wire.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  Wire.beginTransmission(8);
  readIR(5);                        // Read 5 times the IR values and store the average
  determineGesture();               // It determines the type of gesture made
 processHoldGesture();     
 Wire.endTransmission();
 delay(500);// It checks if the hand is being hold
}

void calibrateIR(){
  for(uint8_t x = 0; x < 5; x++){
    digitalWrite(IR_TX_TL,LOW);
    digitalWrite(IR_TX_TR,LOW);
    digitalWrite(IR_TX_BL,LOW);
    digitalWrite(IR_TX_BR,LOW);
    digitalWrite(IR_TX_L,LOW);
    digitalWrite(IR_TX_R,LOW);
    delay(2);
    ambientIR_TL = analogRead(IR_RX_TL);
    ambientIR_TR = analogRead(IR_RX_TR);
    ambientIR_BL = analogRead(IR_RX_BL);
    ambientIR_BR = analogRead(IR_RX_BR);
    ambientIR_L = analogRead(IR_RX_L);
    ambientIR_R = analogRead(IR_RX_R);
    digitalWrite(IR_TX_TL,HIGH);
    digitalWrite(IR_TX_TR,HIGH);
    digitalWrite(IR_TX_BL,HIGH);
    digitalWrite(IR_TX_BR,HIGH);
    digitalWrite(IR_TX_L,HIGH);
    digitalWrite(IR_TX_R,HIGH);
    delay(1);
    obstacleIR_TL = analogRead(IR_RX_TL);
    obstacleIR_TR = analogRead(IR_RX_TR);
    obstacleIR_BL = analogRead(IR_RX_BL);
    obstacleIR_BR = analogRead(IR_RX_BR);
    obstacleIR_L = analogRead(IR_RX_L);
    obstacleIR_R = analogRead(IR_RX_R);
    value_TL[x] = - ambientIR_TL + obstacleIR_TL;
    value_TR[x] = - ambientIR_TR + obstacleIR_TR;
    value_BL[x] = - ambientIR_BL + obstacleIR_BL;
    value_BR[x] = - ambientIR_BR + obstacleIR_BR;
    value_L[x] = - ambientIR_L + obstacleIR_L;
    value_R[x] = - ambientIR_R + obstacleIR_R;
  }
  for(uint8_t x = 0; x < 5; x++){
    distance_TL += value_TL[x];
    distance_TR += value_TR[x];
    distance_BL += value_BL[x];
    distance_BR += value_BR[x];
    distance_L += value_L[x];
    distance_R += value_R[x];
  }
  calibration_TL = (distance_TL / 5);
  calibration_TR = (distance_TR / 5);
  calibration_BL = (distance_BL / 5);
  calibration_BR = (distance_BR / 5);
  calibration_L = (distance_L / 5);
  calibration_R = (distance_R / 5);
}
void readIR(uint8_t times){
  for(uint8_t x = 0; x < times; x++){
    digitalWrite(IR_TX_TL,LOW);                   // We need to turn off the IR LEDs to read the ambient IR
    digitalWrite(IR_TX_TR,LOW);
    digitalWrite(IR_TX_BL,LOW);
    digitalWrite(IR_TX_BR,LOW);
    digitalWrite(IR_TX_L,LOW);
    digitalWrite(IR_TX_R,LOW);
    delay(2);                                     // Wait until the IR LEDs are completely turned off
    ambientIR_TL = analogRead(IR_RX_TL);          // Stores the ambient IR light
    ambientIR_TR = analogRead(IR_RX_TR);
    ambientIR_BL = analogRead(IR_RX_BL);
    ambientIR_BR = analogRead(IR_RX_BR);
    ambientIR_L = analogRead(IR_RX_L);
    ambientIR_R = analogRead(IR_RX_R);
    digitalWrite(IR_TX_TL,HIGH);                  // Turn on the IR LEDs to read the IR light reflected by the obstacle
    digitalWrite(IR_TX_TR,HIGH);
    digitalWrite(IR_TX_BL,HIGH);
    digitalWrite(IR_TX_BR,HIGH);
    digitalWrite(IR_TX_L,HIGH);
    digitalWrite(IR_TX_R,HIGH);
    delay(1);                                     // Wait until the IR LEDs are completely turned on
    obstacleIR_TL = analogRead(IR_RX_TL);         // Stores the IR light reflected by the obstacle
    obstacleIR_TR = analogRead(IR_RX_TR);
    obstacleIR_BL = analogRead(IR_RX_BL);
    obstacleIR_BR = analogRead(IR_RX_BR);
    obstacleIR_L = analogRead(IR_RX_L);
    obstacleIR_R = analogRead(IR_RX_R);
    value_TL[x] = - ambientIR_TL + obstacleIR_TL;   // Store the IR value to later calculate the average
    value_TR[x] = - ambientIR_TR + obstacleIR_TR;
    value_BL[x] = - ambientIR_BL + obstacleIR_BL;
    value_BR[x] = - ambientIR_BR + obstacleIR_BR;
    value_L[x] = - ambientIR_L + obstacleIR_L;
    value_R[x] = - ambientIR_R + obstacleIR_R;
  }
  for(uint8_t x = 0; x < times; x++){             // Calculate the average IR value per sensor
    distance_TL += value_TL[x];
    distance_TR += value_TR[x];
    distance_BL += value_BL[x];
    distance_BR += value_BR[x];
    distance_L += value_L[x];
    distance_R += value_R[x];
  }
  distance_TL = (distance_TL / times);            // Save the distance value
  distance_TR = (distance_TR / times);
  distance_BL = (distance_BL / times);
  distance_BR = (distance_BR / times);
  distance_L = (distance_L / times);
  distance_R = (distance_R / times);
  if(calibration_TL + 50 > distance_TL){          // Calibrate the sensors
    distance_TL = 0;
  }
  else{
    distance_TL -= calibration_TL + 50;
  }
  if(calibration_TR + 50 > distance_TR){
    distance_TR = 0;
  }
  else{
    distance_TR -= calibration_TR + 50;
  }
  if(calibration_BL + 50 > distance_BL){
    distance_BL = 0;
  }
  else{
    distance_BL -= calibration_BL + 50;
  }
  if(calibration_BR + 50 > distance_BR){
    distance_BR = 0;
  }
  else{
    distance_BR -= calibration_BR + 50;
  }
  if(calibration_L + 50 > distance_L){
    distance_L = 0;
  }
  else{
    distance_L -= calibration_L + 50;
  }
  if(calibration_R + 50 > distance_R){
    distance_R = 0;
  }
  else{
    distance_R -= calibration_R + 50;
  }

  // If any distance is greater than 0 then map it from a 0-1023 range to a 0-255 range (to be able to store it in a 1B variable) and store it in an array
  if(distance_TL != 0 || distance_TR != 0 || distance_BL != 0 || distance_BR != 0 || distance_L != 0 || distance_R != 0){
    distance_TR = map(constrain(distance_TR, 0, 1100), 0, 1100, 0, 255);
    distance_TL = map(constrain(distance_TL, 0, 1100), 0, 1100, 0, 255);
    distance_BR = map(constrain(distance_BR, 0, 1100), 0, 1100, 0, 255);
    distance_BL = map(constrain(distance_BL, 0, 1100), 0, 1100, 0, 255);
    distance_L = map(constrain(distance_L, 0, 1100), 0, 1100, 0, 255);
    distance_R = map(constrain(distance_R, 0, 1100), 0, 1100, 0, 255);
    if(distanceArrayCounter < 100){
      distanceArray_TR[distanceArrayCounter] = distance_TR;
      distanceArray_TL[distanceArrayCounter] = distance_TL;
      distanceArray_BR[distanceArrayCounter] = distance_BR;
      distanceArray_BL[distanceArrayCounter] = distance_BL;
      distanceArray_R[distanceArrayCounter] = distance_R;
      distanceArray_L[distanceArrayCounter] = distance_L;
      distanceArrayCounter += 1;
    }
    else{      
      distanceArray_TR[0] = distance_TR;
      distanceArray_TL[0] = distance_TL;
      distanceArray_BR[0] = distance_BR;
      distanceArray_BL[0] = distance_BL;
      distanceArray_L[0] = distance_L;
      distanceArray_R[0] = distance_R;
      distanceArrayCounter = 1;
    }
  }
  else if(distance_TL == 0 && distance_TR == 0 && distance_BL == 0 && distance_BR == 0 && distance_L == 0 && distance_R == 0){      // If not then reset the arrays
    readGesture = true;
    distanceArrayCounter = 0;
    distanceArray_TR[0] = 0;
    distanceArray_TL[0] = 0;
    distanceArray_BR[0] = 0;
    distanceArray_BL[0] = 0;
    distanceArray_R[0] = 0;
    distanceArray_L[0] = 0;
  }
}
void determineGesture(){
  if(distanceArrayCounter > 2){       // If the arrays have more than 2 elements then determine the gesture
    uint8_t distancePost_TL = 0;      // The next "distancePost..." vars are the sum of it previous value plus 
    uint8_t distancePost_TR = 0;      // the last element minus the previous element of the array
    uint8_t distancePost_BL = 0;
    uint8_t distancePost_BR = 0;
    uint8_t distancePost_L = 0;
    uint8_t distancePost_R = 0;
    uint8_t distancePostNeg_TL = 0;
    uint8_t distancePostNeg_TR = 0;
    uint8_t distancePostNeg_BL = 0;
    uint8_t distancePostNeg_BR = 0;
    uint8_t distancePostNeg_R = 0;
    uint8_t distancePostNeg_L = 0;
    uint8_t expectedGesture = 0;      // Used later to determine the gesture made
    bool increasing_TL = false;       // Used later to determine the gesture made. If the IR sensor's value 
    bool decreasing_TL = false;       // increases then "increasing_xx" is true. Otherwise, if it decreasing 
    bool increasing_TR = false;       // then "decreasing_xx" is true
    bool decreasing_TR = false;
    bool increasing_BL = false;
    bool decreasing_BL = false;
    bool increasing_BR = false;
    bool decreasing_BR = false;
    bool increasing_L = false;
    bool decreasing_L = false;
    bool increasing_R = false;
    bool decreasing_R = false;
    // For every element in the distanceArray...
    for(uint8_t x = 1; x < distanceArrayCounter; x++){
      // Update the "distancePost..." vars values
      if(distanceArray_TL[x] - distanceArray_TL[x-1] > 0){
        distancePost_TL += distanceArray_TL[x] - distanceArray_TL[x-1];
      }
      else{
        distancePostNeg_TL += distanceArray_TL[x-1] - distanceArray_TL[x];
      }
      
      if(distanceArray_TR[x] - distanceArray_TR[x-1] > 0){
        distancePost_TR += distanceArray_TR[x] - distanceArray_TR[x-1];
      }
      else{
        distancePostNeg_TR += distanceArray_TR[x-1] - distanceArray_TR[x];
      }
      
      if(distanceArray_BL[x] - distanceArray_BL[x-1] > 0){
        distancePost_BL += distanceArray_BL[x] - distanceArray_BL[x-1];
      }
      else{
        distancePostNeg_BL += distanceArray_BL[x-1] - distanceArray_BL[x];
      }
      
      if(distanceArray_BR[x] - distanceArray_BR[x-1] > 0){
        distancePost_BR += distanceArray_BR[x] - distanceArray_BR[x-1];
      }
      else{
        distancePostNeg_BR += distanceArray_BR[x-1] - distanceArray_BR[x];
      }
      if(distanceArray_L[x] - distanceArray_L[x-1] > 0){
        distancePost_L += distanceArray_L[x] - distanceArray_L[x-1];
      }
      else{
        distancePostNeg_L += distanceArray_L[x-1] - distanceArray_L[x];
      }
      if(distanceArray_R[x] - distanceArray_R[x-1] > 0){
        distancePost_R += distanceArray_R[x] - distanceArray_R[x-1];
      }
      else{
        distancePostNeg_R += distanceArray_R[x-1] - distanceArray_R[x];
      }
    }
      
      // Based on the "distancePost..." vars determine if the value is increasing or decreasing
      if(distancePost_TL > 10 && distancePostNeg_TL < 10){
        increasing_TL = true;
        decreasing_TL = false;
      }
      else if(distancePostNeg_TL > 10){
        increasing_TL = false;
        decreasing_TL = true;
      }
      
      if(distancePost_TR > 10 && distancePostNeg_TR < 10){
        increasing_TR = true;
        decreasing_TR = false;
      }
      else if(distancePostNeg_TR > 10){
        increasing_TR = false;
        decreasing_TR = true;
      }
      
      if(distancePost_BL > 10 && distancePostNeg_BL < 10){
        increasing_BL = true;
        decreasing_BL = false;
      }
      else if(distancePostNeg_BL > 10){
        increasing_BL = false;
        decreasing_BL = true;
      }
      
      if(distancePost_BR > 10 && distancePostNeg_BR < 10){
        increasing_BR = true;
        decreasing_BR = false;
      }
      else if(distancePostNeg_BR > 10){
        increasing_BR = false;
        decreasing_BR = true;
      }
      if(distancePost_L > 10 && distancePostNeg_L < 10){
        increasing_L = true;
        decreasing_L = false;
      }
      else if(distancePostNeg_L > 10){
        increasing_L = false;
        decreasing_L = true;
      }
      if(distancePost_R > 10 && distancePostNeg_R < 10){
        increasing_BR = true;
        decreasing_BR = false;
      }
      else if(distancePostNeg_R > 10){
        increasing_R = false;
        decreasing_R = true;
      }

      // Determine the type of gesture //for present south and north gestures have not been updated
      switch(expectedGesture){
        case 0:                   // If "expectedGesture" is 0 then determine the expected gesture 
                                  // by analyzing if the sensors' values are increasing or decreasing
          command=0;;
          if((increasing_TR && !increasing_TL) || (increasing_BR && !increasing_BL) || (increasing_R && !increasing_L)){         // Left to right movement
            expectedGesture = 1;  // Expected an "east" gesture
          }
          else if((!increasing_TR && increasing_TL) || (!increasing_BR && increasing_BL) || (!increasing_R && increasing_L)){    // Right to left movement
            expectedGesture = 2;  // Expected a "west" gesture
          }
          else if(increasing_TR && increasing_TL && !increasing_BL && !increasing_BR){    // Top to bottom movement
            expectedGesture = 3;  // Expected a "south" gesture
          }
          else if(increasing_BR && increasing_BL && !increasing_TL && !increasing_TR){    // Bottom to top movement
            expectedGesture = 4;  // Expected a "north" gesture
          }
          // Check if the current "distance_xx" minus the 2nd element in the array is greater than 50 then it is a "down" movement.
          // The "increasing_xx" vars are not used because they are very precise and they didn´t detect the "down" movement most of the time.
          else if(distance_TL - distanceArray_TL[2] > 50 && distance_TR - distanceArray_TR[2] > 50 && distance_BL - distanceArray_BL[2] > 50 && distance_BR - distanceArray_BR[2] > 50){  // Up to down movementd
            expectedGesture = 5;  // Expected a "down" gesture
          }
          
        case 1:   
          // If "expectedGesture" is "east" then check if TR and BR sensors are increasing and TL and BL are decreasing. 
          // If this is true then save the current distance to the var "lastDistanceHold" to use it later to check a "hold 
          // gesture" action. Also, reset the "expectedGesture" var and set the "gesture" var to 1 ("east")
          if((increasing_TR && !increasing_TL) || (increasing_BR && !increasing_BL) || (increasing_R && !increasing_L)){
            lastDistanceHold_TR = distance_TR;
            lastDistanceHold_BR = distance_BR;
            lastDistanceHold_TL = distance_TL;
            lastDistanceHold_BL = distance_BL;
            lastDistanceHold_R = distance_R;
            lastDistanceHold_L = distance_L;
            if(readGesture){
              Serial.println("EAST");
              gesture = 1;
              holdMillis = 300;   // Change it to 300ms to add a small delay between the gesture "east" and the "hold gesture" action
              lastHoldMillis = millis();
              command=1;
              Wire.write(command);
            }
            expectedGesture = 0;
            increasing_TL = false;
            decreasing_TL = false;
            increasing_TR = false;
            decreasing_TR = false;
            increasing_BL = false;
            decreasing_BL = false;
            increasing_BR = false;
            decreasing_BR = false;
            increasing_R = false;
            decreasing_R = false;
            increasing_L = false;
            decreasing_L = false;
            readGesture = false;
          }
          
        case 2:
          if((!increasing_TR && increasing_TL) || (!increasing_BR && increasing_BL) || (!increasing_R && increasing_L))
          {
            if(readGesture){
              Serial.println("WEST");
              gesture = 2;
              holdMillis = 300;
              lastHoldMillis = millis();
              command=2;
              Wire.write(command);
            }
            expectedGesture = 0;
            lastDistanceHold_TR = distance_TR;
            lastDistanceHold_BR = distance_BR;
            lastDistanceHold_TL = distance_TL;
            lastDistanceHold_BL = distance_BL;
            lastDistanceHold_R = distance_R;
            lastDistanceHold_L = distance_L;
            increasing_TL = false;
            decreasing_TL = false;
            increasing_TR = false;
            decreasing_TR = false;
            increasing_BL = false;
            decreasing_BL = false;
            increasing_BR = false;
            decreasing_BR = false;
            increasing_R = false;
            decreasing_R = false;
            increasing_L = false;
            decreasing_L = false;
            readGesture = false;
          }
          break;
          
       /* case 3:
          if(increasing_BR && increasing_BL && decreasing_TL && decreasing_TR){
            if(readGesture){
              Serial.println("SOUTH");
              gesture = 3;
              holdMillis = 300;
              lastHoldMillis = millis();
            }
            expectedGesture = 0;
            lastDistanceHold_TR = distance_TR;
            lastDistanceHold_BR = distance_BR;
            lastDistanceHold_TL = distance_TL;
            lastDistanceHold_BL = distance_BL;
            increasing_TL = false;
            decreasing_TL = false;
            increasing_TR = false;
            decreasing_TR = false;
            increasing_BL = false;
            decreasing_BL = false;
            increasing_BR = false;
            decreasing_BR = false;
            readGesture = false;
          }
          
        case 4:
          if(increasing_TR && increasing_TL && decreasing_BL && decreasing_BR){
            if(readGesture){
              Serial.println("NORTH");
              gesture = 4;
              holdMillis = 300;
              lastHoldMillis = millis();
            }
            expectedGesture = 0;
            lastDistanceHold_TR = distance_TR;
            lastDistanceHold_BR = distance_BR;
            lastDistanceHold_TL = distance_TL;
            lastDistanceHold_BL = distance_BL;
            increasing_TL = false;
            decreasing_TL = false;
            increasing_TR = false;
            decreasing_TR = false;
            increasing_BL = false;
            decreasing_BL = false;
            increasing_BR = false;
            decreasing_BR = false;
            readGesture = false;
          }
          break;
          
        /*case 5:
          // The next if statement checks if 3 sensors are increasing its value and if it is true then store a "down" gesture
          if((increasing_BR && increasing_BL && increasing_TL) || (increasing_TR && increasing_BL && increasing_TL) || (increasing_TR && increasing_BR && increasing_TL) || (increasing_TR && increasing_BR && increasing_BL)){
            if(readGesture){
              Serial.println("DOWN");
              gesture = 5;
              holdMillis = 300;
              lastHoldMillis = millis();
            }
            expectedGesture = 0;
            increasing_TL = false;
            decreasing_TL = false;
            increasing_TR = false;
            decreasing_TR = false;
            increasing_BL = false;
            decreasing_BL = false;
            increasing_BR = false;
            decreasing_BR = false;
            readGesture = false;
          }
          break;*/
      } 
    
  }
}
void processHoldGesture(){
  // If the actual "distance_xx" minus the "lastDistanceHold_xx" (saved when the gesture was detected) 
  // is within a range (20) then make a "hold gesture" action
  if(-20 <= distance_TR - lastDistanceHold_TR && distance_TR - lastDistanceHold_TR <= 20 && -20 <= distance_TL - lastDistanceHold_TL && distance_TL - lastDistanceHold_TL <= 20 && -20 <= distance_BR - lastDistanceHold_BR && distance_BR - lastDistanceHold_BR <= 20 && -20 <= distance_BL - lastDistanceHold_BL && distance_BL - lastDistanceHold_BL <= 20 && -20 <= distance_R - lastDistanceHold_R && distance_R - lastDistanceHold_R <= 20 && -20 <= distance_L - lastDistanceHold_L && distance_L - lastDistanceHold_L <= 20){
    if(distance_TR != 0 || distance_TL != 0 || distance_BR != 0 || distance_BL != 0 || distance_L != 0 || distance_R != 0  ){
      switch(gesture){
        case 1:
          if(millis() - lastHoldMillis > holdMillis){
            holdMillis = 100;
            Serial.println("HOLD EAST");
            lastHoldMillis = millis();
          }
          break;
        case 2:
          if(millis() - lastHoldMillis > holdMillis){
            holdMillis = 100;
            Serial.println("HOLD WEST");
            lastHoldMillis = millis();
          }
          break;
        case 3:
          if(millis() - lastHoldMillis > holdMillis){
            holdMillis = 100;
            Serial.println("HOLD SOUTH");
            lastHoldMillis = millis();
          }
          break;
        case 4:
          if(millis() - lastHoldMillis > holdMillis){
            holdMillis = 100;
            Serial.println("HOLD NORTH");
            lastHoldMillis = millis();
          }
          break;
      }
    }
    else{
      // Set a high number for the "lastDistanceHold" vars to stop reading a "hold gesture" action
      lastDistanceHold_TL = 20000;
      lastDistanceHold_TR = 20000;
      lastDistanceHold_BR = 20000;
      lastDistanceHold_BL = 20000;
    }
  }
  else{
    lastDistanceHold_TL = 20000;
    lastDistanceHold_TR = 20000;
    lastDistanceHold_BL = 20000;
    lastDistanceHold_BR = 20000;
    lastDistanceHold_R = 20000;
    lastDistanceHold_L = 20000;
  }
}
