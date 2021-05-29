#ifndef CUSTOMCHARS_H
#define CUSTOMCHARS_H

// ########    CUSTOM CHARACTER DEFINITIONS (ICONS)   ########

int CHR_SELECT = 1;
int CHR_UP = 2;
int CHR_DOWN = 3;
int CHR_LEFT = 4;
int CHR_RIGHT = 5;
int CHR_RESET = 6;

// Some additional characters from some example code. Notice this is uint8_t arr[8] instead of byte arr[8].
// TODO: See what the difference is? This is from the example code for the LiquidCrystal library by F Malpartida.
//uint8_t bell[8]  = {0x4,0xe,0xe,0xe,0x1f,0x0,0x4};
//uint8_t note[8]  = {0x2,0x3,0x2,0xe,0x1e,0xc,0x0};
//uint8_t clock[8] = {0x0,0xe,0x15,0x17,0x11,0xe,0x0};
//uint8_t heart[8] = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};
//uint8_t duck[8]  = {0x0,0xc,0x1d,0xf,0xf,0x6,0x0};
//uint8_t check[8] = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0};
//uint8_t cross[8] = {0x0,0x1b,0xe,0x4,0xe,0x1b,0x0};
//uint8_t retarrow[8] = {0x1,0x1,0x5,0x9,0x1f,0x8,0x4};


byte c_up[8] = {
        B00000,
        B00100,
        B01110,
        B11111,
        B01110,
        B01110,
        B01110,
        B00000,
};

byte c_down[8] = {
        B00000,
        B01110,
        B01110,
        B01110,
        B11111,
        B01110,
        B00100,
        B00000,
};

byte c_left[8] = {
        B00000,
        B00100,
        B01111,
        B11111,
        B01111,
        B00100,
        B00000,
        B00000,
};

byte c_right[8] = {
        B00000,
        B00100,
        B11110,
        B11111,
        B11110,
        B00100,
        B00000,
        B00000,
};

byte c_select[8] = {
        B00000,
        B01110,
        B11111,
        B11111,
        B11111,
        B11111,
        B01110,
        B00000,
};

byte c_reset[8] = {
        B00000,
        B01110,
        B10001,
        B10101,
        B10101,
        B10001,
        B01110,
        B00000,
};

#endif


/**/
