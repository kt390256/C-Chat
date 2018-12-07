#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h> /* for close() */
#define RCVBUFSIZE 500 /* Size of receive buffer */
//void DieWithError(char *errorMessage); /* Error handling function */

void HandleTCPClient(int clntSock)
{
  int msgNo = 0;
  int clientNo = 0,login = 0, i, echoStrLen;
  long recvMsgSize = 1;
  char users[2][20], passwords[2][20], option[2], username[30], password[30], recipient[100], sender[100], msg[1000], msgStat[100], echoBuffer[RCVBUFSIZE], sizeBuffer[10];
  strcpy(users[0], "Alice");
  strcpy(users[1], "Bob");
  strcpy(passwords[0], "12345");
  strcpy(passwords[1], "56789");

    if ((recvMsgSize = recv(clntSock, option, 2, 0)) < 0)
        DieWithError("recv() failed");
    option[recvMsgSize] = '\0';

    /* Send received string and receive again until end of transmission */
    while (recvMsgSize > 0) /* zero indicates end of transmission */
    {
        /* Echo message back to client */
        memset(echoBuffer, 0, RCVBUFSIZE);
        if(login ==1){
            if ((recvMsgSize = recv(clntSock, option, 2, 0)) < 0){
                DieWithError("recv() failed");
              }
            }

        if(strcmp(option, "0")==0){
            if ((recvMsgSize = recv(clntSock, username, RCVBUFSIZE, 0)) < 0)
                DieWithError("recv() failed: unverifiable un") ;
            printf("Login username is %s\n", username);
            if ((recvMsgSize = recv(clntSock, password, RCVBUFSIZE, 0)) < 0)
                DieWithError("recv() failed: unverifiable pw");
            printf("Login password is %s\n", password);
            strcpy(echoBuffer, "login fail\n");
            for(i = 0; i < 2; i++){
                if(strcmp(username, users[i])==0){
                    if(strcmp(password, passwords[i]) == 0){
                        strcpy(echoBuffer, "login success\n");
                        login=1;}}}
            printf("%s", echoBuffer);
            echoStrLen = strlen(echoBuffer);
            if (send(clntSock, echoBuffer, echoStrLen, 0) != echoStrLen){
                DieWithError("send() failed: Login Status");}
        }
        else if(strcmp(option, "1")==0){
            snprintf(sizeBuffer, 10, "%d", i);
            strcpy(echoBuffer, "There are ");
            strcat(echoBuffer, sizeBuffer);
            strcat(echoBuffer, " users:\n");
            for(i=0; i< 2; i++){
                strcat(echoBuffer, users[i]);
                strcat(echoBuffer, "\n");}
            echoStrLen = strlen(echoBuffer);
            if(send(clntSock, echoBuffer, echoStrLen, 0) != echoStrLen)
                DieWithError("send() failed");
            printf("Return User List!\n");
        } else if(strcmp(option, "2")==0){
            if ((recvMsgSize = recv(clntSock, recipient, RCVBUFSIZE, 0)) < 0){
                DieWithError("recv() failed: unverifiable rec");}
            strcpy(sender, username);
            if ((recvMsgSize = recv(clntSock, msg, RCVBUFSIZE, 0)) < 0){
                DieWithError("recv() failed: unverifiable msg");}
            msg[recvMsgSize] = '\0';
            printf("A message to %s\n", recipient);
            printf("Message is %s\n", msg);
            strcpy(msgStat, "Message sent successfully!");
            echoStrLen = strlen(msgStat);
            msgStat[strlen(msgStat)] = '\0';
            if(send(clntSock, msgStat, strlen(msgStat), 0) != strlen(msgStat))
                DieWithError("send() failed");
            memset(msgStat, 0, 100);
        } else if(strcmp(option, "3")==0){
            strcpy(echoBuffer, "You have ");
            if(strcmp(recipient, username)==0){
                msgNo+=1;
                char noS[2];
                snprintf(noS, 2, "%d", msgNo);
                strcat(echoBuffer, noS);
                strcat(echoBuffer, " message(s)!\n");
                strcat(echoBuffer, msg);
                memset(msg, 0, 1000);
            } else {
                strcat(echoBuffer, "0 message(s)!\n");
            }
            echoStrLen = strlen(echoBuffer);
            echoBuffer[echoStrLen] = '\0';
            if(send(clntSock, echoBuffer, echoStrLen, 0) != echoStrLen){
                DieWithError("send() failed");}
            msgNo = 0;
            printf("Sent back %s's message!\n", recipient);
        }
    }
    printf("Client disconnected!\n");
    close(clntSock);
    memset(username, 0, sizeof username);
    memset(password, 0, sizeof password);

    login = 0;
}
