from gpiozero import DigitalOutputDevice
import time

# BCM pins: 17,18,22,27,23,24,25,4 (physical pins 11,12,15,13,16,18,22,7)
pins = [17, 18, 22, 27, 23, 24, 25, 4]
seg_pins = [DigitalOutputDevice(p) for p in pins]

# 7-segment display codes (common cathode): 0-9, A-F, dp
dats = [0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
        0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x80]

def setup():
    for pin in seg_pins:
        pin.off()

def writeOneByte(val):
    for i in range(8):
        seg_pins[i].value = (val >> i) & 0x01

def loop():
    while True:
        for dat in dats:
            writeOneByte(dat)
            time.sleep(0.5)

def destroy():
    for pin in seg_pins:
        pin.off()
        pin.close()

if __name__ == '__main__':
    setup()
    try:
        loop()
    except KeyboardInterrupt:
        destroy()
