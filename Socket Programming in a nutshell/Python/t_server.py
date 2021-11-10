#import socket
host="127.0.0.1"
port=4446
from socket import *
s=socket(AF_INET,SOCK_STREAM,0)
s.bind((host,port))
s.listen(1)
print "Listening for connection.."
q,addr=s.accept()
data=raw_input("Enter data to be send: ")
q.send(data)
s.close()
