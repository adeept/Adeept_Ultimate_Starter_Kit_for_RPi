#!/usr/bin/env python3
from gpiozero import DigitalOutputDevice
import time

seg_pins=[DigitalOutputDevice(p,initial_value=True) for p in [17,18,27,22,23,24,25,4]]
bits=[DigitalOutputDevice(p,initial_value=True) for p in [2,3,8,7]]
segCode=[0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f]
cnt=0

def digitalWriteByte(data):
    for i in range(8):
        seg_pins[i].value=(data>>i)&0x01

def display():
    global cnt
    datBuf=[
        segCode[cnt%10],
        segCode[cnt%100//10],
        segCode[cnt%1000//100],
        segCode[cnt//1000]
    ]
    for _ in range(100):
        bits[0].off()
        bits[1].on()
        bits[2].on()
        bits[3].on()
        digitalWriteByte(datBuf[0])
        time.sleep(0.001)

        bits[0].on()
        bits[1].off()
        bits[2].on()
        bits[3].on()
        digitalWriteByte(datBuf[1])
        time.sleep(0.001)

        bits[0].on()
        bits[1].on()
        bits[2].off()
        bits[3].on()
        digitalWriteByte(datBuf[2])
        time.sleep(0.001)

        bits[0].on()
        bits[1].on()
        bits[2].on()
        bits[3].off()
        digitalWriteByte(datBuf[3])
        time.sleep(0.001)
    cnt+=1
    if cnt==10000:
        cnt=0

try:
    print("Program is running...")
    while True:
        display()
except KeyboardInterrupt:
    pass
finally:
    for pin in seg_pins:
        pin.off()
        pin.close()
    for bit in bits:
        bit.off()
        bit.close()