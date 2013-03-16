from flask import Flask, request, g, render_template, redirect, url_for
import room_controller.arduino


app = Flask(__name__)
the_arduino = room_controller.arduino.Arduino()
print "Flask imported"

@app.route('/')
def index():
    return render_template('index.html')


@app.route('/run', methods=['POST'])
def run_command():
    command = request.form.get('command', '')
    print 'command', command
    the_arduino.send_raw_command(command)
    print 'command', command
    return redirect(url_for('index'))


if __name__ == "__main__":
    app.run('0.0.0.0', 5000)
