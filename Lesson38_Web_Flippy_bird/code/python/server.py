from flask import Flask, render_template
from flask_socketio import SocketIO, Namespace
import RPi.GPIO as GPIO
import time


BtnPin = 12    # pin12 --- button
app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app)

def setup():
  GPIO.setmode(GPIO.BOARD)       # Numbers GPIOs by physical location
  GPIO.setup(BtnPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)    # Set BtnPin's mode is input, and pull up to high level(3.3V)

@app.route('/')
def index():
  return render_template('Game.html')


class Controller(Namespace):

  def on_connect(self):
    print('connected')
  
  def on_disconnect(self):
    print('disconnected')
    GPIO.cleanup()                     # Release resource

  def on_message(self, data):
    counter = 0
    while True:
      setup()
      if GPIO.input(BtnPin) == GPIO.LOW:
        print('Button Press')
        self.emit('response', {'controller': 'press'})
      time.sleep(.5)



if __name__ == '__main__':
  socketio.on_namespace(Controller('/controller'))
  socketio.run(app, port=8080, host='0.0.0.0')

