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

// Include our header
#include "RGBMacroLibrary.hpp"

//--------------------------------------------------------------------+
// Declarations
//--------------------------------------------------------------------+

// This will be used to reference our keypad in our code.
pimoroni::PicoRGBKeypad PicoKeypad;

// We are using this array to store the button assignments the user has setup.
static uint8_t ButtonAssignments[16][3];

// This array is to be used to store button colours so we can reset the colour after the buttonpress
static uint8_t ColourAssignments[16][3];

// Used to avoid calling the functions more than once
uint16_t button_states = 0;
uint16_t last_button_states = 0;
#define MaxBrightness 1.0f
#define MinBrightness 0.2f

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+
/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum
{
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};
static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

// use to avoid send multiple consecutive zero report for keyboard
static bool has_keyboard_key = false;
// use to avoid send multiple consecutive zero report
static bool has_consumer_key = false;
// Used to store the lock keys origional state
uint8_t LockKeysOrigionalColours[3][4];
// Used to store the state of the timer
static bool TimerCancelled;
// used to define the timer and store its state
static struct repeating_timer timer;
static bool LEDDimClock;

// This is all of the tiny usb code with a few modifications pulled from the example at: https://github.com/hathach/tinyusb/blob/master/examples/device/hid_composite/src/main.c
// My code is below.

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void)remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

