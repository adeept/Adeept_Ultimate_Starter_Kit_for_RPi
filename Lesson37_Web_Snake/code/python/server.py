from flask import Flask, render_template
from flask_socketio import SocketIO, Namespace
import PCF8591 as ADC
import time


app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app)

@app.route('/')
def index():
    return render_template('index.html')

def setup():
    ADC.setup(0X48)
    global state

def direction():    #get joystick result
    state = ['', 'up', 'down', 'left', 'right']
    i = 0   

    if ADC.read(1) <= 5:
        i = 1        #up
    if ADC.read(1) >= 250: 
        i = 2        #down
    if ADC.read(2) <= 5:
        i = 3        #left
    if ADC.read(2) >= 250: 
        i = 4        #right
    
    return state[i]

keyCodes = {
  'left': 37,
  'up': 38,
  'right': 39,
  'down': 40
}

class Controller(Namespace):
  
  def on_connect(self):
    print("connected")
  
  def on_disconnect(self):
    print("close connection")
  
  def on_message(self, data):
    status = ''
    while True:
      tmp = direction()
      if tmp != None and tmp != status and tmp != '': 
        print(tmp)
        self.emit('response', {'keyCode': keyCodes[tmp]})
        status = tmp
      time.sleep(0.5)

if __name__ == '__main__':
  setup()
  socketio.on_namespace(Controller('/controller'))
  socketio.run(app, port=8080, host='0.0.0.0')