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
const int DELAY_DELTA = 10;  // The delta value in milliseconds by which blink_delay is either incremented or decremented.

int blink_delay = 10;  // Blink delay in milliseconds. blink_delay will change dynamically. Starting value.

const int DELAY_MIN = 10;  // Minimum blink_delay value in ms before the delta_dir will change to ascending (1).

const int DELAY_MAX = 140;  // Maximum blink_delay value in ms before the delta_dir will change to descending (0).

int delta_dir = 1;  // Direction for blink_delay increment/decrement: 1 up (add), 0 down (subtract)
// Up/increment/add means the delays get longer. Down/decrement/subtract mean the delays get shorter.
// The same delay value is used for LED <ON> time as for the <OFF> time, which are two different delay events.

const int LED_PIN = 13;

const int MILLISECONDS_PER_TICK = portTICK_PERIOD_MS;  // So: ticks = milliseconds / MILLISECONDS_PER_TICK
// Ticks will be needed for vTaskDelay() and likely elsewhere.


/******** ROM Character Demo ********/

// The ROM Characters can sort of be separated in to blocks of 16 characters which approximately divides different
// sections of the full range of slots that may or may not contain factory-programmed ROM characters. Also note,
// LCD Keypad shields from different vendors and/or for different countries may come with different factory-programmed
// ROM character sets. These characters cannot be changed, however there is a way to make your own custom characters.
// On my Velleman VMA203 LCD Keypad Shield, I have what I think is the most common set of European and Japanese Kanji
// character sets, including some additional symbols. There are some 16 character blocks that have no characters
// defined. A configuration for the character demo which shows ALL possible characters, meaning all possible blocks,
// would be this: {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}. However, I am eliminating the blocks with no
// characters configured and some of the Kanji blocks, since I will not use any of those characters and the demo
// is more about FreeRTOS and simultaneity than about available ROM characters.
// Max possible array size is 16. Max possible block number is 15 as a 0-based index is used.
// Blocks removed from demo: Blank: 0,1,8,9  and  Kanji: 11, 12, 13
// Blocks 2, 3, 4, 5, 6 and 7 cover European Alphanumerics with most standard keyboard symbols included.
// Blocks 10, 14 and 15 include some additional scientific and math symbols, accented letters and a few Kanji.

const int rom_char_demo_blocks[9] = {2, 3, 4, 5, 6, 7, 10, 14, 15};


/******** LCD Keypad ********/

/* NOTE: For the configuration of LCD Keypad hardware, there is a small chance of variance between different
 * hardware vendors. You should check with the reference documentation for your specific hardware.
 * My LCD Keypad Shield is made to attach to an Arduino UNO or Mega 2560 and is the Velleman VMA203 LCD1602.
 * The pin configuration for the LCD module for the LiquidCrystal library is:
 * LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
 * WARNING: Some LCD Keypads allow pin control of the backlight for on/off/brightness. NEVER USE THIS FEATURE. Nearly
 * all LCD Keypad shields that allow pin control of the backlight have a hardware flaw. (On many such shields,
 * this is on pin 10.) If you ever set the designated pin to OUTPUT in order to use this feature, you will most likely
 * burn out your Arduino voltage regulator. Do NOT use this feature. Always leave this pin in INPUT mode.
 * Your LCD backlight will simply stay on at full brightness.
 * */

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


/******** FreeRTOS Task Function Prototypes ********/

// Function prototypes (Is this just a FreeRTOS thing or when else would we do this?)
// TODO: note on void args (The FreeRTOS book/examples/tutorials on the main site should help answer this.)
// In general, regarding function prototypes in CPP (source: Google):
// The function prototypes are used to tell the compiler about the number of arguments and about the required
// datatypes of a function parameter, it also tells about the return type of the function. By this information,
// the compiler cross-checks the function signatures before calling it.
// TODO: UPDATE: Discovered that it is not strictly required. I accidentally got a new task working without the
// function prototype for it in place. Coding best practice says still use them for sure as it helps the compiler
// and more but it is not a requirement for FreeRTOS, as it was sort of made to seem to be in some info I was
// following.
[[noreturn]] void dynamic_blink_cycle_task(void *pvParameters);
[[noreturn]] void simple_blink_cycle_task(void *pvParameters);
[[noreturn]] void rom_characters_demo_task(void *pvParameters);

/**
 * Good info on function prototypes I found:
 *
 * With the declaration of a function the compiler can check the consistent use of parameters and return value,
 * and can compile the code even if the function is not implemented in this module. If the function is only declared
 * but not implemented in the respective module, this gap will be closed by the linker, not the compiler.
 * It's similar to declaring extern variables. If you'd define them, the memory for them would be allocated multiple
 * times. That's why you should never define variables in h-files, but declare them there. Including the h-file
 * would result in multiple allocations of memory.
 * FROM 3 or 4 postings down on:
 * https://stackoverflow.com/questions/21670671/why-use-function-prototypes
 * */


/************************************************ FUNCTION DEFINITIONS ************************************************/

// Application-specific functions go here, where they must be defined before possible usage in setup() or loop().

[[noreturn]] void log_info_task(void *pvParameters) {

} /* log_info_task */