void RGBMacroPad::SendKeypress(uint8_t report_id, uint8_t KeyCode, uint8_t Modifiers)
{
  // skip if hid is not ready yet
  if (!tud_hid_ready())
    return;

  switch (report_id)
  {
  case REPORT_ID_KEYBOARD:
  {
    if (!has_consumer_key)
    {
      uint8_t keycode[6] = {KeyCode, 0, 0, 0, 0, 0};
      tud_hid_keyboard_report(REPORT_ID_KEYBOARD, Modifiers, keycode);
      has_keyboard_key = true;
    }
  }
  break;

  case REPORT_ID_CONSUMER_CONTROL:
  {
    if (!has_keyboard_key)
    {
      // volume down
      uint16_t ComsumerKeyCode = KeyCode;
      tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &ComsumerKeyCode, 2);
      has_consumer_key = true;
    }
  }
  break;

  default:
    break;
  }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint8_t len)
{
  // we do not use this but im leaving it in to avoid issues
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
  (void)instance;

  if (report_type == HID_REPORT_TYPE_OUTPUT)
  {
    // Set keyboard LED e.g Capslock, Numlock etc...
    if (report_id == REPORT_ID_KEYBOARD)
    {
      // bufsize should be (at least) 1
      if (bufsize < 1)
        return;

      uint8_t const kbd_leds = buffer[0];

      // Set the key colours
      // Check if the key is actually set
      if (LockKeysOrigionalColours[0][3] != 0)
      {
        // Handle Caps Lock
        if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
        {
          // Caps lock is on if the user has a caps lock button light the Button light
          ColourAssignments[LockKeysOrigionalColours[0][3]][0] = 0x20;
          ColourAssignments[LockKeysOrigionalColours[0][3]][1] = 0x20;
          ColourAssignments[LockKeysOrigionalColours[0][3]][2] = 0x20;
        }
        else
        {
          // Caps Lock is not on
          ColourAssignments[LockKeysOrigionalColours[0][3]][0] = LockKeysOrigionalColours[0][0];
          ColourAssignments[LockKeysOrigionalColours[0][3]][1] = LockKeysOrigionalColours[0][1];
          ColourAssignments[LockKeysOrigionalColours[0][3]][2] = LockKeysOrigionalColours[0][2];
        }
        PicoKeypad.illuminate(LockKeysOrigionalColours[0][3], ColourAssignments[LockKeysOrigionalColours[0][3]][0], ColourAssignments[LockKeysOrigionalColours[0][3]][1], ColourAssignments[LockKeysOrigionalColours[0][3]][2]);
      }

      // Check if the key is actually set
      if (LockKeysOrigionalColours[1][3] != 0)
      {
        // Handle Num Lock
        if (kbd_leds & KEYBOARD_LED_NUMLOCK)
        {
          // Numlock is on
          ColourAssignments[LockKeysOrigionalColours[1][3]][0] = 0x20;
          ColourAssignments[LockKeysOrigionalColours[1][3]][1] = 0x20;
          ColourAssignments[LockKeysOrigionalColours[1][3]][2] = 0x20;
        }
        else
        {
          // Numlock is off
          ColourAssignments[LockKeysOrigionalColours[1][3]][0] = LockKeysOrigionalColours[1][0];
          ColourAssignments[LockKeysOrigionalColours[1][3]][1] = LockKeysOrigionalColours[1][1];
          ColourAssignments[LockKeysOrigionalColours[1][3]][2] = LockKeysOrigionalColours[1][2];
        }
        PicoKeypad.illuminate(LockKeysOrigionalColours[1][3], ColourAssignments[LockKeysOrigionalColours[1][3]][0], ColourAssignments[LockKeysOrigionalColours[1][3]][1], ColourAssignments[LockKeysOrigionalColours[1][3]][2]);
      }

      // Check if the key is actually set
      if (LockKeysOrigionalColours[2][3] != 0)
      {
        // Handle Scroll Lock
        if (kbd_leds & KEYBOARD_LED_SCROLLLOCK)
        {
          // Scroll lock is on
          ColourAssignments[LockKeysOrigionalColours[2][3]][0] = 0x20;
          ColourAssignments[LockKeysOrigionalColours[2][3]][1] = 0x20;
          ColourAssignments[LockKeysOrigionalColours[2][3]][2] = 0x20;
        }
        else
        {
          // Scroll lock is off
          ColourAssignments[LockKeysOrigionalColours[2][3]][0] = LockKeysOrigionalColours[2][0];
          ColourAssignments[LockKeysOrigionalColours[2][3]][1] = LockKeysOrigionalColours[2][1];
          ColourAssignments[LockKeysOrigionalColours[2][3]][2] = LockKeysOrigionalColours[2][2];
        }
        PicoKeypad.illuminate(LockKeysOrigionalColours[2][3], ColourAssignments[LockKeysOrigionalColours[2][3]][0], ColourAssignments[LockKeysOrigionalColours[2][3]][1], ColourAssignments[LockKeysOrigionalColours[2][3]][2]);
      }
      PicoKeypad.update();
    }
  }
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
  static uint32_t start_ms = 0;
  static bool led_state = false;

  // blink is disabled
  if (!blink_interval_ms)
    return;

  // Blink every interval ms
  if (board_millis() - start_ms < blink_interval_ms)
    return; // not enough time
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}

//--------------------------------------------------------------------+
// Library Code
//--------------------------------------------------------------------+

void RGBMacroPad::RemoveButtonSetup(int ButtonNum)
{
  // if the key is a lock key clear it
  if (ButtonAssignments[ButtonNum][0] == HID_KEY_CAPS_LOCK)
  {
    LockKeysOrigionalColours[0][0] = 0;
    LockKeysOrigionalColours[0][1] = 0;
    LockKeysOrigionalColours[0][2] = 0;
    LockKeysOrigionalColours[0][3] = 0;
  }
  else if (ButtonAssignments[ButtonNum][0] == HID_KEY_NUM_LOCK)
  {
    LockKeysOrigionalColours[1][0] = 0;
    LockKeysOrigionalColours[1][1] = 0;
    LockKeysOrigionalColours[1][2] = 0;
    LockKeysOrigionalColours[1][3] = 0;
  }
  else if (ButtonAssignments[ButtonNum][0] == HID_KEY_SCROLL_LOCK)
  {
    LockKeysOrigionalColours[2][0] = 0;
    LockKeysOrigionalColours[2][1] = 0;
    LockKeysOrigionalColours[2][2] = 0;
    LockKeysOrigionalColours[2][3] = 0;
  }
  ColourAssignments[ButtonNum][0] = 0;
  ColourAssignments[ButtonNum][1] = 0;
  ColourAssignments[ButtonNum][2] = 0;
  PicoKeypad.illuminate(ButtonNum, 0, 0, 0);
  PicoKeypad.update();
  ButtonAssignments[ButtonNum][0] = 0;
  ButtonAssignments[ButtonNum][1] = 0;
  ButtonAssignments[ButtonNum][2] = 0;
}

