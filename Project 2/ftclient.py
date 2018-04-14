# Daniel Olivas
# CS 372 Programming Assignment #2
# 11/26/17
# ftclient.py

#both client and data connections are created using
#lecture 15 of CS 372

#imported header files
from socket import *
from sys import *
from time import *
import os.path


def check_arg_length(argv):
	#ensure argument length is correct
	if len(argv) < 3 or len(argv) > 5:
		print "For Server Directory:"
		print "python ftclient.py {server port number} -l {data port number}"
		print "For File From Server:"
		print "python ftclient.py {server port number} -g {file name} {data port number}"
		exit(1)


def set_up_control(argv):
	#server info from argument lines
	serverHost = "127.0.0.1"
	serverPort = argv[1]
	#command = "-l" or "-g"
	command = argv[2]
	#creates client socket and connects to server
	clientSocket = socket(AF_INET, SOCK_STREAM)
	clientSocket.connect((serverHost, int(serverPort)))
	return command, clientSocket, serverPort


def send_command(argv, clientSocket, command):
	# "-l" sent to server to get directory displayed
	if command == "-l":
	    #assigns dataPort from argument line
	    dataPort = argv[3]
	    fileName = None
	    #sends command to server so server knows to return directory
	    clientSocket.send(command)
	    #sleep delays messages getting received from server as 1 message
	    sleep(.05)
	    #port number data directory will be sent back on
	    clientSocket.send(dataPort)
	# "-g" sent to server to receive requested file    
	elif command == "-g":
	    dataPort = argv[4]
	    fileName = argv[3]
	    #https://docs.python.org/2/library/os.path.html
	    #checks current directory to see if file already exists
	    #prints and error and program exits
	    if os.path.isfile(fileName):
	        print "file already in directory"
	        exit()
	    #if file doesn't exist, command is sent to server that a file
	    #wants to be tranfered
	    clientSocket.send(command)
	    #sleep delays messages getting received from server as 1 message
	    sleep(.05)
	    #sends fileName to server
	    clientSocket.send(fileName)
	    sleep(.05)
	    #sends port number file will be sent back on
	    clientSocket.send(dataPort)
	return dataPort, fileName


def set_up_data(serverHost, dataPort):
	# create TCP welcoming socket
	dataSocket = socket(AF_INET, SOCK_STREAM)
	dataSocket.bind((serverHost, int(dataPort)))

	#begin listening for data to be sent back
	dataSocket.listen(1)

	#connection is established
	connectionSocket, addr = dataSocket.accept()
	return connectionSocket, dataSocket

def get_dir(connectionSocket):
	print "Receiving directory structure from " + argv[1] + ":"   
	#receiving directory as 1 large string, file names are
	# separated by '*' 
	dirContent = connectionSocket.recv(1000)
	#https://stackoverflow.com/questions/12723751/replacing-instances-of-a-character-in-a-string
	# replaces '*' with next line character
	dirContent = dirContent.replace('*', '\n')
	#displays directory to screen
	print dirContent
	

def get_file(connectionSocket, fileName):
	    #https://stackoverflow.com/questions/17667903/python-socket-receive-large-amount-of-data
	    #data will hold file contents
	    data = ""
	    #while loops through through to to assign chunks of file content
	    # into variable part, and then appended to data 
	    #loop breaks out once part receives all the chunks
	    while True:
	        part = connectionSocket.recv(4096)
	        data += part
	        if len(part) < 4096:
	            break
	    #if server does not have file, server returns "FILE NOT FOUND"
	    # error message is displayes and program exits
	    if data == "FILE NOT FOUND":
	        print argv[1] + ":" + serverPort + " says FILE NOT FOUND"
	        exit()
	    #if file odes exits
	    else:
	        #open new file to write
	        newFile = open(fileName,"w+")
	        print "Receiving " + fileName + " from " + argv[1]
	        #writes contents of data variable into new file
	        newFile.write(data)
	        newFile.close()
	        print "File transfer complete."


def main():
	serverHost = "127.0.0.1"
	check_arg_length(argv)
	command, clientSocket, serverPort = set_up_control(argv)	
	dataPort, fileName = send_command(argv, clientSocket, command)
	connectionSocket, dataSocket = set_up_data(serverHost, dataPort)

	# receiving directory from server
	if command == "-l":
	    get_dir(connectionSocket)

	# receives file from directory
	elif command == "-g":
		get_file(connectionSocket, fileName)

	#close all connections made
	dataSocket.close()
	connectionSocket.close()
	clientSocket.close()


if __name__ == "__main__":
	main()
