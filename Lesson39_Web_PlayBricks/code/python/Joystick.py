from smbus import SMBus
import time

I2C_BUS=1
PCF8591_ADDR=0x48
Z_CH=0x40
Y_CH=0x41
X_CH=0x42

bus=SMBus(I2C_BUS)

def setup():
    pass

def read_adc(channel):
    bus.write_byte(PCF8591_ADDR,channel)
    bus.read_byte(PCF8591_ADDR)
    return bus.read_byte(PCF8591_ADDR)

def direction():
    state=['home','up','down','left','right','pressed']
    i=0
    x=read_adc(X_CH)
    y=read_adc(Y_CH)
    z=read_adc(Z_CH)
    if y<100:
        i=1
    if y>230:
        i=2
    if x<50:
        i=3
    if x>220:
        i=4
    if z==0:
        i=5
    if z-125<15 and z-125>-15 and y-125<15 and y-125>-15 and x==255:
        i=0
    return state[i]

def loop():
    status=''
    while True:
        tmp=direction()
        if tmp!=None and tmp!=status:
            print(tmp)
            print("z x y:",read_adc(Z_CH),read_adc(X_CH),read_adc(Y_CH))
            status=tmp
        elif tmp=='':
            status=''
        time.sleep(0.1)

def destroy():
    bus.close()

if __name__=='__main__':
    setup()
    try:
        loop()
    except KeyboardInterrupt:
        destroy()
