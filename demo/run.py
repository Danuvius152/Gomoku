from flask import Flask, render_template, jsonify, request
from datetime import timedelta
import subprocess
import numpy as np
import sys

app = Flask(__name__)
app.config['SEND_FILE_MAX_AGE_DEFAULT'] = timedelta(seconds=1)

class AI:
    def __init__(self, path): #相当于类内定义成员
        self.path = path
        self.ison = False 

    def send(self, message):
        value = str(message) + '\n'
        value = bytes(value, 'UTF-8')
        self.proc.stdin.write(value)
        self.proc.stdin.flush()

    def receive(self):
        return self.proc.stdout.readline().strip().decode()
    
    def init(self, side):
        self.proc = subprocess.Popen(self.path,
                                    stdin=subprocess.PIPE,
                                    stdout=subprocess.PIPE)
        self.send(1 - side)
        self.name = self.receive()
        self.ison = True
        # if (side == 1):
        #     self.action(-1, -1)
        

    def action(self, a, b):
        self.send(str(a) + ' ' + str(b))
        val = self.receive().split(' ')
        return int(val[0]), int(val[1])

    def end(self):
        if (self.ison):
            self.proc.terminate()
            self.ison = False
            
ai = AI("./sample")
bs = AI("./base") 

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/initialize', methods=['POST'])
def start():
    message_back=""
    player_side = int(request.get_data())
    ai.end()
    ai.init(player_side)
    message_back +='Successfully run '+ai.name+'!@'
    if(player_side):
        a, b = ai.action(-1, -1)
        message_back += str(a) + " " + str(b)
    return message_back

@app.route('/action')
def get_action():
    a = int(request.args['row'])
    b = int(request.args['col'])
    a, b = ai.action(a, b)
    message_back = str(a) + " " + str(b)
    message_json = {"message": message_back}
    return jsonify(message_json)
    
@app.route('/start_jiji', methods=['POST'])
def begin():
    message_back=""
    player_side = int(request.get_data())
    ai.end()
    ai.init(player_side)
    bs.end()
    bs.init(1-player_side)
    message_back +='Successfully run AI!'+'@'
    if(player_side):
        a, b = ai.action(-1, -1)
        message_back += str(a) + " " + str(b)
    else:
        a, b = bs.action(-1, -1)
        message_back += str(a) + " " + str(b)
    return message_back

@app.route('/action_bs')
def get_action_bs():
    a = int(request.args['row'])
    b = int(request.args['col'])
    a, b = bs.action(a, b)
    message_back = str(a) + " " + str(b)
    message_json = {"message": message_back}
    return jsonify(message_json)


if __name__ == '__main__':
    # app.run()
    app.run(debug=True)