// ButtonNum varies from 0 to 15.
void RGBMacroPad::SetupButton(uint8_t ButtonNum, uint8_t r, uint8_t g, uint8_t b, uint8_t KeyCode, uint8_t ModifierKeys, uint8_t KeyboardType)
{
  // if the user is using any lock keys set them up
  if (KeyCode == HID_KEY_CAPS_LOCK)
  {
    LockKeysOrigionalColours[0][0] = r;
    LockKeysOrigionalColours[0][1] = g;
    LockKeysOrigionalColours[0][2] = b;
    LockKeysOrigionalColours[0][3] = ButtonNum;
  }
  else if (KeyCode == HID_KEY_NUM_LOCK)
  {
    LockKeysOrigionalColours[1][0] = r;
    LockKeysOrigionalColours[1][1] = g;
    LockKeysOrigionalColours[1][2] = b;
    LockKeysOrigionalColours[1][3] = ButtonNum;
  }
  else if (KeyCode == HID_KEY_SCROLL_LOCK)
  {
    LockKeysOrigionalColours[2][0] = r;
    LockKeysOrigionalColours[2][1] = g;
    LockKeysOrigionalColours[2][2] = b;
    LockKeysOrigionalColours[2][3] = ButtonNum;
  }
  ColourAssignments[ButtonNum][0] = r;
  ColourAssignments[ButtonNum][1] = g;
  ColourAssignments[ButtonNum][2] = b;
  PicoKeypad.illuminate(ButtonNum, r, g, b);
  PicoKeypad.update();
  ButtonAssignments[ButtonNum][0] = KeyCode;
  ButtonAssignments[ButtonNum][1] = ModifierKeys;
  ButtonAssignments[ButtonNum][2] = KeyboardType;
}

int64_t RGBMacroPad::ResetLEDsRepeat(alarm_id_t id, void *user_data)
{
  for (int i = 0; i <= 15; i++)
  {
    PicoKeypad.illuminate(i, ColourAssignments[i][0], ColourAssignments[i][1], ColourAssignments[i][2]);
  }
  PicoKeypad.update();
  TimerCancelled = true;
  return 0;
}

// simple timer that will DIM the led's when called. The timer is started below.
bool RGBMacroPad::DimLEDTimer(struct repeating_timer *t)
{
  PicoKeypad.set_brightness(MinBrightness);
  PicoKeypad.update();
  LEDDimClock = false;
  cancel_repeating_timer(&timer);
  return true;
}

// Just a simple function to be called that allows us to setup the TinyUSB
void RGBMacroPad::InitializeDevice()
{
  // Set the timer status
  TimerCancelled = true;
  LEDDimClock = false;
  // init the keypad
  PicoKeypad.init();
  PicoKeypad.set_brightness(MaxBrightness);
  // Start the timer that will dim the leds after 5 mins
  add_repeating_timer_ms(DimLedDuration, DimLEDTimer, NULL, &timer);
  LEDDimClock = true;
  // Initialize TinyUSB
  board_init();
  tusb_init();
}

