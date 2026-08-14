from gpiozero import Device, RotaryEncoder, Button, PWMLED
from gpiozero.pins.lgpio import LGPIOFactory
from signal import pause

Device.pin_factory = LGPIOFactory()

SW_PIN = 27
CLK_PIN = 18
DT_PIN = 17
LED_PIN = 22

counter = 0

encoder = RotaryEncoder(CLK_PIN, DT_PIN, max_steps=100, wrap=False)
button = Button(SW_PIN, pull_up=True)
led = PWMLED(LED_PIN)

def update_led():
    global counter
    if counter < 0:
        counter = 0
    if counter > 100:
        counter = 100
    led.value = counter / 100.0
    print(f"{counter}")

def encoder_changed():
    global counter
    counter = encoder.steps
    update_led()

def reset_counter():
    global counter
    counter = 0
    encoder.steps = 0
    update_led()

encoder.when_rotated = encoder_changed
button.when_pressed = reset_counter

print("Rotary Encoder + LED Test")
print("Clockwise: Counter +1")
print("Counter-clockwise: Counter -1")
print("Press button: Reset to 0")

update_led()
pause()