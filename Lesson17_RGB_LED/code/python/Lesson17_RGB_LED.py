from gpiozero import RGBLED
import time

colors = [0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0xFF00FF, 0x00FFFF, 0x6F00D2, 0xFF5809]

# BCM pins: 17,18,27 (physical pins 11,12,13)
# Common anode LED (active low)
led = RGBLED(17, 18, 27, active_high=False)

def setup():
    led.color = (0, 0, 0)  # Off

def setColor(col):  # For example: col = 0x112233
    r = ((col & 0xff0000) >> 16) / 255.0
    g = ((col & 0x00ff00) >> 8) / 255.0
    b = (col & 0x0000ff) / 255.0
    led.color = (r, g, b)

def loop():
    while True:
        for col in colors:
            setColor(col)
            time.sleep(0.5)

def destroy():
    led.off()
    led.close()

if __name__ == "__main__":
    setup()
    try:
        loop()
    except KeyboardInterrupt:
        destroy()
