from gpiozero import LED
import time

# BCM pins: 17,18,27,22,23,24,25,4 (physical pins 11,12,13,15,16,18,22,7)
# Common anode LEDs (active low)
pins = [17, 18, 27, 22, 23, 24, 25, 4]
leds = [LED(pin, active_high=False) for pin in pins]

def setup():
    for led in leds:
        led.off()

def loop():
    while True:
        for led in leds:  # Left to right
            led.on()
            time.sleep(0.5)
            led.off()

def destroy():
    for led in leds:
        led.off()

if __name__ == '__main__':
    setup()
    try:
        loop()
    except KeyboardInterrupt:
        destroy()
