
#include "libencoder.h"

void libencoder_init(libencoder_encoder_t *encoder, libencoder_encoderBtn_t *encoderBtn, getBntStateCb_t getBtnState, getTimerCntCb_t getTimerCnt){

    encoder->getTimerCnt = getTimerCnt;

    encoder->currentCnt = 0;
    encoder->prevCnt = 0;
    encoder->isTurned = false;
    encoder->isTurnedLeft = false;
    encoder->isTurnedRight = false;

    if(encoderBtn != NULL){
        encoder->encoderBtn = encoderBtn;
        encoder->encoderBtn->getBtnState = getBtnState;
        encoder->encoderBtn->clickCnt = 0;
        encoder->encoderBtn->isHeldDown = false;
        encoder->encoderBtn->isPressed = false;
        encoder->encoderBtn->isReleased = false;

        encoder->encoderBtn->btnAsserted = 0;
        encoder->encoderBtn->btnPrevious = 0;
    }
}

void libencoder_poll(libencoder_encoder_t *encoder){

    encoder->currentCnt = encoder->getTimerCnt();

    if(encoder->currentCnt != encoder->prevCnt){ // Detect a change in count value

        /*
         * TODO:
         * The timer is 16-bit and hence counts from 0 to 65535.
         * Rotating left increase the count while rotating right decrease the count.
         * The timer starts at zero, and hence rotating right will roll over to 65535.
         * This edge case must be handled to avoid glitching.
         * Setting the timer to some value in the middle is not a solution, since the
         * user eventually might reach rollover anyway.
         */
        if(encoder->currentCnt > encoder->prevCnt){ // Determine which way the count has changed
        //if(((int32_t)encoder->prevCnt - (int32_t)encoder->currentCnt) < 0){ // Determine which way the count has changed

            encoder->isTurnedLeft = true;
            encoder->isTurnedRight = false;
        }
        else{

            encoder->isTurnedLeft = false;
            encoder->isTurnedRight = true;
        }

        encoder->isTurned = true;

        encoder->prevCnt = encoder->currentCnt; // Update the previous count value
    }

    if(encoder->encoderBtn != NULL){

        uint8_t btnState = encoder->encoderBtn->getBtnState(); // Read the digital input pin

        uint8_t tmp = encoder->encoderBtn->btnAsserted; // Save the assertion state

        // The idea is that this code (the poll function) is called with a time interval
        // The following debounce algorithm will work if the interval is sufficiently long
        encoder->encoderBtn->btnAsserted |= (encoder->encoderBtn->btnPrevious & btnState); // Set asserted if previous and current is high
        encoder->encoderBtn->btnAsserted &= (encoder->encoderBtn->btnPrevious | btnState); // Clear asserted if previous and current is low, the or operator ensures that both previous and current must be low.

        encoder->encoderBtn->btnPrevious = btnState; // Save the current digital input state so we are ready for the next iteration

        tmp ^= encoder->encoderBtn->btnAsserted; // Detect a change in the assertion state

        if(tmp){
            if(encoder->encoderBtn->btnAsserted){
                encoder->encoderBtn->isHeldDown = true;

                encoder->encoderBtn->isPressed = true; // Save that the button has been pressed
            }
            else{
                encoder->encoderBtn->isHeldDown = false;

                encoder->encoderBtn->isReleased = true; // Save that the button has been released
            }
        }
    }
}

bool libencoder_isEncTurnedLeft(libencoder_encoder_t *encoder){

    bool isEncTurnedLeft = encoder->isTurnedLeft;
    encoder->isTurnedLeft = false;
    return isEncTurnedLeft;
}

bool libencoder_isEncTurnedRight(libencoder_encoder_t *encoder){

    bool isEncTurnedRight = encoder->isTurnedRight;
    encoder->isTurnedRight = false;
    return isEncTurnedRight;
}

bool libencoder_isEncTurned(libencoder_encoder_t *encoder){

    bool isEncTurned = encoder->isTurned;
    encoder->isTurned = false;
    return isEncTurned;
}

uint32_t libencoder_getEncCount(libencoder_encoder_t *encoder){
    return encoder->currentCnt;
}

bool libencoder_isBtnPressed(libencoder_encoder_t *encoder){

    bool isPressed = encoder->encoderBtn->isPressed;
    encoder->encoderBtn->isPressed = false;
    return isPressed;
}

bool libencoder_isBtnReleased(libencoder_encoder_t *encoder){

    bool isReleased = encoder->encoderBtn->isReleased;
    encoder->encoderBtn->isReleased = false;
    return isReleased;
}

bool libencoder_isBtnHeldDown(libencoder_encoder_t *encoder){

    return encoder->encoderBtn->isHeldDown;
}
