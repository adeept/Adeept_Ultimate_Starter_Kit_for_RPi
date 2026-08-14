import smbus
import time
from gpiozero import LED

bus = smbus.SMBus(1)
channels = [0x40, 0x41, 0x42, 0x43]

def setup(Addr):
    global address
    address = Addr

def read(chn):
    bus.write_byte(address, channels[chn])
    bus.read_byte(address)  # Dummy read for PCF8591
    return bus.read_byte(address)

def write(val):
    bus.write_byte_data(address, 0x40, int(val))

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


if __name__ == '__main__':
    setup(0x48)
    light_threshold = 100
    # BCM17 (physical pin 11)
    led = Led(17)

    try:
        while True:
            light_intensity = read(0)
            print('Light intensity:', light_intensity)
            if light_intensity > light_threshold:
                led.on()
            else:
                led.off()
            time.sleep(1)
    except KeyboardInterrupt:
        led.destroy()
