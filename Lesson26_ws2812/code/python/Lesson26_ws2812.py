import spidev
import time
import random

LED_COUNT = 3
SPI_BUS = 0
SPI_DEVICE = 0
SPI_SPEED = 2400000

colors = [
    (0, 0, 50),
    (50, 0, 0),
    (0, 255, 0),
    (22, 123, 52),
    (2, 88, 45),
    (127, 127, 127)
]

spi = spidev.SpiDev()
spi.open(SPI_BUS, SPI_DEVICE)
spi.max_speed_hz = SPI_SPEED
spi.mode = 0

def encode_byte(value):
    data = []
    for i in range(7, -1, -1):
        if value & (1 << i):
            data += [0x06, 0x06, 0x06]
        else:
            data += [0x04, 0x04, 0x04]
    return data

def encode_pixel(r, g, b):
    data = []
    data += encode_byte(g)
    data += encode_byte(r)
    data += encode_byte(b)
    return data

def show(pixels):
    data = []
    for r, g, b in pixels:
        data += encode_pixel(r, g, b)
    spi.xfer2(data)
    time.sleep(0.0001)

pixels = [(0, 0, 0)] * LED_COUNT

try:
    while True:
        for i in range(LED_COUNT):
            pixels[i] = random.choice(colors)
            show(pixels)
        time.sleep(1)
except KeyboardInterrupt:
    pixels = [(0, 0, 0)] * LED_COUNT
    show(pixels)
    spi.close()