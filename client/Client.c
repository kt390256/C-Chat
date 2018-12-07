#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include "DieWithError.c"
#define RCVBUFSIZE 500 /* Size of receive buffer */
void DieWithError(char *errorMessage);

int main(int argc, char *argv[])
{
    int sock, chatSock, clntLen, clntSock; /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in echoClntAddr;
    int echoServPort; /* Echo server port */
    char servlP[20], echoBuffer[RCVBUFSIZE], option[1], user1[20], user2[20], password[20], recipient[20], msg[500], msg1[500], sender[100]; /* Server IP address (dotted quad) */
    char *echoString; /* String to send to echo server */
    unsigned int echoStringLen; /* Length of string to echo */
    int connected = 0, bytesRcvd, recvMsgSize; /* Bytes read in single recv() and total bytes read */
    while(1)
    {
        /* Receive up to the buffer size (minus i to leave space for a null terminator) bytes from the sender */
        printf("-----------------------------------------------\nCommand:\n");
        printf("0. Connect to the server\n");
        printf("1. Get the user list\n");
        printf("2. Send a message\n");
        printf("3. Get my messages\n");
        printf("4. Initial a chat with my friend\n");
        printf("5. Chat with my friend\n");
        printf("Your option<enter a number>: ");
        scanf("%s", option);
        if(connected == 1){
            if(send(sock, option, 1, 0) != 1)
                DieWithError("send() sent a different number of bytes than expected!");
        }
//===================================option 0=============================================================//
        if(strcmp(option, "0")==0){
            if(connected == 0){
                printf("Please enter the IP address: ");
                scanf("%s", servlP);
                printf("Please enter the port number: ");
                scanf("%d", &echoServPort);
                printf("Connecting...\n");
                /* Create a reliable, stream socket using TCP */
                if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
                    DieWithError(" socket () failed") ;}
                /* Construct the server address structure */
                memset(&echoServAddr, 0, sizeof(echoServAddr));
                /* Zero out structure */
                echoServAddr.sin_family = AF_INET;
                /* Internet address family */
                echoServAddr.sin_addr.s_addr = inet_addr(servlP);
                /* Server IP address */
                echoServAddr.sin_port = htons(echoServPort); /* Server port */
                /* Establish the connection to the echo server */
                if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0){
                    DieWithError(" connect () failed");}
                connected = 1;
            }//end of if

            //sending option to server for next step processing
            if(send(sock, option, 1, 0) != 1)
                DieWithError("send() sent a different number of bytes than expected!");

            printf("Connected!\nWelcome! Please log in.\nUsername: ");

            scanf("%s", user1);
            echoStringLen = strlen(user1);
            if(send(sock, user1, echoStringLen, 0) != echoStringLen){
                DieWithError("send() sent a different number of bytes than expected!");}

            printf("Password: ");
            scanf("%s", password);
            echoStringLen = strlen(password);
            if(send(sock, password, echoStringLen, 0) != echoStringLen){
                DieWithError("send() sent a different number of bytes than expected!");}

                /* Receive up to the buffer size bytes from the sender */
            if((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE, 0)) < 0){
                DieWithError("recv() failed");}
            printf("%s\n", echoBuffer);
            if(strcmp(echoBuffer, "login fail\n")==0){
                exit(0);}
        }
//==========================================Option 1============================================//
        else if(strcmp(option, "1")==0){
          //receiving user lise
            if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0){
                DieWithError("recv() failed or connection closed prematurely");}
            echoBuffer[bytesRcvd] = '\0';
            printf("%s\n", echoBuffer);
        }
//============================================Option 2========================================//
        else if(strcmp(option, "2")==0){
            printf("Please enter the username: ");
            scanf("%s", recipient);
            echoStringLen = strlen(recipient);
            if(send(sock, recipient, echoStringLen, 0) != echoStringLen){
                DieWithError("send() sent a different number of bytes than expected!");}
            printf("Please enter the message: ");
            scanf("%s", msg);
            fgets(msg1, 500, stdin);
            strcat(msg, msg1);
            echoStringLen = strlen(msg);
            if(send(sock, msg, echoStringLen, 0) != echoStringLen){
                DieWithError("send() sent a different number of bytes than expected!");}
            if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0){
                DieWithError("recv() failed or connection closed prematurely");}
            echoBuffer[bytesRcvd] = '\0';
            printf("%s\n", echoBuffer);
        }
