from gpiozero import PWMLED
import time

# BCM17 - Servo control pin (50Hz PWM)
servo = PWMLED(17, frequency=50)
servo.value = 0
time.sleep(1)

while True:
    for i in range(0, 181, 10):
        print('i=', i)
        # 0deg = 2.5% duty, 180deg = 12.5% duty
        servo.value = (2.5 + 10 * i / 180) / 100
        time.sleep(0.2)
        servo.value = 0
        time.sleep(0.5)

    for i in range(180, 0, -10):
        print('........i=', i)
        servo.value = (2.5 + 10 * i / 180) / 100
        time.sleep(0.2)
        servo.value = 0
        time.sleep(0.5)
