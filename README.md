# Raspberry Pi Pico MacroPad

[![Compile For the Pico.](https://github.com/SuperNinja-4965/Raspberry-Pi-Pico-MacroPad/actions/workflows/main.yml/badge.svg)](https://github.com/SuperNinja-4965/Raspberry-Pi-Pico-MacroPad/actions/workflows/main.yml)<br>
This code allows you to use the Pimoroni RGB Keypad for the Pico as a macro keypad in C++ <br>
Setting what keybind each key is really easy to do and the code is written in a way that makes it easy to change what each key does.<br>

## Downloading

It is advised that you clone the latest version of the code when you want to use this project rather than using the latest release. The latest release might not contain the latest version of the code.

### HTTP

The latest version of the code can be downloaded here: [Download](https://github.com/SuperNinja-4965/Raspberry-Pi-Pico-MacroPad/archive/refs/heads/main.zip) <br>
Just dont forget to include the RGB Keypad library in libs/pimoroni-pico which can be downloaded here: [Pimoroni-Pico](https://github.com/pimoroni/pimoroni-pico/archive/refs/heads/main.zip)

### Git

Clone the repo:

```sh
git clone https://github.com/SuperNinja-4965/Raspberry-Pi-Pico-MacroPad.git
```

Initialise submodules:

```sh
git submodule update --init --recursive
```

## TinyUSB

This code uses the TinyUSB library in order to perform the hid keyboard responses. <br>
To find the possible keys that can be used visit here: [TinyUSB hid.h](https://github.com/hathach/tinyusb/blob/master/src/class/hid/hid.h) <br>
ONLY KEYS FROM THE NORMAL KEYBOARD AND THE CONSUMER KEYBOARD are supported by default you will have to add any other keyboards yourself. Modifier keys such as CTRL and ALT can only be used with the normal keyboard.<br>
The Code cannot be published pre-compiled because all of the keybinds are set in the code. <br>
<br>
If you run into any bugs please open an issue.

## Building

For building see:
https://github.com/SuperNinja-4965/Raspberry-Pi-Pico-MacroPad/blob/90debf3c4d497ac0d6c5061a2cab16b4f5a9b88a/.github/workflows/main.yml#L51-L62
https://github.com/SuperNinja-4965/Raspberry-Pi-Pico-MacroPad/blob/90debf3c4d497ac0d6c5061a2cab16b4f5a9b88a/.github/workflows/main.yml#L72-L76
https://github.com/SuperNinja-4965/Raspberry-Pi-Pico-MacroPad/blob/90debf3c4d497ac0d6c5061a2cab16b4f5a9b88a/.github/workflows/main.yml#L86-L93

If you have not used the Pico SDK before I recommend you read this to get started: https://rptl.io/pico-get-started

# Links

[The Pi Hut - Raspberry Pi Pico](https://thepihut.com/products/raspberry-pi-pico) <br>
[Pimoroni - Raspberry Pi Pico](https://shop.pimoroni.com/products/raspberry-pi-pico?variant=32402092294227) <br>
[Pimoroni - RGB Keypad Base](https://shop.pimoroni.com/products/pico-rgb-keypad-base) <br>

# Testing

This has been tested on: <br>

- Raspberry Pi Pico: [Pimoroni - Raspberry Pi Pico](https://shop.pimoroni.com/products/raspberry-pi-pico?variant=32402092294227), [The Pi Hut - Raspberry Pi Pico](https://thepihut.com/products/raspberry-pi-pico)
- With the Pimoroni RGB Keypad Base: https://shop.pimoroni.com/products/pico-rgb-keypad-base
- Compiled on Arch Linux and Ubuntu 20.04 LTS using the latest pico-sdk, Pimoroni Libraries on 24/12/2021

# License

```
The MIT License (MIT)

Copyright (c) 2022 SuperNinja_4965

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
```
