from flask import Flask
import RPi.GPIO as GPIO

# Create Flask app
app = Flask(__name__)

# html code
html = """
  <a href='/on'>Turn on</a>
  <a href='/off'>Turn off</a>
  <script>
    window.onbeforeunload = () => {
        alert('关闭')
      fetch('/destroy').then(() => {
        alert('关闭')
      })
    }
  <script>
"""

class Led:
  def __init__(self, pin):
    self.pin = pin  # LED GPIO pin

  def setup(self):
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(self.pin, GPIO.OUT)
    GPIO.output(self.pin, GPIO.HIGH)

  def on(self):
    """turn on led"""
    GPIO.output(self.pin, GPIO.LOW)
  
  def off(self):
    """turn off led"""
    GPIO.output(self.pin, GPIO.HIGH)

  def destroy(self):
    """close GPIO pin"""
    GPIO.cleanup()


led = Led(11)
led.setup()


@app.route('/')
def index():
  """index page"""
  return html


@app.route('/on')
def on():
  """turn on led"""
  led.on()
  return html


@app.route('/off')
def off():
  """turn off led"""
  led.off()
  return html


@app.route('/destroy')
def destroy():
  """close GPIO pin"""
  led.destroy()


if __name__ == '__main__':
  app.run(host='0.0.0.0', port=5000)