//==========================================Option 3========================================//
         else if(strcmp(option, "3")==0){
            if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0){
                DieWithError("recv() failed or connection closed prematurely");}
            echoBuffer[bytesRcvd] = '\0';
            printf("%s\n", echoBuffer);
        }
//==========================================option 4========================================//
        else if(strcmp(option, "4")==0){
            close(sock);
            printf("-------------------disconnect from the server-------------------\n");
            printf("Please enter the port number you want to listen on: ");
            scanf("%d", &echoServPort);
            if ((chatSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
                DieWithError( "socket () failed");}
            echoServAddr.sin_port = htons(echoServPort);
            if (bind(chatSock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0){
                DieWithError ( "bind () failed");}
            /* Mark the socket so it will listen for incoming connections */
            if (listen(chatSock, 5) < 0){
                DieWithError("listen() failed") ;}
            printf("I am listening on 127.0.0.1:%d!\n", echoServPort);
            /* Set the size of the in-out parameter */
            clntLen = sizeof(echoClntAddr);
            /* Wait for a client to connect */
            if ((clntSock = accept(chatSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0){
                DieWithError("accept() failed");}
            printf("Client connected!\n");
            if ((recvMsgSize = recv(clntSock, user2, 100, 0)) < 0){
                DieWithError("recv() failed");}
            user2[recvMsgSize] = '\0';
            printf("%s is listening.\n", user2);
            echoStringLen = strlen(user1);
            if(send(clntSock, user1, echoStringLen, 0) != echoStringLen){
                DieWithError("send() sent a different number of bytes than expected!");}
            while(1>0){
                if ((recvMsgSize = recv(clntSock, msg, 500, 0)) < 0){
                    DieWithError("recv() failed");}
                msg[recvMsgSize] = '\0';
                printf("Type 'Bye' to stop the conversation!\n");
                printf("%s: %s", user2, msg);
                if(strcmp(msg, "Bye\n")==0)
                break;

                memset(msg, 0, sizeof(msg));
                printf("%s: ", user1);
                scanf("%s", msg);
                fgets(msg1, 500, stdin);
                strcat(msg, msg1);
                echoStringLen = strlen(msg);
                if(send(clntSock, msg, echoStringLen, 0) != echoStringLen)
                    DieWithError("send() sent a different number of bytes than expected!");
                if(strcmp(msg, "Bye\n")==0)
                break;
              }
            close(chatSock);
        }

        else if(strcmp(option, "5")==0){
            close(sock);
            printf("-------------------disconnect from the server-------------------\n");
            printf("Please enter your friend's IP address: ");
            scanf("%s", &servlP);
            printf("Please enter your friend's port number: ");
            scanf("%d", &echoServPort);
            printf("Connecting to your friend...\n");
            /* Construct the server address structure */
            memset(&echoServAddr, 0, sizeof(echoServAddr));
            if ((chatSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
                DieWithError(" socket () failed") ;}
            /* Zero out structure */
            echoServAddr.sin_family = AF_INET;
            echoServAddr.sin_addr.s_addr = inet_addr(servlP);
            /* Server IP address */
            echoServAddr.sin_port = htons(echoServPort); /* Server port */
            /* Establish the connection to the echo server */
            if (connect(chatSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0){
                DieWithError(" connect () failed");}
            printf("Connected!\n");
            echoStringLen = strlen(user1);
            if(send(chatSock, user1, echoStringLen, 0) != echoStringLen){
                DieWithError("send() sent a different number of bytes than expected!");}
            if ((recvMsgSize = recv(chatSock, user2, 500, 0)) < 0){
                DieWithError("recv() failed");}
            while(1>0){
                printf("Type 'Bye' to stop the conversation!\n");
                printf("%s: ", user1);
                scanf("%s", msg);
                fgets(msg1, 500, stdin);
                strcat(msg, msg1);
                echoStringLen = strlen(msg);
                if(send(chatSock, msg, echoStringLen, 0) != echoStringLen){
                    DieWithError("send() sent a different number of bytes than expected!");}
                if(strcmp(msg, "Bye\n")==0) break;
                if ((recvMsgSize = recv(chatSock, msg, 500, 0)) < 0)
                    DieWithError("recv() failed");
                msg[recvMsgSize] = '\0';
                printf("%s: %s", user2, msg);
                if(strcmp(msg, "Bye\n")==0) break;
                memset(msg, 0, sizeof(msg));}
            close(chatSock);
        } else {
            exit(0);
        }
        printf("----------------------------\n");
    }

    printf("\n"); /* Print a final linefeed */

    close(sock);
    exit(0);
}