/*
This will be added into the loop for void main this function will handle the detection of the button presses and handles the keypress too.
Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
tud_hid_report_complete_cb() is used to send the next report after previous one is complete
*/
void RGBMacroPad::Loop(void)
{
  tud_task(); // tinyusb device task
  if (UseBlinking)
  {
    led_blinking_task();
  }
  // Poll every 10ms
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;

  if (board_millis() - start_ms < interval_ms)
    return; // not enough time
  start_ms += interval_ms;

  // Remote wakeup
  if (tud_suspended())
  {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    tud_remote_wakeup();
  }
  else
  {
    if (tud_hid_ready())
    {
      // send empty key report if previously has key pressed
      if (has_keyboard_key)
      {
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
        has_keyboard_key = false;
        return;
      }
      // send empty key report (release key) if previously has key pressed
      uint16_t empty_key = 0;
      if (has_consumer_key)
      {
        tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &empty_key, 2);
        has_consumer_key = false;
        return;
      }
    }

    button_states = PicoKeypad.get_button_states();
    // Check to see if a button was pressed and that it was not the same as the last check. Allows us to avoid duplicate registers.
    if (last_button_states != button_states)
    {
      last_button_states = button_states;
      if (button_states > 0)
      {
        uint8_t ButtonLEDAddr = 0;
        // convert the number into the 0 - 15 address for the led. Uses a binary shift to perform this calcualtion.
        unsigned int number = button_states;
        while (number >>= 1)
          ++ButtonLEDAddr;

        // Cancel the timer if it is still running.
        // if the timer that dims the leds after 5 mins is running cancel it. if it isn't then reset the led brightness
        if (LEDDimClock == true)
        {
          cancel_repeating_timer(&timer);
          LEDDimClock = false;
        }
        else
        {
          PicoKeypad.set_brightness(MaxBrightness);
          PicoKeypad.update();
        }
        // restart the timer that will dim the leds after 5 mins
        add_repeating_timer_ms(DimLedDuration, DimLEDTimer, NULL, &timer);
        LEDDimClock = true;

        // If the user has specified our custom keyboard then we need to make sure that we handle it.
        if (ButtonAssignments[ButtonLEDAddr][2] == REPORT_ID_TINYPICO)
        {
          for (uint8_t i = 0; i < 16; i++)
          {
            PicoKeypad.illuminate(i, 0x00, 0x00, 0x00);
          }
          PicoKeypad.update();
          reset_usb_boot(0, 0);
          // Check to see if the keypad is actually ready for a keypress
        }
        else if (tud_hid_ready())
        {
          if (!(ButtonAssignments[ButtonLEDAddr][0] == HID_KEY_CAPS_LOCK || ButtonAssignments[ButtonLEDAddr][0] == HID_KEY_NUM_LOCK || ButtonAssignments[ButtonLEDAddr][0] == HID_KEY_SCROLL_LOCK))
          {
            // If the key is not a modifier key then we need to send it to the keyboard.
            // Change the colour of the led to the assigned colour
            PicoKeypad.illuminate(ButtonLEDAddr, 0x20, 0x20, 0x00);
          }
          // Send the keypress for the pressed key.
          SendKeypress(ButtonAssignments[ButtonLEDAddr][2], ButtonAssignments[ButtonLEDAddr][0], ButtonAssignments[ButtonLEDAddr][1]);
        }
        else
        {
          for (int i = 0; i <= 15; i++)
          {
            // Device not ready. Set all the leds to red.
            PicoKeypad.illuminate(i, 0x20, 0x00, 0x00);
          }
        }
        PicoKeypad.update();

        // Start the timer to reset the colours after 300ms.
        if (TimerCancelled == true && !(ButtonAssignments[ButtonLEDAddr][0] == HID_KEY_CAPS_LOCK || ButtonAssignments[ButtonLEDAddr][0] == HID_KEY_NUM_LOCK || ButtonAssignments[ButtonLEDAddr][0] == HID_KEY_SCROLL_LOCK))
        {
          TimerCancelled = false;
          add_alarm_in_ms(300, ResetLEDsRepeat, NULL, false);
        }
      }
    }
  }
}
