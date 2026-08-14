#!/usr/bin/env python3
from gpiozero import DigitalInputDevice, DigitalOutputDevice
from time import sleep

ROW = [17, 18, 27, 12]
COLUMN = [23, 24, 25, 4]

KEYPAD = [
    ['1', '2', '3', 'A'],
    ['4', '5', '6', 'B'],
    ['7', '8', '9', 'C'],
    ['*', '0', '#', 'D']
]

rows = [DigitalInputDevice(pin, pull_up=True) for pin in ROW]
cols = [DigitalOutputDevice(pin, initial_value=False) for pin in COLUMN]


def set_input_pullup(device):
    device.pin.function = 'input'
    device.pin.pull = 'up'


def set_output_low(device):
    device.pin.function = 'output'
    device.pin.state = 0


def getKey():
    rowVal = -1
    colVal = -1

    for col in cols:
        set_output_low(col)

    for row in rows:
        set_input_pullup(row)

    sleep(0.001)

    for i in range(4):
        if rows[i].pin.state == 0:
            rowVal = i

    if rowVal < 0 or rowVal > 3:
        return None

    for col in cols:
        set_input_pullup(col)

    set_output_low(rows[rowVal])

    sleep(0.001)

    for i in range(4):
        if cols[i].pin.state == 0:
            colVal = i

    if colVal < 0 or colVal > 3:
        return None

    return KEYPAD[rowVal][colVal]


def main():
    print("Program is starting ...")
    print("Press Ctrl+C to exit")

    last_key = None

    try:
        while True:
            key = getKey()

            if key is not None:
                if key != last_key:
                    print("You Pressed Key :", key)
                    last_key = key
            else:
                last_key = None

            sleep(0.05)

    except KeyboardInterrupt:
        print("\nEnding program")

    finally:
        for row in rows:
            try:
                row.close()
            except:
                pass

        for col in cols:
            try:
                col.close()
            except:
                pass


if __name__ == "__main__":
    main()
