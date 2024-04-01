/*
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen MAJERLE, 2014
 * |
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * |
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 *
 * Adapted to fit my project. Eirik Haustveit, 2024.
 * The original source is at: https://github.com/MaJerle/stm32f429
 */

#include <stm32l4xx_ll_utils.h>

#include "libhd44780.h"

/* Private functions */
static void libHD44780_Cmd(libHD44780_ctx_t *ctx, uint8_t cmd);
static void libHD44780_Data(libHD44780_ctx_t *ctx, uint8_t data);
static void libHD44780_CursorSet(libHD44780_ctx_t *ctx, uint8_t col, uint8_t row);
static void libHD44780_Cmd4bit(libHD44780_ctx_t *ctx, uint8_t cmd, bool rs);

/* Private variable */
//static libHD44780_Options_t libHD44780_Opts;

/* Pin definitions */
//#define HD44780_RS_LOW              TM_GPIO_SetPinLow(HD44780_RS_PORT, HD44780_RS_PIN)
//#define HD44780_RS_HIGH             TM_GPIO_SetPinHigh(HD44780_RS_PORT, HD44780_RS_PIN)
//#define HD44780_E_LOW               TM_GPIO_SetPinLow(HD44780_E_PORT, HD44780_E_PIN)
//#define HD44780_E_HIGH              TM_GPIO_SetPinHigh(HD44780_E_PORT, HD44780_E_PIN)

//#define HD44780_E_BLINK             HD44780_E_HIGH; libHD44780_Delay(20); HD44780_E_LOW; libHD44780_Delay(20)
#define libHD44780_Delay(x)           LL_mDelay(x);

/* Commands*/
#define HD44780_CLEARDISPLAY        0x01
#define HD44780_RETURNHOME          0x02
#define HD44780_ENTRYMODESET        0x04
#define HD44780_DISPLAYCONTROL      0x08
#define HD44780_CURSORSHIFT         0x10
#define HD44780_FUNCTIONSET         0x20
#define HD44780_SETCGRAMADDR        0x40
#define HD44780_SETDDRAMADDR        0x80

/* Flags for display entry mode */
#define HD44780_ENTRYRIGHT          0x00
#define HD44780_ENTRYLEFT           0x02
#define HD44780_ENTRYSHIFTINCREMENT 0x01
#define HD44780_ENTRYSHIFTDECREMENT 0x00

/* Flags for display on/off control */
#define HD44780_DISPLAYON           0x04
#define HD44780_CURSORON            0x02
#define HD44780_BLINKON             0x01

/* Flags for display/cursor shift */
#define HD44780_DISPLAYMOVE         0x08
#define HD44780_CURSORMOVE          0x00
#define HD44780_MOVERIGHT           0x04
#define HD44780_MOVELEFT            0x00

/* Flags for function set */
#define HD44780_8BITMODE            0x10
#define HD44780_4BITMODE            0x00
#define HD44780_2LINE               0x08
#define HD44780_1LINE               0x00
#define HD44780_5x10DOTS            0x04
#define HD44780_5x8DOTS             0x00

void libHD44780_Init(libHD44780_ctx_t *ctx, libHD44780_setGPIOPinCb_t setGPIOPinCb, libHD44780_setEnableCb_t setEnableCb, uint8_t cols, uint8_t rows){

    ctx->setGPIOPinCb = setGPIOPinCb;
    ctx->setEnableCb = setEnableCb;

    ///* Initialize delay */
    //TM_DELAY_Init();

    ///* Init pinout */
    //TM_HD44780_InitPins();

    ///* At least 40ms */
    libHD44780_Delay(45);

    /* Set LCD width and height */
    ctx->rows = rows;
    ctx->cols = cols;

    /* Set cursor pointer to beginning for LCD */
    ctx->currentX = 0;
    ctx->currentY = 0;

    ctx->displayFunction = HD44780_4BITMODE | HD44780_5x8DOTS | HD44780_1LINE;
    if (rows > 1) {
        ctx->displayFunction |= HD44780_2LINE;
    }

    /* Try to set 4bit mode */
    libHD44780_Cmd4bit(ctx, 0x03, false);
    libHD44780_Delay(45);

    /* Second try */
    libHD44780_Cmd4bit(ctx, 0x03, false);
    libHD44780_Delay(45);

    /* Third goo! */
    libHD44780_Cmd4bit(ctx, 0x03, false);
    libHD44780_Delay(45);

    /* Set 4-bit interface */
    libHD44780_Cmd4bit(ctx, 0x02, false);
    libHD44780_Delay(10);

    /* Set # lines, font size, etc. */
    libHD44780_Cmd(ctx, HD44780_FUNCTIONSET | ctx->displayFunction);

    /* Turn the display on with no cursor or blinking default */
    ctx->displayControl = HD44780_DISPLAYON;
    libHD44780_DisplayOn(ctx);

    /* Clear lcd */
    libHD44780_Clear(ctx);

    /* Default font directions */
    ctx->displayMode = HD44780_ENTRYLEFT | HD44780_ENTRYSHIFTDECREMENT;
    libHD44780_Cmd(ctx, HD44780_ENTRYMODESET | ctx->displayMode);

    /* Delay */
    libHD44780_Delay(4500);
}

