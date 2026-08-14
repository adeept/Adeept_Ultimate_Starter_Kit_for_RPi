from gpiozero import PWMLED
import time

# BCM18 (physical pin 12), common anode LED (active low), 1kHz PWM
led = PWMLED(18, active_high=False, frequency=1000)

try:
    while True:
        for dc in range(0, 101, 4):  # Fade in
            led.value = dc / 100.0
            time.sleep(0.05)
        time.sleep(1)
        for dc in range(100, -1, -4):  # Fade out
            led.value = dc / 100.0
            time.sleep(0.05)
        time.sleep(1)
except KeyboardInterrupt:
    led.off()