[[noreturn]] void dynamic_blink_cycle_task(void *pvParameters) {
    // See simple_blink_cycle_task() below for comments on [[noreturn]], used to suppress CLang-Tidy endless loop warn.

    while (true) {
        TickType_t blink_ticks;  // Will hold calculated tick value before each call to vTaskDelay().
        // TODO: Determine best practice here. cppcheck told me I could reduce scope of blink_ticks, but to me this
        // looks like unnecessary repeated attempts to re-create/initialize the variable which I realize is probably
        // just a warning in most languages, but according to cppcheck, it is no problem here and was supposedly at
        // too high a scope just up outside of this while. Is this situation (with a loop) for var init different
        // between cpp and perl? This definitely deserves clarification for both languages.
        // TODO: ABOVE ANSWERED: At least for CPP, declaring the vars INSIDE the loop (when possible) IS in fact
        // BEST PRACTICE. The compiler only allocates once and is smart about this being a loop. The code check is
        // correct to recommend moving this declaration from just up/before and outside of this loop to inside
        // of it.
        // https://stackoverflow.com/questions/7959573/declaring-variables-inside-loops-good-practice-or-bad-practice
        // TODO: Research this same topic for Python, and for arguments sake, since Java is a well-designed and formal
        // object-oriented language with lots of rules and reasons for doing things .. let's see how the issue is
        // addressed for Java as well.

        digitalWrite(LED_PIN, HIGH);
        blink_ticks = blink_delay / MILLISECONDS_PER_TICK;
        vTaskDelay(blink_ticks);
        digitalWrite(LED_PIN, LOW);
        blink_ticks = blink_delay / MILLISECONDS_PER_TICK;
        vTaskDelay(blink_ticks);

        if (delta_dir == 0) {
            blink_delay = blink_delay - DELAY_DELTA;
        } else {
            blink_delay = blink_delay + DELAY_DELTA;
        }

        if (blink_delay < DELAY_MIN) {
            delta_dir = 1;
            blink_delay = DELAY_MIN;
        }
        if (blink_delay > DELAY_MAX) {
            delta_dir = 0;
            blink_delay = DELAY_MAX;
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
    TickType_t blink_ticks = 500 / MILLISECONDS_PER_TICK;

    while (true) {
        digitalWrite(LED_PIN, HIGH);
        vTaskDelay(blink_ticks);
        digitalWrite(LED_PIN, LOW);
        vTaskDelay(blink_ticks);
    }
} /* simple_blink_cycle_task() */


[[noreturn]] void rom_characters_demo_task(void *pvParameters) {
    TickType_t char_pause_ticks = 100 / MILLISECONDS_PER_TICK;
    TickType_t scroll_pause_ticks = 1000 / MILLISECONDS_PER_TICK;


//    ATTEMPT AT CONFIGURABLE ROM CHARACTER DEMO BLOCKS - NOT YET WORKING
//    int demo_blocks_count = sizeof(rom_char_demo_blocks);
//
//    for (int i = 0; i < demo_blocks_count; i++) {
//        int current_block = rom_char_demo_blocks[i];
//        lcd.clear();
//        lcd.print("Codes 0x"); lcd.print(current_block, HEX);
//        lcd.print("-0x"); lcd.print(current_block + 15, HEX);
//        lcd.setCursor(0, 1);
//        for (int j = 0; j < 16; j++) {
//            lcd.write(current_block + j);
//            // Slow down character printing to simulate typing and make demo more interesting.
//            vTaskDelay(char_pause_ticks);
//        }
//        vTaskDelay(scroll_pause_ticks);
//    }

    uint8_t i = 0;
    while (1) {
        lcd.clear();
        lcd.print("Codes 0x"); lcd.print(i, HEX);
        lcd.print("-0x"); lcd.print(i + 15, HEX);
        lcd.setCursor(0, 1);
        for (int j = 0; j < 16; j++) {
            lcd.write(i + j);
            // Slow down character printing to simulate typing and make demo more interesting.
            vTaskDelay(char_pause_ticks);
        }
        i+=16;
        // Pause on each line of 16 rom characters for long enough for the user to read/inspect them somewhat.
        vTaskDelay(scroll_pause_ticks);
    }

}


/*************************************************** ARDUINO SETUP ****************************************************/

//cppcheck-suppress unusedFunction
void setup() {
    pinMode(LED_PIN, OUTPUT);  // Our LED pin must be set for output.

    // LCD Initialization
    lcd.begin(16,2);
    lcd.clear();

    // Serial will be used for logging or communicating back to a PC/host over USB.
    // BLE (Bluetooth Low Energy) via an HM-10 module is also a good/similar option and also requires Serial.
    Serial.begin(9600);

    // FreeRTOS Initialization

    xTaskCreate(dynamic_blink_cycle_task,
                "DynamicBlinkTask",
                128,
                NULL,
                1,
                NULL);

    xTaskCreate(rom_characters_demo_task,
                "RomCharactersDemoTask",
                128,
                NULL,
                1,
                NULL);

//    xTaskCreate(simple_blink_cycle_task,
//                "SimpleBlinkTask",
//                128,
//                NULL,
//                1,
//                NULL);

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
 * eventually, the use of an LCD Keypad as I would like to develop a nice real-time, non-blocking menu system for
 * Arduino + an LCD Keypad. I think this extremely affordable hardware combo has massive value potential for so many
 * kinds of projects. It all hinges on being non-blocking, in my opinion, because even though the LCD Keypad has
 * great potential for human I/O, this is NOT true if it remains single-threaded and blocking, for pretty-much any
 * kind of project. Yes, you could have a single Arduino running only the LCD Keypad menu I/O system and then
 * communicate with 1 or more other Arduinos or whatever, over serial or circuit logic etc., but there
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
