/* APP NAME: freertosapp
 * VERSION: 0.7.0
 * AUTHOR: Jimmy Gizmo
 * REPO: git@github.com/jimmygizmo/freertosapp
 * WEBSITE: https://ninthdevice.com
 * DESCRIPTION AND OTHER INFO: (See end of file)
 * LANGUAGES (Versions approx.): Arduino, FreeRTOS v10.4.3-8, C++ v TODO: Finish
 * PLATFORM: Atmel AVR (3.3.0) > Arduino Uno  [OSEPP Arduino UNO R4 (Rev 4.0)], Velleman VMA203 LCD Keypad
 * HARDWARE: ATMEGA328P 16MHz, 2KB RAM, 31.50KB Flash
 * TOOLS: PlatformIO v5.1.1, TODO: Finish
 */
/************************************************* INCLUDES & DEFINES *************************************************/

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

// FreeRTOS Configuration
#define INCLUDE_vTaskDelay  1  // To enable vTaskDelay()
#define configSUPPORT_DYNAMIC_ALLOCATION  1  // To enable xTaskCreate()


/******************************************* CONFIGURATION & INITIALIZATION *******************************************/

int delay_delta = 10;  // The delta value in milliseconds by which blink_delay is either incremented or decremented.

int blink_delay = 10;  // Starting value for blink delay in milliseconds. blink_delay will change dynamically.

int delay_min = 10;  // Minimum blink_delay value in milliseconds before the delta_dir will change to ascending (1).

int delay_max = 140;  // Maximum blink_delay value in milliseconds before the delta_dir will change to descending (0).

int delta_dir = 1;  // Initial direction for blink_delay increment/decrement: 1 up (add), 0 down (subtract)
// Up/increment/add means the delays get longer. Down/decrement/subtract mean the delays get shorter.
// The same delay value is used for LED <ON> time as for the <OFF> time, which are two different delay events.

int led_pin = 13;

int milliseconds_per_tick = portTICK_PERIOD_MS;  // So: ticks = milliseconds / milliseconds_per_tick
// Ticks will be needed for vTaskDelay() and likely elsewhere.


/************************************************ FUNCTION DEFINITIONS ************************************************/

// Application-specific functions go here, where they must be defined before possible usage in setup() or loop().

void dynamic_blink_cycle_task(void *) {
    TickType_t blink_ticks;  // Will hold calculated tick value before each call to vTaskDelay().

    /* Consider how we might make this blinking non-blocking, if not completely, at least to a much greater degree.
     * At the moment it is a loop that modifies a delta value linearly larger and smaller again to operate the LED.
     * It totally blocks during the two delay events of each iteration in the full cycle.
     * We should be able to break this up by making each delay event into a FreeRTOS task so the Arduino can run
     * other tasks during this new NON-BLOCKING delay. The task scheduling will be dynamic like the delays in the
     * blocking version are. But, by using FreeRTOS, other parts of the application can independently do work at
     * the same time. For instance, we could also have a servo moving in some pattern, totally independent of the
     * dynamic blinking. This should be a very good initial proof of concept, prior to involving the LCD Keypad
     * and user-input etc. */

    digitalWrite(led_pin, HIGH);
    //delay(blink_delay);  // Trying the below FreeRTOS non-blocking delay
    blink_ticks = blink_delay / milliseconds_per_tick;
    vTaskDelay(blink_ticks);
    digitalWrite(led_pin, LOW);
    //delay(blink_delay);  // Trying the below FreeRTOS non-blocking delay
    blink_ticks = blink_delay / milliseconds_per_tick;
    vTaskDelay(blink_ticks);

    if (delta_dir == 0) {
        blink_delay = blink_delay - delay_delta;
    }
    else {
        blink_delay = blink_delay + delay_delta;
    }

    if (blink_delay < delay_min) {
        delta_dir = 1;
        blink_delay = delay_min;
    }
    if (blink_delay > delay_max) {
        delta_dir = 0;
        blink_delay = delay_max;
    }

} /* dynamic_blink_cycle() */


/*************************************************** ARDUINO SETUP ****************************************************/

//cppcheck-suppress unusedFunction
void setup() {
    pinMode(led_pin, OUTPUT);  // Our LED pin must be set for output.

    // FreeRTOS Initialization
    Serial.begin(9600);
    xTaskCreate(dynamic_blink_cycle_task, "BlinkTask", 128, NULL, 1, NULL);
    vTaskStartScheduler();


} /* setup() */


/************************************************* ARDUINO MAIN LOOP **************************************************/

//cppcheck-suppress unusedFunction
void loop() {
    // Currently this is just a simple blink test that does not actually use FreeRTOS yet.

    TaskFunction_t dynamic_blink_cycle_task();

} /* loop() */


/******************************************************* NOTES ********************************************************/

/*
 * DESCRIPTION:
 * This is my basic implementation of Arduino FreeRTOS (Real Time Operating System) on an UNO, as a starting point
 * for later specific, utilitarian purposes, initially this will involve basic blink, serial monitor output and
 * possibly the use of an LCD Keypad as I would like to develop a nice real-time, non-blocking menu system for
 * Arduino + LCDKeypad. I think this extremely affordable hardware combo has massive value potential for so many
 * kinds of projects. It all hinges on being non-blocking, in my opinion, because even though the LCDKeypad has
 * great potential for human I/O, this is NOT true if it remains single-threaded/blocking, for pretty-much any
 * kind of project. Yes, you could have a single Arduino running only the LCD Keypad menu I/O system and then
 * communicate with 1 or more other Arduinos or whatever, over serial or circuit logic or whatever, but there
 * is much value in being able to use an RTOS to free up the Arduino to do its most important work, while
 * simultaneously running a menu system, that does not necessarily need much CPU work; it simply needs real-time
 * responsiveness, while allowing other work to proceed with little or no interference between those separate
 * but simultaneous concerns.
 *
 */


/* NOTES:
 * Tutorial behind some of this work:
 * https://circuitdigest.com/microcontroller-projects/arduino-freertos-tutorial1-creating-freertos-task-to-blink-led-in-arduino-uno
 *
 * FreeRTOS Arduino Reference/Documentation:
 * https://www.arduino.cc/reference/en/libraries/freertos/
 *
 * Interesting tutorial that should be looked at, maybe referenced in this project (we'll see):
 * https://create.arduino.cc/projecthub/feilipu/using-freertos-multi-tasking-in-arduino-ebc3cc
 * It is a simple blink using FreeRTOS. They say it is generic for any Arduino and dont specify what board.
 *
 */


/**/
