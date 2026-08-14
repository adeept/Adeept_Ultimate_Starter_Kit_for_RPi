#!/usr/bin/env python3
import smbus
from time import sleep

def delay_ms(ms):
    sleep(ms/1000.0)

def delay_us(us):
    sleep(us/1000000.0)

class Screen:
    ENABLE=1<<2
    RW=1<<1
    RS=1<<0
    BACKLIGHT=1<<3

    def __init__(self,cols=16,rows=2,addr=0x27,bus=1):
        self.cols=cols
        self.rows=rows
        self.addr=addr
        self.bus=smbus.SMBus(bus)
        self.data_mask=self.BACKLIGHT
        self.display_init()

    def enable_backlight(self):
        self.data_mask|=self.BACKLIGHT

    def disable_backlight(self):
        self.data_mask&=~self.BACKLIGHT

    def expanderWrite(self,data):
        self.bus.write_byte(self.addr,data|self.data_mask)

    def write4bits(self,value):
        value&=~self.ENABLE
        self.expanderWrite(value)
        self.expanderWrite(value|self.ENABLE)
        delay_us(1)
        self.expanderWrite(value)

    def send(self,data,mode):
        self.write4bits((data&0xF0)|mode)
        self.write4bits(((data<<4)&0xF0)|mode)

    def command(self,value,delay=50):
        self.send(value,0)
        delay_us(delay)

    def print_char(self,char):
        self.send(ord(char),self.RS)

    def println(self,line):
        for char in line:
            self.print_char(char)

    def cursorTo(self,row,col):
        offsets=[0x00,0x40,0x14,0x54]
        if row>=self.rows:
            return
        self.command(0x80|(offsets[row]+col))

    def clear(self):
        self.command(0x01,2000)

    def display_data(self,*args):
        self.clear()
        for row,arg in enumerate(args):
            if row>=self.rows:
                break
            self.cursorTo(row,0)
            self.println(str(arg)[:self.cols].ljust(self.cols))

    def display_init(self):
        self.enable_backlight()
        delay_ms(50)
        self.write4bits(0x30)
        delay_ms(5)
        self.write4bits(0x30)
        delay_us(150)
        self.write4bits(0x30)
        delay_us(150)
        self.write4bits(0x20)
        delay_us(150)
        self.command(0x28)
        self.command(0x08)
        self.clear()
        self.command(0x06)
        self.command(0x0C)

    def close(self):
        self.disable_backlight()
        self.expanderWrite(0)
        self.bus.close()

if __name__=="__main__":
    screen=Screen(bus=1,addr=0x27,cols=16,rows=2)
    line=" www.adeept.com"
    screen.enable_backlight()
    try:
        while True:
            screen.display_data(line,line[::-1])
            sleep(1)
            screen.display_data(line[::-1],line)
            sleep(1)
    except KeyboardInterrupt:
        screen.close()