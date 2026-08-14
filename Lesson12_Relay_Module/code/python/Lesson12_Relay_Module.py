#encoding:utf-8
from gpiozero import OutputDevice
import time

time_out = 3  # Delay time 3s
RELAY = 4     # BCM pin 4

relay = OutputDevice(RELAY)

try:
    while True:
        relay.on()
        time.sleep(time_out)
        relay.off()
        time.sleep(time_out)
except KeyboardInterrupt:
    relay.close()
