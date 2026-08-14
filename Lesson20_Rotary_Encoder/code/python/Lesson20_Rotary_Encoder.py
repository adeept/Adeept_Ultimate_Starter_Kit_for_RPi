#!/usr/bin/env python
from gpiozero import Button, DigitalInputDevice
import time

# BCM pins: CLK=17, DT=18, SW=27 (physical pins 11,12,13)
RoAPin = 17
RoBPin = 18
BtnPin = 27

globalCounter = 0
flag = 0
Last_RoB_Status = 0
Current_RoB_Status = 0

# Rotary encoder pins
clk = DigitalInputDevice(RoAPin)
dt = DigitalInputDevice(RoBPin)

def setup():
    global btn
    # Button with pull-up, falling edge interrupt
    btn = Button(BtnPin, pull_up=True)
    btn.when_pressed = btnISR

def rotaryDeal():
    global flag, Last_RoB_Status, Current_RoB_Status, globalCounter
    Last_RoB_Status = dt.value
    while not clk.value:
        Current_RoB_Status = dt.value
        flag = 1
    if flag == 1:
        flag = 0
        if (Last_RoB_Status == 0) and (Current_RoB_Status == 1):
            globalCounter -= 1
        if (Last_RoB_Status == 1) and (Current_RoB_Status == 0):
            globalCounter += 1

def btnISR():
    global globalCounter
    globalCounter = 0

def loop():
    global globalCounter
    tmp = 0  # Rotary temporary
    while True:
        rotaryDeal()
        if tmp != globalCounter:
            print('globalCounter = %d' % globalCounter)
            tmp = globalCounter

def destroy():
    clk.close()
    dt.close()
    btn.close()

if __name__ == '__main__':
    setup()
    try:
        loop()
    except KeyboardInterrupt:
        destroy()
