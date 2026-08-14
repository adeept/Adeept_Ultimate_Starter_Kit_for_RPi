from flask import Flask
from gpiozero import LED

# Create Flask app
app = Flask(__name__)

# HTML code
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
        # BCM pin, common anode LED (active low)
        self.led = LED(pin, active_high=False)

    def on(self):
        """Turn on LED"""
        self.led.on()

    def off(self):
        """Turn off LED"""
        self.led.off()

    def destroy(self):
        """Close GPIO pin"""
        self.led.close()


# BCM17 (physical pin 11)
led = Led(17)


@app.route('/')
def index():
    """Index page"""
    return html


@app.route('/on')
def on():
    """Turn on LED"""
    led.on()
    return html


@app.route('/off')
def off():
    """Turn off LED"""
    led.off()
    return html


@app.route('/destroy')
def destroy():
    """Close GPIO pin"""
    led.destroy()
    return ''


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
