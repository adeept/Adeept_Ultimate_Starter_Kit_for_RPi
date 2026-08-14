from flask import Flask, render_template
from flask_socketio import SocketIO, Namespace
from gpiozero import Button
from smbus import SMBus
import time
app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app, async_mode='threading')
I2C_BUS = 1
PCF8591_ADDR = 0x48
X_CH = 0x42
Y_CH = 0x41
Z_CH = 0x43
SW_PIN = 27
bus = None
button = None
running = True
@app.route('/')
def index():
    return render_template('index.html')
def setup():
    global bus, button
    bus = SMBus(I2C_BUS)
    button = Button(SW_PIN, pull_up=True)
    button.when_pressed = button_pressed
def read_adc(channel):
    bus.write_byte(PCF8591_ADDR, channel)
    bus.read_byte(PCF8591_ADDR)
    return bus.read_byte(PCF8591_ADDR)
def button_pressed():
    print("Button pressed")
    socketio.emit('response', {'keyCode': 32}, namespace='/controller')
def direction():
    x = read_adc(X_CH)
    y = read_adc(Y_CH)
    if x < 50:
        return 'left'
    elif x > 220:
        return 'right'
    elif y < 100:
        return 'up'
    elif y > 230:
        return 'down'
    return ''
keyCodes = {
    'left': 37,
    'up': 38,
    'right': 39,
    'down': 40
}
def joystick_loop():
    last_direction = ''
    while running:
        try:
            current_direction = direction()
            if current_direction != '' and current_direction != last_direction:
                key_code = keyCodes[current_direction]
                print("Joystick:", current_direction, "KeyCode:", key_code)
                socketio.emit('response', {'keyCode': key_code}, namespace='/controller')
                last_direction = current_direction
            elif current_direction == '':
                last_direction = ''
        except Exception as e:
            print("Joystick error:", e)
        time.sleep(0.05)
class Controller(Namespace):
    def on_connect(self):
        print("Browser connected")
        self.emit('response', {'keyCode': 0})
    def on_disconnect(self):
        print("Browser disconnected")
    def on_message(self, data):
        print("Browser message:", data)
if __name__ == '__main__':
    setup()
    socketio.on_namespace(Controller('/controller'))
    socketio.start_background_task(joystick_loop)
    try:
        socketio.run(app, host='0.0.0.0', port=8080, allow_unsafe_werkzeug=True)
    finally:
        running = False
        if button:
            button.close()
        if bus:
            bus.close()