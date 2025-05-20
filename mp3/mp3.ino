#include "SoftwareSerial.h"
SoftwareSerial mySerial(10, 11);

#define Start_Byte       0x7E
#define Version_Byte     0xFF
#define Command_Length   0x06
#define End_Byte         0xEF
#define Acknowledge      0x00 // 0x01: feedback, 0x00: no feedback
#define ACTIVATED        LOW

// Button pins
int buttonNext     = 2;
int buttonPause    = 3;
int buttonPrevious = 4;
int buttonVolUp    = 5;
int buttonVolDown  = 6;

int currentVolume = 20; // Default volume
boolean isPlaying = false;

void setup() {
  // Setup buttons
  pinMode(buttonPause, INPUT);
  digitalWrite(buttonPause, HIGH);
  pinMode(buttonNext, INPUT);
  digitalWrite(buttonNext, HIGH);
  pinMode(buttonPrevious, INPUT);
  digitalWrite(buttonPrevious, HIGH);
  pinMode(buttonVolUp, INPUT_PULLUP);
  digitalWrite(buttonVolUp, HIGH);
  pinMode(buttonVolDown, INPUT_PULLUP);
  digitalWrite(buttonVolDown, HIGH);

  // Start serial
  mySerial.begin(9600);
  delay(1000);

  // Play first track
  playFirst();
  isPlaying = true;
}

void loop() {
  if (digitalRead(buttonPause) == ACTIVATED) {
    if (isPlaying) {
      pause();
      isPlaying = false;
    } else {
      play();
      isPlaying = true;
    }
    delay(300); // Debounce delay
  }

  if (digitalRead(buttonNext) == ACTIVATED && isPlaying) {
    playNext();
    delay(300);
  }

  if (digitalRead(buttonPrevious) == ACTIVATED && isPlaying) {
    playPrevious();
    delay(300);
  }

  if (digitalRead(buttonVolUp) == ACTIVATED) {
    Serial.println("Volume Up Pressed");
    increaseVolume();
    delay(300);
  }
// debugging in C++
  if (digitalRead(buttonVolDown) == ACTIVATED) {
    Serial.println("Volume Down Pressed");
    decreaseVolume();
    delay(300);
  }

}





// ----------- MP3 Control Functions -----------

void playFirst() {
  execute_CMD(0x3F, 0, 0); // Reset
  delay(500);
  setVolume(currentVolume);
  delay(300);
  execute_CMD(0x11, 0, 1); // Play first
  delay(500);
}

void pause() {
  execute_CMD(0x0E, 0, 0); // Pause
  delay(500);
}

void play() {
  execute_CMD(0x0D, 0, 1); // Resume play
  delay(500);
}

void playNext() {
  execute_CMD(0x01, 0, 1); // Next track
  delay(500);
}

void playPrevious() {
  execute_CMD(0x02, 0, 1); // Previous track
  delay(500);
}

void setVolume(int volume) {
  currentVolume = constrain(volume, 0, 30);
  execute_CMD(0x06, 0, currentVolume);
  delay(500);
}

void increaseVolume() {
  if (currentVolume < 30) {
    currentVolume++;
    setVolume(currentVolume);
  }
}

void decreaseVolume() {
  if (currentVolume > 0) {
    currentVolume--;
    setVolume(currentVolume);
  }
}

void execute_CMD(byte CMD, byte Par1, byte Par2) {
  word checksum = -(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);
  byte Command_line[10] = {
    Start_Byte, Version_Byte, Command_Length, CMD, Acknowledge,
    Par1, Par2, highByte(checksum), lowByte(checksum), End_Byte
  };

  for (byte k = 0; k < 10; k++) {
    mySerial.write(Command_line[k]);
  }
}
