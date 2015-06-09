int segmentPins[] = {4, 5, 6, 7, 8, 9, 10, 11};
int numPins = 8;
int prevSegment = -1;
int firstDigitPin = 2;
int secondDigitPin = 3;

int estimates[] = {0, 0, 0, 0, 0};
int displayedEstimates[] = {0, 0, 0, 0, 0};
int estimatesLength = 5;
int numEstimates = 0;
int numDisplayedEstimates = 0;
int displayIndex = 0;
int maxEstimates = 3;
String dataIn;

int digitSegments[8];
int secondDigitSegments[8];
int numSecondSegments;
int numSegments;

long start = 0;
long serialPoll = 1000;
long serialTime = 0;
long displayTime = 0;
long displaySwitch = 3000;
long currentTime = 0;
long dataTime = 0;
long dataTimeout = 45000;

void setup() {
  for (int i = 0; i < numPins; i++) {
    pinMode(segmentPins[i], OUTPUT); 
  }
  pinMode(firstDigitPin, OUTPUT);
  pinMode(secondDigitPin, OUTPUT);
  
  Serial.begin(9600);
  Serial.setTimeout(1000);
  start = millis();
}

void loop() {
   currentTime = millis();
   readSerialAtInterval();
   switchEstimate();
   displayDigit();
}

void displayDigit() {
   if (displayedEstimates[displayIndex] > 0) {
     displayNumber(displayedEstimates[displayIndex], 1);
   } else {
     delay(1);
   }
}

void switchEstimate() {
   if (estimates[3] > 0 && estimates[3] - estimates[0] < 30) {
      maxEstimates = 4;
      displaySwitch = 3000;
   } else {
     maxEstimates = 3;
     displaySwitch = 4000; 
   }
   if (displayTime == 0 || currentTime - displayTime > displaySwitch) {
     if (displayTime == 0 || displayIndex >= min(numEstimates, maxEstimates) - 1) {
       displayIndex = 0; 
     } else {
       if (
         estimates[displayIndex + 1] == displayedEstimates[displayIndex + 1] ||
         displayedEstimates[displayIndex + 1] - estimates[displayIndex] > 3 || 
         displayedEstimates[displayIndex + 1] == 0
        ) {
         displayIndex++;
       }
     }
     displayTime = currentTime;
     displayedEstimates[displayIndex] = estimates[displayIndex];
   }
}

void readSerialAtInterval() {
  if (serialTime == start || currentTime - serialTime > serialPoll) {
    serialTime = currentTime;
    readSerial();
  }
}

void readSerial() {
 if (Serial.available() > 0) {
   dataIn = Serial.readStringUntil(';');
   if (dataIn.length() > 0) {
     dataTime = currentTime;
     clearEstimates();
     int estimateIndex = -1;
     for (int i = 0; i < dataIn.length(); i++) {
         if (dataIn[i] == ' ') {
           estimateIndex++;
         } else {
           estimates[estimateIndex+1] *= 10;
           estimates[estimateIndex+1] += (dataIn[i] - '0');
         }
      }
      numEstimates = estimateIndex + 1;
      for (int i = estimateIndex + 1; i < estimatesLength; i++) {
        estimates[i] = 0; 
      }
   } else {
     clearEstimatesIfTimedOut();   
   }
 } else {
   clearEstimatesIfTimedOut(); 
 }
}

void clearEstimatesIfTimedOut() {
  if (currentTime - dataTime > dataTimeout) {
   clearEstimates(); 
  } 
}

void clearEstimates() {
   for (int i = 0; i < estimatesLength; i++) {
     estimates[i] = 0;
   }
}

void displayNumber(int number, int time) {
   int firstDigit = (number % 100) / 10;
   int secondDigit = (number % 10);
   displayDigits(firstDigit, secondDigit, time);
}

void activateSegments(int segments[], int numSegments, int delayTime) {
   for (int i = 0; i < numSegments; i++) {
     digitalWrite(segmentPins[segments[i]], HIGH);
     if (prevSegment >= 0) digitalWrite(segmentPins[prevSegment], LOW);
     prevSegment = segments[i];
     delayMicroseconds(delayTime);
  }
   if (prevSegment >= 0) digitalWrite(segmentPins[prevSegment], LOW);
}

void displayDigits(int digit, int secondDigit, long time) {
   int delayTime = 100;
   long currTime = 0L;
   numSegments = getDigitSegments(digit, digitSegments);
   numSecondSegments = getDigitSegments(secondDigit, secondDigitSegments);
   time = time*1000;
   while (currTime < time) {
     if (digit > 0) {
          activateFirstDigit();
          activateSegments(digitSegments, numSegments, delayTime);
          currTime += numSegments * delayTime;
     }
     activateSecondDigit();
     activateSegments(secondDigitSegments, numSecondSegments, delayTime);
     currTime += numSecondSegments * delayTime;
   }
}

void activateFirstDigit() {
 digitalWrite(firstDigitPin, HIGH);
 digitalWrite(secondDigitPin, LOW); 
}

void activateSecondDigit() {
 digitalWrite(secondDigitPin, HIGH); 
 digitalWrite(firstDigitPin, LOW);
}

int getDigitSegments(int digit, int digitSegments[]) {
  switch(digit) {
     case 1:
        digitSegments[0] = 3;
        digitSegments[1] = 7;
        return 2;
        break;
     case 2:
        digitSegments[0] = 0;
        digitSegments[1] = 2;
        digitSegments[2] = 3;
        digitSegments[3] = 4;
        digitSegments[4] = 6;
        return 5;
        break;
      case 3:
        digitSegments[0] = 0;
        digitSegments[1] = 2;
        digitSegments[2] = 3;
        digitSegments[3] = 4;
        digitSegments[4] = 7;
        return 5;
        break;
      case 4:
        digitSegments[0] = 0;
        digitSegments[1] = 1;
        digitSegments[2] = 3;
        digitSegments[3] = 7;
        return 4;
        break;
      case 5:
        digitSegments[0] = 0;
        digitSegments[1] = 1;
        digitSegments[2] = 2;
        digitSegments[3] = 4;
        digitSegments[4] = 7;
        return 5;
        break;
      case 6:
        digitSegments[0] = 0;
        digitSegments[1] = 1;
        digitSegments[2] = 2;
        digitSegments[3] = 4;
        digitSegments[4] = 6;
        digitSegments[5] = 7;
        return 6;
        break;
      case 7:
        digitSegments[0] = 2;
        digitSegments[1] = 3;
        digitSegments[2] = 7;
        return 3;
        break;
      case 8:
        digitSegments[0] = 0;
        digitSegments[1] = 1;
        digitSegments[2] = 2;
        digitSegments[3] = 3;
        digitSegments[4] = 4;
        digitSegments[5] = 6;
        digitSegments[6] = 7;
        return 7;
        break;
      case 9:
        digitSegments[0] = 0;
        digitSegments[1] = 1;
        digitSegments[2] = 2;
        digitSegments[3] = 3;
        digitSegments[4] = 4;
        digitSegments[5] = 7;
        return 6;
        break;
     case 0:
        digitSegments[0] = 1;
        digitSegments[1] = 2;
        digitSegments[2] = 3;
        digitSegments[3] = 4;
        digitSegments[4] = 6;
        digitSegments[5] = 7;
        return 6;
        break;
   }
}
