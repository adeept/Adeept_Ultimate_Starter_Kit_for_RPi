from gpiozero import LED, Button

# BCM17 (physical pin 11) - LED, common anode (active low)
led = LED(17, active_high=False)
# BCM18 (physical pin 12) - Button, pull-up
btn = Button(18, pull_up=True)

def loop():
    while True:
        if btn.is_pressed:  # Button pressed (LOW)
            print('...led on')
            led.on()
        else:
            print('led off...')
            led.off()

def destroy():
    led.off()

if __name__ == '__main__':
    try:
        loop()
    except KeyboardInterrupt:
        destroy()
