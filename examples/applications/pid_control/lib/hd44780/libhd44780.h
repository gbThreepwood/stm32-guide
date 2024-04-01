/*
   ----------------------------------------------------------------------
    Copyright (C) Tilen MAJERLE, 2015

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------

 * Adapted to fit my project. Eirik Haustveit, 2024.
 * The original source is at: https://github.com/MaJerle/stm32f429
 *
 * I prefer a library which is not tied to the GPIO
 * functions of my specific MCU. E.g. something which required
 * the user to implement some callback functions, but I could not
 * find anything in a quick google search. Thus I opted to modify this
 * library to add that functionality.
 *
 * Another advantage is that it is much more easy to have two instances
 * of this version of the library.
 *
 * The original library relies on the use of blocking delays to ensure
 * that the communication with the display is not too fast. This is not
 * acceptable for me. Instead a active object in the QPC framework is
 * created to perform this task.
*/
#ifndef LIBHD44780_H
#define LIBHD44780_H

//#include <stm32l4xx_ll_gpio.h>
#include <stdint.h>
#include <stdbool.h>

typedef void (*libHD44780_setGPIOPinCb_t)(uint8_t data, bool rs, bool rw);
typedef void (*libHD44780_setEnableCb_t)(bool en);
//typedef uint16_t (*libHD44780_getGPIOPinCb_t)();

/* Private libHD44780 structure */
typedef struct {
   libHD44780_setGPIOPinCb_t setGPIOPinCb;
   libHD44780_setEnableCb_t setEnableCb;
   uint8_t displayControl;
   uint8_t displayFunction;
   uint8_t displayMode;
   uint8_t rows;
   uint8_t cols;
   uint8_t currentX;
   uint8_t currentY;
} libHD44780_ctx_t;


/**
 * @brief  Initializes HD44780 LCD
 * @brief  cols: width of lcd
 * @param  rows: height of lcd
 * @retval None
 */
void libHD44780_Init(libHD44780_ctx_t *ctx, libHD44780_setGPIOPinCb_t setGPIOPinCb, libHD44780_setEnableCb_t setEnableCb, uint8_t cols, uint8_t rows);

/**
 * @brief  Turn display on
 * @param  None
 * @retval None
 */
void libHD44780_DisplayOn(libHD44780_ctx_t *ctx);

/**
 * @brief  Turn display off
 * @param  None
 * @retval None
 */
void libHD44780_DisplayOff(libHD44780_ctx_t *ctx);

/**
 * @brief  Clears entire LCD
 * @param  None
 * @retval None
 */
void libHD44780_Clear(libHD44780_ctx_t *ctx);

/**
 * @brief  Puts string on lcd
 * @param  x location
 * @param  y location
 * @param  *str: pointer to string to display
 * @retval None
 */
void libHD44780_Puts(libHD44780_ctx_t *ctx, uint8_t x, uint8_t y, char* str);

/**
 * @brief  Enables cursor blink
 * @param  None
 * @retval None
 */
void libHD44780_BlinkOn(libHD44780_ctx_t *ctx);

/**
 * @brief  Disables cursor blink
 * @param  None
 * @retval None
 */
void libHD44780_BlinkOff(libHD44780_ctx_t *ctx);

/**
 * @brief  Shows cursor
 * @param  None
 * @retval None
 */
void libHD44780_CursorOn(libHD44780_ctx_t *ctx);

/**
 * @brief  Hides cursor
 * @param  None
 * @retval None
 */
void libHD44780_CursorOff(libHD44780_ctx_t *ctx);

/**
 * @brief  Scrolls display to the left
 * @param  None
 * @retval None
 */
void libHD44780_ScrollLeft(libHD44780_ctx_t *ctx);

/**
 * @brief  Scrolls display to the right
 * @param  None
 * @retval None
 */
void libHD44780_ScrollRight(libHD44780_ctx_t *ctx);

/**
 * @brief  Creates custom character
 * @param  location: Location where to save character on LCD. LCD supports up to 8 custom characters, so locations are 0 - 7
 * @param *data: Pointer to 8-bytes of data for one character
 * @retval None
 */
void libHD44780_CreateChar(libHD44780_ctx_t *ctx, uint8_t location, uint8_t* data);

/**
 * @brief  Puts custom created character on LCD
 * @param  location: Location on LCD where character is stored, 0 - 7
 * @retval None
 */
void libHD44780_PutCustom(libHD44780_ctx_t *ctx, uint8_t x, uint8_t y, uint8_t location);


#endif /* LIBHD44780_H */
