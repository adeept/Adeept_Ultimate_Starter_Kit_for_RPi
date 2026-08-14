#!/usr/bin/env python3
from gpiozero import DigitalOutputDevice,DigitalInputDevice
from time import sleep
import smbus

class keypad:
    KEYPAD=[
        ['1','2','3','A'],
        ['4','5','6','B'],
        ['7','8','9','C'],
        ['*','0','#','D']
    ]
    ROW=[17,18,27,12]
    COLUMN=[23,24,25,4]

    def __init__(self):
        self.col_devices=[DigitalOutputDevice(pin,initial_value=False) for pin in self.COLUMN]
        self.row_devices=[DigitalInputDevice(pin,pull_up=True) for pin in self.ROW]

    def set_input_pullup(self,device):
        device.pin.function='input'
        device.pin.pull='up'

    def set_output_low(self,device):
        device.pin.function='output'
        device.pin.state=0

    def getKey(self):
        rowVal=-1
        colVal=-1

        for col in self.col_devices:
            self.set_output_low(col)

        for row in self.row_devices:
            self.set_input_pullup(row)

        sleep(0.001)

        for i in range(4):
            if self.row_devices[i].pin.state==0:
                rowVal=i

        if rowVal<0 or rowVal>3:
            return None

        for col in self.col_devices:
            self.set_input_pullup(col)

        self.set_output_low(self.row_devices[rowVal])

        sleep(0.001)

        for i in range(4):
            if self.col_devices[i].pin.state==0:
                colVal=i

        if colVal<0 or colVal>3:
            return None

        return self.KEYPAD[rowVal][colVal]

    def close(self):
        for dev in self.row_devices:
            try:
                dev.close()
            except:
                pass
        for dev in self.col_devices:
            try:
                dev.close()
            except:
                pass

def delay(time_ms):
    sleep(time_ms/1000.0)

def delayMicroseconds(time_us):
    sleep(time_us/1000000.0)

class Screen:
    enable_mask=1<<2
    rw_mask=1<<1
    rs_mask=1<<0
    backlight_mask=1<<3
    data_mask=0x00

    def __init__(self,cols=16,rows=2,addr=0x27,bus=1):
        self.cols=cols
        self.rows=rows
        self.bus_num=bus
        self.bus=smbus.SMBus(self.bus_num)
        self.addr=addr
        self.display_init()

    def enable_backlight(self):
        self.data_mask|=self.backlight_mask

    def disable_backlight(self):
        self.data_mask&=~self.backlight_mask

    def display_data(self,*args):
        self.clear()
        for line,arg in enumerate(args):
            if line>=self.rows:
                break
            self.cursorTo(line,0)
            self.println(str(arg)[:self.cols].ljust(self.cols))

    def cursorTo(self,row,col):
        offsets=[0x00,0x40,0x14,0x54]
        self.command(0x80|(offsets[row]+col))

    def clear(self):
        self.command(0x01)
        delay(2)

    def println(self,line):
        for char in line:
            self.print_char(char)

    def print_char(self,char):
        self.send(ord(char),self.rs_mask)

    def display_init(self):
        delay(50)
        self.write4bits(0x30)
        delay(5)
        self.write4bits(0x30)
        delay(5)
        self.write4bits(0x30)
        delay(1)
        self.write4bits(0x20)
        self.command(0x28)
        self.command(0x0C)
        self.clear()
        self.command(0x06)

    def command(self,value,delay_us=50):
        self.send(value,0)
        delayMicroseconds(delay_us)

    def send(self,data,mode):
        self.write4bits((data&0xF0)|mode)
        self.write4bits(((data<<4)&0xF0)|mode)

    def write4bits(self,value):
        value=value&~self.enable_mask
        self.expanderWrite(value)
        self.expanderWrite(value|self.enable_mask)
        self.expanderWrite(value)

    def expanderWrite(self,data):
        self.bus.write_byte(self.addr,data|self.data_mask)

    def close(self):
        self.bus.close()

key_func={
    'A':'+',
    'B':'-',
    'C':'*',
    'D':'/',
    '#':'=',
    '*':'delete'
}

if __name__=='__main__':
    screen=None
    kp=None
    try:
        screen=Screen(bus=1,addr=0x27,cols=16,rows=2)
        screen.enable_backlight()
        kp=keypad()
        expression=''
        print('Program is starting ...')
        while True:
            digit=None
            print('current expression:',expression)
            while digit is None:
                digit=kp.getKey()
                sleep(0.01)
            # print('key:',digit)
            if digit=='#':
                try:
                    result=eval(expression)
                    print('result:',result)
                    screen.display_data(expression,'= '+str(result))
                    expression=''
                except Exception:
                    print('Error')
                    screen.display_data(expression,'Error')
                    expression=''
            elif digit=='*':
                screen.display_data('','')
                expression=''
            elif digit in key_func:
                expression+=key_func[digit]
                screen.display_data(expression,'')
            else:
                expression+=str(digit)
                screen.display_data(expression,'')
            sleep(0.3)
    except KeyboardInterrupt:
        print('\nExit')
    finally:
        if kp:
            kp.close()
        if screen:
            screen.close()

