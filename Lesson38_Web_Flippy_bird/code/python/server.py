from flask import Flask,render_template
from flask_socketio import SocketIO,Namespace
from gpiozero import Button

BtnPin=18
app=Flask(__name__)
app.config['SECRET_KEY']='secret!'
socketio=SocketIO(app)

btn=Button(BtnPin,pull_up=True)

@app.route('/')
def index():
    return render_template('Game.html')

def button_pressed():
    print('Button Press')
    socketio.emit('response',{'controller':'press'},namespace='/controller')

class Controller(Namespace):
    def on_connect(self):
        print('connected')
    def on_disconnect(self):
        print('disconnected')

if __name__=='__main__':
    btn.when_pressed=button_pressed
    socketio.on_namespace(Controller('/controller'))
    try:
        socketio.run(app,port=8080,host='0.0.0.0')
    finally:
        btn.close()