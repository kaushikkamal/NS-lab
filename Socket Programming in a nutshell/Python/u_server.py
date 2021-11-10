import socket
host="127.0.0.1"
port=5005
sock=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
sock.bind((host,port))
while True:
	data, addr=sock.recvfrom(1024)
	print "received mesage: ", data
