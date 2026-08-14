from gpiozero import DigitalInputDevice
import time

# BCM17 (physical pin 11)
PIR_OUT_PIN = 17

pir = DigitalInputDevice(PIR_OUT_PIN)

def setup():
    pass  # gpiozero auto-initializes

def loop():
    while True:
        if pir.value == 0:
            print('...Movement not detected!')
        else:
            print('Movement detected!...')
        time.sleep(1)

def destroy():
    pir.close()

if __name__ == '__main__':
    setup()
    try:
        loop()
    except KeyboardInterrupt:
        destroy()
