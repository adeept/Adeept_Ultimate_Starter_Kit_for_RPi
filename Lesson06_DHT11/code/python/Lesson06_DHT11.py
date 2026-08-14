import ctypes
import time

DHT_PIN=4
DHTLIB_OK=0
DHTLIB_ERROR_CHECKSUM=-1
DHTLIB_ERROR_TIMEOUT=-2

lib=ctypes.CDLL("./libdht11.so")

lib.dht11_init.argtypes=[]
lib.dht11_init.restype=ctypes.c_int

lib.dht11_read.argtypes=[
    ctypes.c_int,
    ctypes.POINTER(ctypes.c_float),
    ctypes.POINTER(ctypes.c_float)
]
lib.dht11_read.restype=ctypes.c_int

lib.dht11_close.argtypes=[]
lib.dht11_close.restype=None

ret=lib.dht11_init()

if ret!=DHTLIB_OK:
    print("DHT11 init failed:",ret)
    exit(1)

try:
    count=0

    while True:
        count+=1

        humidity=ctypes.c_float()
        temperature=ctypes.c_float()

        ret=lib.dht11_read(
            DHT_PIN,
            ctypes.byref(humidity),
            ctypes.byref(temperature)
        )

        print(f"Measurement counts: {count}")

        if ret==DHTLIB_OK:
            print(f"Humidity: {humidity.value:.2f} %")
            print(f"Temperature: {temperature.value:.2f} °C\n")
        elif ret==DHTLIB_ERROR_CHECKSUM:
            print("Checksum error\n")
        elif ret==DHTLIB_ERROR_TIMEOUT:
            print("Timeout error\n")
        else:
            print("Data not good, skip\n")

        time.sleep(2)

except KeyboardInterrupt:
    print("\nExit")

finally:
    lib.dht11_close()