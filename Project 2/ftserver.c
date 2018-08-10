/*
* Daniel Olivas
* CS 372 Programming Assignment #2
* 11/26/17
* ftserver.c
*/

//header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <dirent.h>
#include <sys/stat.h>

/*
* Source for setting up control server comes from 
* CS344 Operating Systems Block 4 By Prof Brewster.
* http://man7.org/linux/man-pages/man3/getaddrinfo.3.html was
* used to setup data connection as with program 1
*/

#define BUFFSIZE 1000

int listenSetUp(int portNumber);
void controlSetUp(int controlSocket);
void getDir(int controlConnect);
void getFile(int controlConnect);
int dataConnection(char *dataPortChar,char *dataServer);
void sendDirectory(int dataSocket, char *dataPortChar, char *dataServer);
void sendFile(int dataSocket, char *dataPortChar, char *dataServer, char *fileName);


int main(int argc, char *argv[]){
    
    //make sure there are 2 arguments
    if(argc != 2){
        printf("ftserver <SERVER_PORT>\n");
        exit(0);
    }
    
    //converts port number to int
    int portNumber = atoi(argv[1]);
    int controlSocket = listenSetUp(portNumber);   
    
    while(1){
        controlSetUp(controlSocket);
    }
    return 0;
}

/* 
* ListenSetUp function is passed portNumber that is given in the command line.
* Function sets up ftp server to listen for client on port number. 
*/
int listenSetUp(int portNumber){
    int controlSocket;
    struct sockaddr_in controlAddress;

    memset((char *)&controlAddress, '\0', sizeof(controlAddress));

    controlAddress.sin_family = AF_INET;
    controlAddress.sin_port = htons(portNumber);
    controlAddress.sin_addr.s_addr = INADDR_ANY;
    //creates the control socket
    controlSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(controlSocket < 0)
        printf("SOCKET error\n");
    //binds socket
    if(bind(controlSocket, (struct sockaddr *)&controlAddress, sizeof(controlAddress)) < 0){
        printf("Error BINDING\n");
        exit(0);
    }

    //server begins listening for connections
    listen(controlSocket, 5);
    printf("Server open on %d\n", portNumber);

    return controlSocket;
}


void controlSetUp(int controlSocket){

    socklen_t sizeofClientInfo;
    struct sockaddr_in clientAddress;
    int controlConnect;
    char buffer[BUFFSIZE];
    char command[256];
    char dataPortChar[256];
    int charsRead;

    sizeofClientInfo = sizeof(clientAddress);
    //new connection is formed
    controlConnect = accept(controlSocket, (struct sockaddr *)&clientAddress, &sizeofClientInfo);
    if(controlConnect < 0)
        printf("Unable to accept connection\n");

    memset(buffer, '\0', BUFFSIZE);
    //first recv is command "-l" or "-g" put into buffer
    // and copied into command variable
    charsRead = recv(controlConnect, buffer, BUFFSIZE, 0);
    strcpy(command, buffer);
    if(charsRead < 0)
        printf("Something happeneded while reading command");  
        
    //if command if "-l", then the server will return directory content
    if(strcmp(command, "-l") == 0){
        getDir(controlConnect);            
    } 

    // if "-g", the file requested get sent to client
    else if(strcmp(command, "-g") == 0){
       getFile(controlConnect);            
    }
}

void getDir(int controlConnect){
    int charsRead;
    char buffer[BUFFSIZE];
    char dataPortChar[256];

    memset(buffer, '\0', BUFFSIZE);     
    //receives Data port number
    charsRead = recv(controlConnect, buffer, BUFFSIZE, 0);
    strcpy(dataPortChar, buffer);
    if(charsRead < 0)
        printf("Error receiving port number");
    
    memset(buffer, '\0', BUFFSIZE);
    sleep(1);
            
    printf("List directory requested on port %s.\n", dataPortChar);

    socklen_t ai_addrlen;
    int dataSocket;   
    char dataServer[30];    

    dataSocket = dataConnection(dataPortChar, dataServer);                   
    sendDirectory(dataSocket, dataPortChar, dataServer);

}

