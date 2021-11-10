import socket
host="127.0.0.1"
port=5005
data="Hello, World!"
sock=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
sock.sendto(data,(host,port))
