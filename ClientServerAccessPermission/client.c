/*
Name: Ashita Jayesh Shetty
Student ID: 00001607967
Topic: COEN 233 Programming Assignment 2
Program name: client.c  
*/
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <stdbool.h>
#define PORT 8080
#define ACK_TIMER 3

//Packet Primitives
#define SPACKETID 0XFFFF
#define EPACKETID 0XFFFF
#define CLIENTID 0XFF
#define ACC_PER 0XFFF8

//Packet Type
#define NOTPAID 0XFFF9
#define NOTEXIST 0XFFFA
#define PAID 0XFFFB

typedef struct idRequestPacket{
	int packetID;
	int clientID;
	int permissionNo;
	int segNo;
	int len;
	int technology;
	long subscriberNo;
	int endPacketID;
} idRequestPacket;

//struct for response packet
typedef struct idResponsePacket{
	int packetID;
	int clientID;
	int permit;
	int segNo;
	int len;
	int technology;
	long subscriberNo;
	int endPacketID;
} idResponsePacket;

idRequestPacket createIdRequestPacket(){
	idRequestPacket req;
	req.packetID = SPACKETID;
	req.clientID = CLIENTID;
	req.permissionNo = ACC_PER;
	req.endPacketID = EPACKETID;
	return req;
}

void displayRequestPacket(idRequestPacket req){
	printf("\n\nRequesting information on following packet:\n");
	printf("Start of Packet ID : %hx\n",req.packetID);
	printf("Client id : %hhx\n",req.clientID);
	printf("Access Permission : %hx\n",req.permissionNo);
	printf("Segment no : %d\n",req.segNo);
	printf("Length : %d\n",req.len);
	printf("Technology : %d\n",req.technology);
	printf("Subscriber Number : %ld\n",req.subscriberNo);
	printf("End of packet ID : %x\n",req.endPacketID);
}

int main(){
	struct sockaddr_in serverAddr;
	int socketFd;
	idRequestPacket req;
	idResponsePacket res;
	FILE *fp;
	char *filename = "listOfUsers.txt";
	char *buffer =NULL;
	size_t buffer_len =0;
	int segmentNo = 1;

	//create a socket
	socketFd = socket(AF_INET, SOCK_DGRAM, 0); 
	if(socketFd < 0){		// exit the program if server could not build the connection
		printf("Socket creation failed.");
		exit(EXIT_FAILURE);
	} else{
		printf("Starting Client..\n");
	}

	//configure the server address
	memset(&serverAddr,0,sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY; 
    serverAddr.sin_port = htons(PORT);
    
    //set socket timeout
    struct timeval timeout;      
    timeout.tv_sec = ACK_TIMER;
    timeout.tv_usec = 0;
    if (setsockopt (socketFd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout,sizeof(struct timeval)) < 0){
    	printf("Setting socket timeout failed\n");
    }

    req = createIdRequestPacket();
    fp = fopen(filename,"r");

    while(getline(&buffer,&buffer_len,fp) != -1){
    	//extract technology and subscriber number from each line
    	int n =0;
		int counter = 0;
    	char line[255];
    	strcpy(line,buffer);
    	char word[2][255];
    	int i =0;

    	char *pch = strtok(line," ");		
    	while(pch!=NULL){
    		strcpy(word[i],pch);
    		pch = strtok(NULL," ");
    		i++;
    	}

    	req.subscriberNo = atol(word[0]);
    	req.technology = atoi(word[1]);
    	req.segNo = segmentNo;
    	req.len = sizeof(req.subscriberNo)+sizeof(req.technology);
    	displayRequestPacket(req);
    	while(n <= 0 && counter < 3){   		
		    sendto(socketFd, &req, sizeof(idRequestPacket), 0, (const struct sockaddr *) &serverAddr, sizeof(serverAddr));
				
			//recieve acknowledgement
			socklen_t len = sizeof(serverAddr);
			n = recvfrom(socketFd,&res,sizeof(idResponsePacket),0,NULL,NULL);		

			//If no acknowledgment recieved, update the counter
			if(n <=0){
				printf("\nWaiting for server to respond, trying again..");
				counter++;
				continue;
			}
			printf("\nResponse from the server:\n");
			//Print the results
			switch(res.permit){
				case PAID:
					printf("Subscriber %ld permitted to access the network\n",req.subscriberNo);
					break;
				case NOTPAID:
					printf("Subscriber %ld has not paid\n",req.subscriberNo);
					break;
				case NOTEXIST:
					printf("Subscriber %ld does not exist on database\n",req.subscriberNo);
					break;
			}
		}
		// If acknowledgement not recieved after 3 tries, exit the program.
		if(counter == 3){
		    printf("\n Failed connecting to the server, exiting the program\n");
		    exit(EXIT_FAILURE);
		}
		segmentNo++;
		sleep(1);
    }
 }