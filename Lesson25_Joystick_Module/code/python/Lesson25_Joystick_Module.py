import PCF8591 as ADC
import time


def setup():
    ADC.setup(0X48)
    global state


def direction():    #get joystick result
    state = ['home', 'up', 'down', 'left', 'right', 'pressed']
    i = 0   

    if ADC.read(1) <= 5:
        i = 1        #up
    if ADC.read(1) >= 250: 
        i = 2        #down

    if ADC.read(2) <= 5: 
        i = 3        #left
    if ADC.read(2) >= 250:
        i = 4        #right

    if ADC.read(0) == 0:
        i = 5        # Button pressed 

    if ADC.read(0) - 125 < 15 and ADC.read(0) - 125 > -15 and ADC.read(1) - 125 < 15 and ADC.read(1) - 125 > -15 and ADC.read(2) == 255:
        i = 0
    
    return state[i]

def loop(): 
    status = ''
    while True:
        tmp = direction()
        if tmp != None and tmp != status: 
            print(tmp)
            # z x y
            print("z x y: ",ADC.read(0), ADC.read(1), ADC.read(2))
            status = tmp
        time.sleep(1)

def destroy():
    pass    

if __name__ == '__main__':        # Program start from here
    setup() 
    try:    
        loop()
    except KeyboardInterrupt:      # When 'Ctrl+C' is pressed, the child program destroy() will be  executed.
        destroy()
