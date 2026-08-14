from gpiozero import LED
import time

# BCM17 (physical pin 11), common anode LED (active low)
led = LED(17, active_high=False)

try:
    while True:
        print('...led on')
        led.on()
        time.sleep(0.5)
        print('led off...')
        led.off()
        time.sleep(0.5)
except KeyboardInterrupt:
    led.off()
