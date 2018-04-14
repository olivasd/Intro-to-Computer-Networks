
How to compile chatclient:

	g++ -o chatclient chatclient.cpp

How to run chatserve:

	python chatserve.py {port number}

How to run chatclient:
	
	chatclient {server hostName} {port number}

The connection will persist until \quit is sent by chatserve or chatclient.
Then chatclient will be available for another client until SIGINT(Ctrl - c) 
is sent.
