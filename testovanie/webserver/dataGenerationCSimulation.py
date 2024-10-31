import socket
import json
import time

HOST = "localhost"
PORT = 9001

s = socket.socket()
s.connect((HOST, PORT))

azimut = 0
elevacia = 5

while True:
    d = {"azimuth" : azimut,
         "elevation": elevacia}
    s.send((json.dumps(d) + '\n').encode())
    time.sleep(1)
    azimut += 2
    elevacia += 3
    