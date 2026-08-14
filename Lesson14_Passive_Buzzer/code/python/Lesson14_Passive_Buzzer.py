from gpiozero import PWMOutputDevice
import time

buzzer=PWMOutputDevice(18)
frequencies=[200,400,600,800,900,1000,1100]

try:
    while True:
        for f in frequencies:
            buzzer.frequency=f
            buzzer.value=0.5
            time.sleep(0.2)
        for f in reversed(frequencies):
            buzzer.frequency=f
            buzzer.value=0.5
            time.sleep(0.2)
except KeyboardInterrupt:
    buzzer.value=0
    buzzer.close()