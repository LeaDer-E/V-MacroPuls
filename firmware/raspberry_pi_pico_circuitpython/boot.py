# -*- coding: utf-8 -*-
# V-MacroPlus Hardware USB HID Bridge — Raspberry Pi Pico (CircuitPython boot.py)
# Copy BOTH boot.py and code.py to the root of your CIRCUITPY USB drive, then unplug and replug the Pico.

import usb_cdc
import usb_hid

# Enable BOTH console and secondary data serial channel + Hardware USB HID Keyboard & Mouse
usb_cdc.enable(console=True, data=True)
usb_hid.enable((usb_hid.Device.KEYBOARD, usb_hid.Device.MOUSE))
