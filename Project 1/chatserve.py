# Daniel Olivas
# CS 372 Program 1
# 10/29/2017
# chatserve.py

from socket import*
import sys


def main():
	serverPort = sys.argv[1]
	hostName = 'ChatServer'
	serverHost = '127.0.0.1'
	# create TCP welcoming socket
	serverSocket = socket(AF_INET, SOCK_STREAM)
	serverSocket.bind((serverHost, int(serverPort)))
	#begin listening for requests
	serverSocket.listen(1)

	#loop runs until sinint(Ctrol + c) is signaled
	while 1:
		#awaiting connection confirms server is up
		print "Awaiting connection"
		#new socket created and return to connectSocket
		connectionSocket, addr = serverSocket.accept()

		#receives handle from client
		handle = connectionSocket.recv(500)

		#confirms connection with client
		print "Connection established with " + str(handle)
		
		#loops until if statement breaks out
		while 1:
			#receiving text from client, assigning to buffer
			buff = connectionSocket.recv(500)
			#if statement break out if received \quit
			if '\quit' in str(buff):
				print "Chat ended by " + str(handle)
				break
			#displays text received from client
			print str(handle) + ">" + str(buff)
			#adds input from console into buff
			buff = raw_input(hostName + ">")
			#if buff is \quit, then send quit message and break out
			if buff == "\quit":
				connectionSocket.send(buff)
				break
			#sends server text to client
			connectionSocket.send(buff)
		#close connection to client
		connectionSocket.close()

if __name__ == "__main__":
	main()