void libHD44780_Clear(libHD44780_ctx_t *ctx) {
    libHD44780_Cmd(ctx, HD44780_CLEARDISPLAY);
    libHD44780_Delay(3000);
}

void libHD44780_Puts(libHD44780_ctx_t *ctx, uint8_t x, uint8_t y, char* str) {

    libHD44780_CursorSet(ctx, x, y);

    while (*str) {
        if (ctx->currentX >= ctx->cols) {
            ctx->currentX = 0;
            ctx->currentY++;
            libHD44780_CursorSet(ctx, ctx->currentX, ctx->currentY);
        }
        if (*str == '\n') {
            ctx->currentY++;
            libHD44780_CursorSet(ctx, ctx->currentX, ctx->currentY);
        } else if (*str == '\r') {
            libHD44780_CursorSet(ctx, 0, ctx->currentY);
        } else {
            libHD44780_Data(ctx, *str);
            ctx->currentX++;
        }
        str++;
    }
}

void libHD44780_DisplayOn(libHD44780_ctx_t *ctx) {
    ctx->displayControl |= HD44780_DISPLAYON;
    libHD44780_Cmd(ctx, HD44780_DISPLAYCONTROL | ctx->displayControl);
}

void libHD44780_DisplayOff(libHD44780_ctx_t *ctx) {
    ctx->displayControl &= ~HD44780_DISPLAYON;
    libHD44780_Cmd(ctx, HD44780_DISPLAYCONTROL | ctx->displayControl);
}

void libHD44780_BlinkOn(libHD44780_ctx_t *ctx) {
    ctx->displayControl |= HD44780_BLINKON;
    libHD44780_Cmd(ctx, HD44780_DISPLAYCONTROL | ctx->displayControl);
}

void libHD44780_BlinkOff(libHD44780_ctx_t *ctx) {
    ctx->displayControl &= ~HD44780_BLINKON;
    libHD44780_Cmd(ctx, HD44780_DISPLAYCONTROL | ctx->displayControl);
}

void libHD44780_CursorOn(libHD44780_ctx_t *ctx) {
    ctx->displayControl |= HD44780_CURSORON;
    libHD44780_Cmd(ctx, HD44780_DISPLAYCONTROL | ctx->displayControl);
}

void libHD44780_CursorOff(libHD44780_ctx_t *ctx) {
    ctx->displayControl &= ~HD44780_CURSORON;
    libHD44780_Cmd(ctx, HD44780_DISPLAYCONTROL | ctx->displayControl);
}

void libHD44780_ScrollLeft(libHD44780_ctx_t *ctx) {
    libHD44780_Cmd(ctx, HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVELEFT);
}

void libHD44780_ScrollRight(libHD44780_ctx_t *ctx) {
    libHD44780_Cmd(ctx, HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVERIGHT);
}

void libHD44780_CreateChar(libHD44780_ctx_t *ctx, uint8_t location, uint8_t* data) {
    uint8_t i;
    /* We have 8 locations available for custom characters */
    location &= 0x07;
    libHD44780_Cmd(ctx, HD44780_SETCGRAMADDR | (location << 3));

    for (i = 0; i < 8; i++) {
        libHD44780_Data(ctx, data[i]);
    }
}

void libHD44780_PutCustom(libHD44780_ctx_t *ctx, uint8_t x, uint8_t y, uint8_t location) {
    libHD44780_CursorSet(ctx, x, y);
    libHD44780_Data(ctx, location);
}

/* Private functions */
static void libHD44780_Cmd(libHD44780_ctx_t *ctx, uint8_t cmd) {
    /* Command mode */

    /* High nibble */
    libHD44780_Cmd4bit(ctx, cmd >> 4, false);
    /* Low nibble */
    libHD44780_Cmd4bit(ctx, cmd & 0x0F, false);
}

static void libHD44780_Cmd4bit(libHD44780_ctx_t *ctx, uint8_t cmd, bool rs) {

    /* Set output port */
    ctx->setGPIOPinCb(cmd, rs, false);

    ctx->setEnableCb(true);
    libHD44780_Delay(10);
    ctx->setEnableCb(false);
    libHD44780_Delay(10);

}

static void libHD44780_Data(libHD44780_ctx_t *ctx, uint8_t data) {
    /* Data mode */

    /* High nibble */
    libHD44780_Cmd4bit(ctx, data >> 4, true);
    /* Low nibble */
    libHD44780_Cmd4bit(ctx, data & 0x0F, true);
}

static void libHD44780_CursorSet(libHD44780_ctx_t *ctx, uint8_t col, uint8_t row) {
    uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};

    /* Go to beginning */
    if (row >= ctx->rows) {
        row = 0;
    }

    /* Set current column and row */
    ctx->currentX = col;
    ctx->currentY = row;

    /* Set location address */
    libHD44780_Cmd(ctx, HD44780_SETDDRAMADDR | (col + row_offsets[row]));
}
