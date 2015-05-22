#include <Wire.h>

//#include "Roomba.h"
#include "ArduinoNunchuk.h"

//Roomba roomba(&Serial1);
int RXLED = 17;  // The RX LED has a defined Arduino pin
int DD = 9;  // Device Detect Pin.  Active Low

ArduinoNunchuk nunchuk = ArduinoNunchuk();

const uint8_t twinkle[] = {
  7, // Length
  72, 16, // C4
  72, 16, // C4
  79, 16, // G4
  79, 16, // G4
  81, 16, // A5
  81, 16, // A5
  79, 32, // G4  
};

const uint8_t elise[] = {
  9,
  76, 16, // E4
  75, 16, // D#4
  76, 16, // E4
  75, 16, // D#4
  76, 16, // E4
  71, 16, // B
  74, 16, // D
  72, 16, // C
  69, 32, // A
};

void send(int message) {
  /* Send a message with a built in delay */
  Serial1.write(message);
  delay(20);
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(19200);
  
  pinMode(RXLED, OUTPUT);
  pinMode(DD, OUTPUT);
  digitalWrite(DD, HIGH);

  Serial.println("Started!!!");
  digitalWrite(RXLED, LOW);
  
  // Device Detect Wakeup
  digitalWrite(DD, LOW);
  digitalWrite(RXLED, LOW);
  delay(1000);
  digitalWrite(DD, HIGH);
  digitalWrite(RXLED, HIGH);  
  delay(500);
  
  // Device Detect Force to 19200 Baud
  for (int i = 0; i < 3; i++) {
    digitalWrite(DD, LOW);
    digitalWrite(RXLED, LOW);
    delay(150);
    digitalWrite(DD, HIGH);
    digitalWrite(RXLED, HIGH);
    delay(150);
  }
  send(128);  // Off -> Passive
  send(130);  // Passive -> Control (Safe)
  send(132);  // Safe -> Full
  save_songs();
  send(141);  // Play song
  send(0);    // Song 0
  send(141);  // Play song
  send(1);    // Song 1
  nunchuk.init();
  
}

void save_songs() {
  send(140);
  send(0);
  for (int i = 0; i < 15; i++) {
    send(twinkle[i]);
  }
  
  // Fur Elise as 1
  send(140);
  send(1);
  for (int i = 0; i < 19; i++) {
    send(elise[i]);
  }
}


void print_status() {
  char buf[26];
  Serial.println("Querying for status...");
  send(142);
  send(0);
  Serial.println("Reading!");
  for (int i = 0; i < 26; i++) {
    while(!Serial1.available());
    buf[i] = Serial1.read();
  }
  Serial.print("Wheeldrop C/L/R: ");
  Serial.print(buf[0] & 0x10, DEC);
  Serial.print("/");
  Serial.print(buf[0] & 0x08, DEC);
  Serial.print("/");
  Serial.println(buf[0] & 0x04, DEC);
  
  Serial.print("Bump L/R: ");
  Serial.print(buf[0] & 0x02, DEC);
  Serial.print("/");
  Serial.println(buf[0] & 0x01, DEC);
  
  Serial.print("Wall: ");
  Serial.println(buf[1], DEC);
  
  Serial.print("Cliff L FL FR R: ");
  for (int j = 2; j < 6; j++) {
    Serial.print(buf[j], DEC);
    Serial.print(" ");
  }
  Serial.println();
  Serial.println();
  
}

void print_wii_status() {
  nunchuk.update();

  Serial.print(nunchuk.analogX, DEC);
  Serial.print(' ');
  Serial.print(nunchuk.analogY, DEC);
  Serial.print(' ');
  Serial.print(nunchuk.zButton, DEC);
  Serial.print(' ');
  Serial.println(nunchuk.cButton, DEC);
}

void loop() {
  //print_status();
  print_wii_status();
  delay(50);
}
