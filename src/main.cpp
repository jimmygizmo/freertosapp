/*
 * Tutorial: https://circuitdigest.com/microcontroller-projects/arduino-freertos-tutorial1-creating-freertos-task-to-blink-led-in-arduino-uno
 */

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

void setup() {
    pinMode(13, OUTPUT);
}

int del = 20;
int dir = 1; // 1 up, 0 down

void loop() {
    digitalWrite(13, HIGH);
    delay(del);
    digitalWrite(13, LOW);
    delay(del);
    if (dir == 0) { del = del - 20; }
    else { del = del + 20; }
    if (del < 20) { dir = 1; del = 20; }
    if (del > 240) {dir = 0; del = 240; }
}

//void setup() {
//// write your initialization code here
//}
//
//void loop() {
//// write your code here
//}