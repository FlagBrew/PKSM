import socket
import sys

def sendpkx(arg): 
	s = socket.socket()
	s.connect((sys.argv[1], 8081))
	f = open (arg, "rb")
	l = f.read(232)
	while (l):
		s.send(l)
		l = f.read(232)
	s.close()

for arg in sys.argv[2:]:
	sendpkx(arg)