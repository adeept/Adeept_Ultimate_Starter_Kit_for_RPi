from gpiozero import DigitalInputDevice, LED
import time

# BCM27 (physical pin 13) - PIR sensor output
PIR_OUT_PIN = 27

pir = DigitalInputDevice(PIR_OUT_PIN)

class Led:
    def __init__(self, pin):
        # BCM pin, common anode LED (active low)
        self.led = LED(pin, active_high=False)

    def on(self):
        """Turn on LED"""
        self.led.on()

    def off(self):
        """Turn off LED"""
        self.led.off()

    def destroy(self):
        """Close GPIO pin"""
        self.led.close()

def destroy():
    pir.close()

if __name__ == '__main__':
    # BCM17 (physical pin 11)
    led = Led(17)
    try:
        while True:
            if pir.value == 0:
                print('...Movement not detected!')
                led.off()
            else:
                print('Movement detected!...')
                led.on()
            time.sleep(0.5)

    except KeyboardInterrupt:
        destroy()
        led.destroy()
