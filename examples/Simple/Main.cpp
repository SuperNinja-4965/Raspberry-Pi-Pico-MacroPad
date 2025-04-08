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

// Standard library includes.
#include "pico/stdlib.h"
// Include our Macropad Library
#include "RGBMacroLibrary.hpp"

//------------------------------------------------------------------------------------------------------------------------+
// Useful info:
// The RGBMacroLibrary uses 2 of the RP2040's timers.
// To find out what keys work you can look here: https://github.com/hathach/tinyusb/blob/master/src/class/hid/hid.h
// Any keys in the REPORT_ID_KEYBOARD and REPORT_ID_CONSUMER_CONTROL devices will work.
//------------------------------------------------------------------------------------------------------------------------+

RGBMacroPad MacroPad;

// Code enters here.
int main()
{
    // False disables the blinking task, 300000 is the sleep timer duration in ms.
    MacroPad.DimLedDuration = 300000;
    MacroPad.UseBlinking = false;
    // Ready the device.
    MacroPad.InitializeDevice();

    // Setup our keys These are just the keys I personally use.
    MacroPad.SetupButton(0, 0x20, 0x00, 0x20, HID_KEY_VOLUME_UP, 0, REPORT_ID_KEYBOARD);
    MacroPad.SetupButton(1, 0x00, 0x20, 0x00, HID_KEY_S, KEYBOARD_MODIFIER_LEFTGUI + KEYBOARD_MODIFIER_LEFTCTRL + KEYBOARD_MODIFIER_LEFTSHIFT + KEYBOARD_MODIFIER_LEFTALT, REPORT_ID_KEYBOARD);
    MacroPad.SetupButton(2, 0x20, 0x00, 0x20, HID_USAGE_CONSUMER_PLAY_PAUSE, 0, REPORT_ID_CONSUMER_CONTROL);
    MacroPad.SetupButton(3, 0x20, 0x00, 0x00, HID_KEY_DELETE, KEYBOARD_MODIFIER_LEFTCTRL + KEYBOARD_MODIFIER_LEFTALT, REPORT_ID_KEYBOARD);

    MacroPad.SetupButton(4, 0x20, 0x00, 0x20, HID_KEY_VOLUME_DOWN, 0, REPORT_ID_KEYBOARD);
    MacroPad.SetupButton(5, 0x20, 0x00, 0x00, HID_KEY_H, KEYBOARD_MODIFIER_LEFTGUI + KEYBOARD_MODIFIER_LEFTCTRL + KEYBOARD_MODIFIER_LEFTSHIFT + KEYBOARD_MODIFIER_LEFTALT, REPORT_ID_KEYBOARD);
    MacroPad.SetupButton(6, 0x20, 0x00, 0x20, HID_USAGE_CONSUMER_SCAN_PREVIOUS, 0, REPORT_ID_CONSUMER_CONTROL);
    MacroPad.SetupButton(7, 0x20, 0x00, 0x20, HID_USAGE_CONSUMER_SCAN_NEXT, 0, REPORT_ID_CONSUMER_CONTROL);

    MacroPad.SetupButton(8, 0x20, 0x00, 0x20, HID_KEY_MUTE, 0, REPORT_ID_KEYBOARD);
    MacroPad.SetupButton(9, 0x00, 0x20, 0x20, HID_KEY_A, KEYBOARD_MODIFIER_LEFTGUI + KEYBOARD_MODIFIER_LEFTCTRL + KEYBOARD_MODIFIER_LEFTSHIFT + KEYBOARD_MODIFIER_LEFTALT, REPORT_ID_KEYBOARD);
    MacroPad.SetupButton(10, 0x00, 0x00, 0x20, HID_KEY_D, KEYBOARD_MODIFIER_LEFTGUI + KEYBOARD_MODIFIER_LEFTCTRL + KEYBOARD_MODIFIER_LEFTSHIFT + KEYBOARD_MODIFIER_LEFTALT, REPORT_ID_KEYBOARD);
    MacroPad.SetupButton(11, 0x20, 0x00, 0x00, 0, 0, REPORT_ID_TINYPICO);

    MacroPad.SetupButton(12, 0x00, 0x00, 0x00, HID_KEY_CAPS_LOCK, 0, REPORT_ID_KEYBOARD);
    MacroPad.SetupButton(13, 0x00, 0x00, 0x00, HID_KEY_NUM_LOCK, 0, REPORT_ID_KEYBOARD);
    // Uncomment this line if you want to use scroll lock. Comment the line below if you don't want to use it. (dont forget to update the line below)
    // SetupButton(14, 0x00, 0x00, 0x00, HID_KEY_SCROLL_LOCK, 0, REPORT_ID_KEYBOARD);
    MacroPad.SetupButton(14, 0x00, 0x20, 0x00, HID_KEY_T, KEYBOARD_MODIFIER_LEFTCTRL + KEYBOARD_MODIFIER_LEFTALT, REPORT_ID_KEYBOARD);
    MacroPad.SetupButton(15, 0x00, 0x20, 0x20, HID_KEY_L, KEYBOARD_MODIFIER_LEFTGUI, REPORT_ID_KEYBOARD);
    // MacroPad.RemoveButtonSetup(ButtonNum); // Use this to remove a buttons config.

    while (true)
    {
        // Run the libraries loop to handle keypresses.
        MacroPad.Loop();
    }

    return 0;
}
