from gpiozero import LED
import smbus
import time

PCF8591_ADDR = 0x48
Y_CH = 0x41
X_CH = 0x42
Z_CH = 0x43

led_pins = [17,18,27,22,23]
leds = [LED(pin,active_high=False) for pin in led_pins]

bus = smbus.SMBus(1)

def read_adc(channel):
    try:
        bus.write_byte(PCF8591_ADDR,channel)
        bus.read_byte(PCF8591_ADDR)
        return bus.read_byte(PCF8591_ADDR)
    except Exception:
        return -1

def led_on(n):
    leds[n].on()

def led_off(n):
    leds[n].off()

def led_forward():
    for i in range(len(leds)):
        led_on(i)
        time.sleep(0.3)
        led_off(i)

def led_backward():
    for i in range(len(leds)-1,-1,-1):
        led_on(i)
        time.sleep(0.3)
        led_off(i)

try:
    while True:
        x = read_adc(X_CH)
        y = read_adc(Y_CH)
        z = read_adc(Z_CH)

        if x < 0 or y < 0 or z < 0:
            print("I2C read error")
            time.sleep(0.1)
            continue

        if x < 50:
            print(f"left X={x} Y={y} Z={z}")
            led_backward()
        elif x > 220:
            print(f"right X={x} Y={y} Z={z}")
            led_forward()
        elif y < 100:
            print(f"up X={x} Y={y} Z={z}")
            led_forward()
        elif y > 230:
            print(f"down X={x} Y={y} Z={z}")
            led_backward()

        time.sleep(0.1)

except KeyboardInterrupt:
    pass

finally:
    for led in leds:
        led.off()
    bus.close()