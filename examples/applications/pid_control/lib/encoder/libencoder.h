/**
 * @file libencoder.h
 * @author Eirik Haustveit (eirik@haustveit.net)
 * @brief Rotary incremental encoder library
 * @version 0.1
 * @date 2024-03-29
 *
 * @copyright Copyright (c) 2024
 *
 *
 */

#ifndef _LIBENCODER_H
#define _LIBENCODER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

#define ENCODER_RANGE 65535

typedef bool (*getBntStateCb_t)();
typedef uint32_t (*getTimerCntCb_t)();

typedef struct _encoderBtn_t {
    getBntStateCb_t getBtnState;
    uint16_t clickCnt;
    bool isPressed;
    bool isReleased;
    bool isHeldDown;
    uint8_t btnAsserted;
    uint8_t btnPrevious;
} libencoder_encoderBtn_t;

typedef struct _encoder_t {
    libencoder_encoderBtn_t *encoderBtn;
    getTimerCntCb_t getTimerCnt;
    uint32_t currentCnt;
    uint32_t prevCnt;
    bool isTurnedLeft;
    bool isTurnedRight;
    bool isTurned;
} libencoder_encoder_t;

void libencoder_init(libencoder_encoder_t *encoder, libencoder_encoderBtn_t *encoderBtn, getBntStateCb_t getBtnState, getTimerCntCb_t getTimerCnt);

/**
 * @brief Poll the encoder timer status
 *
 * This function should be called at fixed intervals. E.g. every millisecond.
 *
 * @param encoder The encoder instance.
 */
void libencoder_poll(libencoder_encoder_t *encoder);

bool libencoder_isEncTurnedLeft(libencoder_encoder_t *encoder);

bool libencoder_isEncTurnedRight(libencoder_encoder_t *encoder);

bool libencoder_isEncTurned(libencoder_encoder_t *encoder);

uint32_t libencoder_getEncCount(libencoder_encoder_t *encoder);

bool libencoder_isBtnPressed(libencoder_encoder_t *encoder);
bool libencoder_isBtnReleased(libencoder_encoder_t *encoder);
bool libencoder_isBtnHeldDown(libencoder_encoder_t *encoder);


#endif /* _LIBENCODER_H */
