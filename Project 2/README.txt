
How to compile ftserver:
	gcc -o ftserver ftserver.c

How to run ftserver:
	./ftserver {server port number}

How to run ftclient to receive server directory content:
	python ftclient.py {server port number} -l {data port number}

How to run ftclient to receive file from server:
	python ftclient.py {server port number} -g {file name} {data port number}

Connections ends once ftserver sends data to ftclient and ftclient has received the data.