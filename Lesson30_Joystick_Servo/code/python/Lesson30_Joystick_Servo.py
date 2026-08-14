from gpiozero import AngularServo
from smbus2 import SMBus
from time import sleep

SERVO_PIN = 17
PCF8591_ADDR = 0x48
Y_CH = 0x41
X_CH = 0x42
Z_CH = 0x43

servo = AngularServo(
    SERVO_PIN,
    min_angle=-90,
    max_angle=90,
    min_pulse_width=0.0005,
    max_pulse_width=0.0025
)

bus = SMBus(1)

def read_adc(channel):
    bus.write_byte(PCF8591_ADDR, channel)
    bus.read_byte(PCF8591_ADDR)
    return bus.read_byte(PCF8591_ADDR)

def set_servo(angle):
    angle = max(-90, min(90, angle))
    servo.angle = angle
    print("Servo angle:", angle)

angle = 0
set_servo(angle)

try:
    while True:
        x = read_adc(X_CH)
        y = read_adc(Y_CH)
        z = read_adc(Z_CH)

        if x < 50:
            print("left")
            angle -= 20
            set_servo(angle)
            sleep(0.2)

        elif x > 220:
            print("right")
            angle += 20
            set_servo(angle)
            sleep(0.2)

        elif y < 100:
            print("up")
            angle += 20
            set_servo(angle)
            sleep(0.2)

        elif y > 230:
            print("down")
            angle -= 20
            set_servo(angle)
            sleep(0.2)

        else:
            sleep(0.05)

except KeyboardInterrupt:
    print("\nExit")

finally:
    servo.detach()
    bus.close()