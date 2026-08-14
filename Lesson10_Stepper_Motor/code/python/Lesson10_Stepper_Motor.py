from gpiozero import DigitalOutputDevice
import time

# BCM pins: 17,27,22,18
IN1 = 17
IN2 = 27
IN3 = 22
IN4 = 18

pins = [DigitalOutputDevice(IN1), DigitalOutputDevice(IN2),
        DigitalOutputDevice(IN3), DigitalOutputDevice(IN4)]

def setStep(w1, w2, w3, w4):
    pins[0].value = w1
    pins[1].value = w2
    pins[2].value = w3
    pins[3].value = w4

def stop():
    setStep(0, 0, 0, 0)

def forward(delay, steps):
    for i in range(0, steps):
        setStep(1, 0, 0, 0)
        time.sleep(delay)
        setStep(0, 1, 0, 0)
        time.sleep(delay)
        setStep(0, 0, 1, 0)
        time.sleep(delay)
        setStep(0, 0, 0, 1)
        time.sleep(delay)

def backward(delay, steps):
    for i in range(0, steps):
        setStep(0, 0, 0, 1)
        time.sleep(delay)
        setStep(0, 0, 1, 0)
        time.sleep(delay)
        setStep(0, 1, 0, 0)
        time.sleep(delay)
        setStep(1, 0, 0, 0)
        time.sleep(delay)

def setup():
    stop()

def loop():
    while True:
        print("backward...")
        backward(0.003, 512)  # 512 steps --- 360 angle

        print("stop...")
        stop()
        time.sleep(3)

        print("forward...")
        forward(0.005, 512)

        print("stop...")
        stop()
        time.sleep(3)

def destroy():
    for p in pins:
        p.close()

if __name__ == '__main__':
    setup()
    try:
        loop()
    except KeyboardInterrupt:
        destroy()