void getFile(int controlConnect){
    int charsRead;
    char buffer[BUFFSIZE];
    char dataPortChar[256];
    char fileName[50];
    memset(buffer, '\0', BUFFSIZE); 
    //gets file name from client, copies into fileName
    charsRead = recv(controlConnect, buffer, BUFFSIZE, 0);
    strcpy(fileName, buffer);
    if(charsRead < 0)
        printf("Error reveiving file name");
    
    memset(buffer, '\0', BUFFSIZE);
    //gets data port number and copies into dataPortChar
    charsRead = recv(controlConnect, buffer, BUFFSIZE, 0);
    strcpy(dataPortChar, buffer);
    if(charsRead < 0)
        printf("Error receiving port number");
    
    memset(buffer, '\0', BUFFSIZE);
    sleep(1);

    printf("File \"%s\" requested on port %s\n", fileName, dataPortChar);

   socklen_t ai_addrlen;
   int dataSocket;   
   char dataServer[30];    

   dataSocket = dataConnection(dataPortChar, dataServer);
   sendFile(dataSocket, dataPortChar, dataServer, fileName);
      
}

int dataConnection(char *dataPortChar,char *dataServer){
    struct addrinfo hint;
    int s, t, dataSocket;
    struct addrinfo *results;

    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = 0;
    //hardcoded local host for client data connection
    strcpy(dataServer, "127.0.0.1");
    printf("Connection from %s.\n", dataServer);
    s = getaddrinfo(dataServer, dataPortChar, &hint, &results);
    if(s < 0){
        printf("getaddrinfo error\n");
        exit(0);
    }
          
    //data connection socket made
    dataSocket = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    //data connection is made
    t = connect(dataSocket, results->ai_addr, results->ai_addrlen);                
    if(t < 0){
        printf("could not connect to data\n");
        exit(0);
    }
    return dataSocket;
}


void sendDirectory(int dataSocket, char *dataPortChar, char *dataServer){
    //https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
    //Creates an array of strings
    char *dirContent[500];
    char wholeDir[1000];
    memset(wholeDir, '\0', 1000);
    //star used to place '*' between each file name
    char star[2];
    strcpy(star, "*\0");
    int fileNums = 0;
    DIR *d;
    struct dirent *dir;
    //opens current directory
    d = opendir(".");
             
    if(d != NULL){
        //while loop cycles through file names
        while((dir = readdir(d)) != NULL){
            if(dir->d_type == DT_REG){
                //assigns file name to current sport in dirContent array
                dirContent[fileNums] = dir->d_name;
                //https://fresh2refresh.com/c-programming/c-strings/c-strcat-function/
                //adds '*' to end of each file name
                strcat(dirContent[fileNums], star);                      
                fileNums++;                        
            }
        }
    }
    char fileNumsChar[20];
    //converts fileNums into a char names filesNumsChar
    //https://stackoverflow.com/questions/12970439/gcc-error-undefined-reference-to-itoa
    sprintf(fileNumsChar, "%d", fileNums);
        
    int i;
    //loops through each dirContent and makes one string called wholeDir            
    for(i = 0; i < fileNums; i++){
        strcat(wholeDir, dirContent[i]);
    }
    printf("Sending directory contents to %s:%s\n", dataServer, dataPortChar);
    //sends directory to client
    send(dataSocket, wholeDir, strlen(wholeDir), 0);
    closedir(d);
}


void sendFile(int dataSocket, char *dataPortChar, char *dataServer, char *fileName){
    DIR *d;
    struct dirent *dir;
    struct stat st;
    long int fileSize;
    int charsRead;
    int found = 0;
    d = opendir(".");
    if(d != NULL){
        //loops through directory
        while((dir = readdir(d)) != NULL){                    
            //if fileName matches in directory
            if(strcmp(dir->d_name, fileName) == 0){
                found = 1;
                //gets size of file and assigns it to fileSize
                //https://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
                stat(fileName, &st);
                fileSize = st.st_size;
                //fileBuffer is size of file + null terminator
                char* fileBuffer = malloc(fileSize + 1);
                int tempSize = fileSize;
                //opens file to be read into fileBuffer
                FILE *file = fopen(fileName, "rb");                            
                printf("Sending \"%s\" to %s:%s\n", fileName, dataServer, dataPortChar);
                //fread puts whole file into fileBuffer
                //https://www.tutorialspoint.com/c_standard_library/c_function_fread.htm
                fread(fileBuffer, fileSize+1, 1, file);
                send(dataSocket, fileBuffer, fileSize, 0);
                printf("file transfer complete\n");
                free(fileBuffer);
            }
        }
        if(!found){
            //if file not found, "FILE NOT FOUND" is send to client"    
            printf("File not found. Sending error message to %s:%s\n", dataServer, dataPortChar);
            char notFound[] = "FILE NOT FOUND";
            send(dataSocket, notFound,  strlen(notFound), 0);
        }
    }
    closedir(d);
}f