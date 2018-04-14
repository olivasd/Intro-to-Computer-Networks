/*
* Daniel Olivas
* CS 372 Program 1
* 10/29/2017
* chatclient.cpp
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>

using namespace std;

//http://man7.org/linux/man-pages/man3/getaddrinfo.3.html
//setUpConnection runs getaddrinfo creates and return addrinfo package
// which is then returned from the function 
struct addrinfo *setUpConnection(char host[], char *port, struct addrinfo *results){
    int s;
    struct addrinfo hint;
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    //logs error to stderr and exits program if getaddrinfo error
    s = getaddrinfo(host, port, &hint, &results);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    return results;
}
//http://man7.org/linux/man-pages/man3/getaddrinfo.3.html
//createConnection passes socket addresses from results struct
//to socket function and returns int sfd
int createConnection(struct addrinfo *results){
    
    int s, sfd;
    sfd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);

    s = connect(sfd, results->ai_addr, results->ai_addrlen);
    //logs error to stderr and exits program if unable to connect
    if(s == -1){
        fprintf(stderr, "could not connect\n");
        exit(EXIT_FAILURE);
    }
    return sfd;
}

void chat(int sfd, char *hostName){

    char handle[10];
    //buffer holds values to send server 
    //and print to screen
    char buffer[500];
    memset(buffer, 0, 500);
    //gets handle for hostB. loops while
    // handle is longer than 10 chars
    do{
        cout << "Please enter handle(up to 10 characters): ";
        cin >> handle;
    } while (strlen(handle) > 10);
    
    //sends handle name to server
    send(sfd, handle, strlen(handle), 0);
    fgets(buffer, 500, stdin);
    
    //while keep looping until nested if statements
    //break out of loop
    while(1){
    
        //displays handle and fget puts input from
        //console (stdin) into buffer
        cout << handle << ">";
        fgets(buffer, 500, stdin);
        //if \quit is typed, program returns to main
        //sends \quit to server so server can disconnect as well
        if(strcmp(buffer, "\\quit\n") == 0){
            send(sfd, buffer, strlen(buffer), 0);
            cout << "ending chat by " << handle << "\n";
            return;
        }
        //sends text in buffer to server
        send(sfd, buffer, strlen(buffer), 0);

        //memset resets memory in buffer to 0
        memset(buffer, 0, 500);

        //recv puts text from server into buffer
        recv(sfd, buffer, 500, 0);

        //if \quit received from server,
        //break out of function and back into main        
        if(strcmp(buffer, "\\quit") == 0){
            cout << "Chat Ended by " << hostName << "\n";
            return;
        }
        //display text from server
        printf("%s> %s\n", hostName , buffer);

        //memset resets memory in buffer to 0
        memset(buffer, 0, 500);
    }

}



int main(int argc, char **argv){

    struct addrinfo *results;
    int sfd;
    
    //assign command arguments to 
    //hostName and port
    char *hostName = argv[1];    
    char *port = argv[2];
    //host is 127.0.0.1, which is localhost ip
    char host[9];
    strcpy(host, "127.0.0.1");
    
    results = setUpConnection(host, port, results);
    sfd = createConnection(results);
    chat(sfd, hostName);

    return 0;
}