#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <openssl/sha.h>
#include "StringEncoder.c"
void *connection_handler(void *);

int main(){
  int welcomeSocket, newSocket;
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;
  pthread_t clientThread;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(7891);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*---- Bind the address struct to the socket ----*/
  bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  /*---- Listen on the socket, with 5 max connection requests queued ----*/
  if(listen(welcomeSocket,5)==0)
    printf("Listening\n");
  else
    printf("Error\n");

  /*---- Accept call creates a new socket for the incoming connection ----*/
  while(1){

    addr_size = sizeof serverStorage;
    newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
    printf("%d\n",newSocket);
    //close(welcomeSocket);


    if(pthread_create(&clientThread,NULL,connection_handler,(void *)&newSocket)){
      fprintf(stderr, "error creating thread");
      return 1;
    }
  }
  return 0;
}

void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char exit[] = "exit";
    char client_message[4096];
    client_message[0] = '\0';
    char buf2[SHA_DIGEST_LENGTH*2];
    char * serverEncodedString;

    unsigned char hash[SHA_DIGEST_LENGTH];



    printf("Inside of thread, thread ID of % d \n",sock);

    while(1){


      recv(sock , client_message , 4096 , 0); 
      if(strncmp(client_message,exit,4) == 0){
        send(sock,client_message,strlen(client_message),0);
        break;
      }

      char * s;
      s = strtok(client_message,"\n");
      memset(buf2,0x0, SHA_DIGEST_LENGTH*2);
      memset(hash,0x0,SHA_DIGEST_LENGTH);
      SHA1(s,strlen(s),hash);
      int i;
      for (i=0; i < SHA_DIGEST_LENGTH; i++) {
         sprintf((char*)&(buf2[i*2]), "%02x", hash[i]);
      }
      printf("Server Hash %s\n", buf2);
      serverEncodedString = stringToEncodedAscii(buf2);
      s = strtok(NULL,"\0");
      printf("Client Encoded String: %s\n", s);
      printf("Server Encoded String: %s\n", serverEncodedString);
      if(strcmp(s,serverEncodedString) == 0){
        char message[] = "true";
        send(sock,message,strlen(message),0);

      }else{
        char message[] = "false";
        send(sock,message,strlen(message),0);

      }
      free(serverEncodedString);
      memset(&client_message[0], 0, sizeof(client_message));

    }
    close(sock);
    pthread_exit(0);

    return 0;
}
