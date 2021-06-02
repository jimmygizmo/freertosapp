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
#include <LiquidCrystal.h>

// FreeRTOS Configuration
#define INCLUDE_vTaskDelay  1  // To enable vTaskDelay()
#define configSUPPORT_DYNAMIC_ALLOCATION  1  // To enable xTaskCreate()


/******************************************* CONFIGURATION & INITIALIZATION *******************************************/

/******** LED Blinking ********/

// TODO: Most of these are constants at the moment, so use the correct style.
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


/******** FreeRTOS Task Function Prototypes ********/

// Function prototypes (Is this just a FreeRTOS thing or when else would we do this?)
// TODO: note on void args
[[noreturn]] void dynamic_blink_cycle_task(void *pvParameters);
[[noreturn]] void simple_blink_cycle_task(void *pvParameters);


/******** LCD Keypad ********/

/* NOTE: For the configuration of LCD Keypad hardware, there is a small chance of variance between different
 * hardware vendors. You should check with the reference documentation for your specific hardware.
 * My LCD Keypad Shield is made to attach to an Arduino UNO or Mega 2560 and is the Velleman VMA203 LCD1602.
 * The pin configuration for the LCD module for the LiquidCrystal library is:
 * LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
 * WARNING: Some LCD Keypads allow pin control of the backlight for on/off/brightness. NEVER USE THIS. Nearly all
 * LCD Keypad shields that allow pin control of the backlight have a hardware flaw. If you ever set the designated
 * pin to OUTPUT in order to use this feature, you will most likely burn out your Arduino voltage regulator.
 * */

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


/************************************************ FUNCTION DEFINITIONS ************************************************/

// Application-specific functions go here, where they must be defined before possible usage in setup() or loop().

[[noreturn]] void dynamic_blink_cycle_task(void *pvParameters) {
    // See simple_blink_cycle_task() below for comments on [[noreturn]], used to suppress CLang-Tidy endless loop warn.
    TickType_t blink_ticks;  // Will hold calculated tick value before each call to vTaskDelay().

    while (true) {
        digitalWrite(led_pin, HIGH);
        blink_ticks = blink_delay / milliseconds_per_tick;
        vTaskDelay(blink_ticks);
        digitalWrite(led_pin, LOW);
        blink_ticks = blink_delay / milliseconds_per_tick;
        vTaskDelay(blink_ticks);

        if (delta_dir == 0) {
            blink_delay = blink_delay - delay_delta;
        } else {
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
    }

} /* dynamic_blink_cycle_task() */


[[noreturn]] void simple_blink_cycle_task(void *pvParameters) {
    // half second on, half second off.
    // [[noreturn]] decoration above suppresses the Clang-Tidy warning that this is an endless loop.
    // If [[noreturn]] does more than warning-suppression, we need to reconsider using it. CLion recommended it.
    // TODO: research the full impact of [[noreturn]] Also, most apps would want to start and stop blinking
    //   as conditions change and for this we might need to use a handle in the xTaskCreate so we can
    //   stop the task later. In many cases an app might need to stop tasks it had started.
    TickType_t blink_ticks = 500 / milliseconds_per_tick;

    while (true) {
        digitalWrite(led_pin, HIGH);
        vTaskDelay(blink_ticks);
        digitalWrite(led_pin, LOW);
        vTaskDelay(blink_ticks);
    }
} /* simple_blink_cycle_task() */


/*************************************************** ARDUINO SETUP ****************************************************/

//cppcheck-suppress unusedFunction
void setup() {
    pinMode(led_pin, OUTPUT);  // Our LED pin must be set for output.

    // LCD Initialization
    lcd.begin(16,2);
    lcd.clear();

    // TODO: This is very temporary, to test the LCD. At the moment, FreeRTOS code is never reached.
    uint8_t i = 0;
    while (1) {
        lcd.clear();
        lcd.print("Codes 0x"); lcd.print(i, HEX);
        lcd.print("-0x"); lcd.print(i+16, HEX);
        lcd.setCursor(0, 1);
        for (int j=0; j<16; j++) {
            lcd.write(i+j);
        }
        i+=16;

        delay(3000);
    }


    // FreeRTOS Initialization
    Serial.begin(9600);
    xTaskCreate(dynamic_blink_cycle_task, "DynamicBlinkTask", 128, NULL, 1, NULL);
    //xTaskCreate(simple_blink_cycle_task, "SimpleBlinkTask", 128, NULL, 1, NULL);
    vTaskStartScheduler();

} /* setup() */


/************************************************* ARDUINO MAIN LOOP **************************************************/

//cppcheck-suppress unusedFunction
void loop() {

    // TODO: Resolve this: I don't think we need to call anything in loop() for tasks. When using FreeRTOS, a lot of
    //   stuff, if not everything, will be happening in (or rather starting from) setup().
    //   What can/should we use loop() for in a FreeRTOS app? Does it use resources even when empty? If we will not
    //   be using it, can we disable its repeated calling by Arduino? Does FreeRTOS offer this?
    //TaskFunction_t dynamic_blink_cycle_task();
    //TaskFunction_t simple_blink_cycle_task();

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
 * Docs relevant to getting the initial FreeRTOS basics working:
 * Helpful forum posting on needing to pass (void *) as arguments to some functions passed to xTaskCreate():
 * https://github.com/espressif/arduino-esp32/issues/1414
 *
 * xTaskCreate Reference:
 * https://www.freertos.org/a00125.html
 *
 *
 *
 *
 *
 */


/**/
