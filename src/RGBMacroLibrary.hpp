/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 SuperNinja_4965
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef RGBMacroPad_H
#define RGBMarcoPad_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
// Standard library includes.
#include "pico/stdlib.h"
// Include TinyUSB libraries.
#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"

// We are including this for a button option later.
#include "pico/bootrom.h"

// Include the RGB Keypad library.
#include "pico_rgb_keypad.hpp"

#define REPORT_ID_TINYPICO 4

class RGBMacroPad
{
public:
    // Variable to define if we are blinking the led or not.
    bool UseBlinking = false;
    // Variable to store the Dim LED duration
    int DimLedDuration = 300000;
    // Define our shared functions
    void SetupButton(uint8_t ButtonNum, uint8_t r, uint8_t g, uint8_t b, uint8_t KeyCode, uint8_t ModifierKeys, uint8_t KeyboardType);
    void RemoveButtonSetup(int ButtonNum);
    void InitializeDevice();
    void Loop(void);
    void SendKeypress(uint8_t report_id, uint8_t KeyCode, uint8_t Modifiers);

private:
    static int64_t ResetLEDsRepeat(alarm_id_t id, void *user_data);
    static bool DimLEDTimer(struct repeating_timer *t);
};

#endif