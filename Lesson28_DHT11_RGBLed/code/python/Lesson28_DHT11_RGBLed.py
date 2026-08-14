import ctypes
import time
import lgpio

DHT_PIN=22
LED_RED=17
LED_GREEN=18
LED_BLUE=27
DHTLIB_OK=0
DHTLIB_ERROR_CHECKSUM=-1
DHTLIB_ERROR_TIMEOUT=-2
PWM_FREQUENCY=1000
RGB_TEST_DELAY=0.5
DHT_READ_DELAY=2.0

lib=ctypes.CDLL("./libdht11.so")
lib.dht11_init.argtypes=[]
lib.dht11_init.restype=ctypes.c_int
lib.dht11_read.argtypes=[ctypes.c_int,ctypes.POINTER(ctypes.c_float),ctypes.POINTER(ctypes.c_float)]
lib.dht11_read.restype=ctypes.c_int
lib.dht11_close.argtypes=[]
lib.dht11_close.restype=None

COLORS=[0xFF0000,0x00FF00,0x0000FF]

def map_value(x,in_min,in_max,out_min,out_max):
    return (x-in_min)*(out_max-out_min)//(in_max-in_min)+out_min

def led_init(h):
    lgpio.gpio_claim_output(h,LED_RED,1)
    lgpio.gpio_claim_output(h,LED_GREEN,1)
    lgpio.gpio_claim_output(h,LED_BLUE,1)

def led_color_set(h,color):
    r=(color>>16)&0xFF
    g=(color>>8)&0xFF
    b=color&0xFF
    r=map_value(r,0,255,0,100)
    g=map_value(g,0,255,0,100)
    b=map_value(b,0,255,0,100)
    lgpio.tx_pwm(h,LED_RED,PWM_FREQUENCY,100-r,0,0)
    lgpio.tx_pwm(h,LED_GREEN,PWM_FREQUENCY,100-g,0,0)
    lgpio.tx_pwm(h,LED_BLUE,PWM_FREQUENCY,100-b,0,0)

def rgb_test(h):
    print("RGB test: RED -> GREEN -> BLUE")
    for color in COLORS:
        led_color_set(h,color)
        time.sleep(RGB_TEST_DELAY)
    led_color_set(h,0)

def main():
    print("Raspberry Pi DHT11 + RGB LED Test")
    print(f"DHT11 = BCM{DHT_PIN}")
    print(f"RGB LED = R:{LED_RED} G:{LED_GREEN} B:{LED_BLUE}")
    gpio_handle=lgpio.gpiochip_open(4)
    if gpio_handle<0:
        print("gpiochip4 not detected, fallback to gpiochip0")
        gpio_handle=lgpio.gpiochip_open(0)
        if gpio_handle<0:
            print("Failed to open GPIO chip")
            return
    dht_ret=lib.dht11_init()
    if dht_ret!=DHTLIB_OK:
        print("DHT11 init failed:",dht_ret)
        lgpio.gpiochip_close(gpio_handle)
        return
    try:
        led_init(gpio_handle)
        rgb_test(gpio_handle)
        count=0
        while True:
            count+=1
            humidity=ctypes.c_float()
            temperature=ctypes.c_float()
            ret=lib.dht11_read(DHT_PIN,ctypes.byref(humidity),ctypes.byref(temperature))
            print(f"Measurement counts: {count}")
            if ret==DHTLIB_OK:
                temperature_int=int(temperature.value)
                humidity_int=int(humidity.value)
                print(f"Humidity: {humidity.value:.2f} %\tTemperature: {temperature.value:.2f} °C")
                color=((temperature_int+humidity_int)&0xFF)<<16
                led_color_set(gpio_handle,color)
            else:
                print("Data not good, skip")
            time.sleep(DHT_READ_DELAY)
    except KeyboardInterrupt:
        print("\nExit")
    finally:
        led_color_set(gpio_handle,0)
        lib.dht11_close()
        lgpio.gpiochip_close(gpio_handle)

if __name__=="__main__":
    main()