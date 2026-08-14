from gpiozero import Button
from smbus import SMBus
from time import sleep

I2C_BUS = 1
PCF8591_ADDR = 0x48
X_CH = 0x42
Y_CH = 0x41
Z_CH = 0x43
SW_PIN = 27

bus = SMBus(I2C_BUS)
button = Button(SW_PIN, pull_up=True)

def read_adc(channel):
    bus.write_byte(PCF8591_ADDR, channel)
    bus.read_byte(PCF8591_ADDR)
    return bus.read_byte(PCF8591_ADDR)

def button_pressed():
    print("press")

try:
    button.when_pressed = button_pressed
    while True:
        x = read_adc(X_CH)
        y = read_adc(Y_CH)
        z = read_adc(Z_CH)

        if x < 50:
            print("left")
        elif x > 220:
            print("right")
        elif y < 100:
            print("up")
        elif y > 230:
            print("down")

        sleep(0.1)
finally:
    button.close()
    bus.close()