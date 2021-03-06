#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <openssl/sha.h>
#include <stdlib.h>
#include "StringEncoder.c"


int main(int argn, char *argv[]){
  int clientSocket;
  int nbytes = 100;
  char buf[SHA_DIGEST_LENGTH*2];
  char * input;
  unsigned char hash[SHA_DIGEST_LENGTH];
  struct sockaddr_in serverAddr;
  socklen_t addr_size;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(7891);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*---- Connect the socket to the server using the address struct ----*/
  addr_size = sizeof serverAddr;
  connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
  char exit[] = "exit";
  while(1){

    char buffer[4096];
    memset(&buffer[0], 0, sizeof(buffer));

    char * joinedStr;

    char * encodedHash;
    input = (char *)malloc(100 + 1);
    memset(buf,0x0, SHA_DIGEST_LENGTH*2);
    memset(hash,0x0,SHA_DIGEST_LENGTH);
    getline(&input,&nbytes,stdin);
    if(strncmp(input,exit,4) == 0){
        send(clientSocket, input,strlen(input),0);
    }else{
      SHA1(input,strlen(input) - 1,hash);
       int i;
       for (i=0; i < SHA_DIGEST_LENGTH; i++) {
           sprintf((char*)&(buf[i*2]), "%02x", hash[i]);
       }
      encodedHash = stringToEncodedAscii(buf);
      joinedStr = (char *)malloc(strlen(input) + strlen(encodedHash) + 1);
      strcpy(joinedStr,input);
      strcat(joinedStr,encodedHash);
      send(clientSocket, joinedStr,strlen(joinedStr),0);
      free(joinedStr);
      free(encodedHash);
      free(input);
    }

    recv(clientSocket, buffer, 4096, 0);
    if(strncmp(buffer,exit,4) == 0){
      free(input);
      break;
    }


    /*---- Print the received message ----*/
    printf("Data received: %s \n",buffer); 
    memset(&buffer[0], 0, sizeof(buffer));
   // fflush(stdin);
    //fflush(stdout);
  }
  
  return 0;
}
