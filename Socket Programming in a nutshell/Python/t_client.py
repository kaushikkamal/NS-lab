#import socket
host="127.0.0.1"
port=4446
from socket import *
s=socket(AF_INET,SOCK_STREAM,0)
s.connect((host,port))
msg=s.recv(100)
print "Message from server: "+msg
s.close()
