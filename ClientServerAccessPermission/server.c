/*
Name: Ashita Jayesh Shetty
Student ID: 00001607967
Topic: COEN 233 Programming Assignment 2
Program name: server.c  
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

//Packet Primitives
#define SPACKETID 0XFFFF
#define EPACKETID 0XFFFF
#define CLIENTID 0XFF

//Packet Type
#define NOTPAID 0XFFF9
#define NOTEXIST 0XFFFA
#define PAID 0XFFFB

//struct for the requested packet
typedef struct idRequestedPacket{
	int packetID;
	int clientID;
	int permissionNo;
	int segNo;
	int len;
	int technology;
	long subscriberNo;
	int endPacketID;
} idRequestedPacket;

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

//struct for verfication database
typedef struct verficationTable{
	long subscriberNo;
	int technology;
	int paid;
}verficationTable;

idResponsePacket createResponsePacket(){
	idResponsePacket res;
	res.packetID = SPACKETID;
	res.clientID = CLIENTID;
	res.endPacketID = EPACKETID;
	return res;
}
void displayResponsePacket(idResponsePacket res){
	printf("\n\nSending response:\n");
	printf("Start of Packet ID : %hx\n",res.packetID);
	printf("Client id : %hhx\n",res.clientID);
	printf("Access Permission : %hx\n",res.permit);
	printf("Segment no : %d\n",res.segNo);
	printf("Length : %d\n",res.len);
	printf("Technology : %d\n",res.technology);
	printf("Subscriber Number : %ld\n",res.subscriberNo);
	printf("End of packet ID : %x\n",res.endPacketID);
}

//create an array of verification table
int readFile(verficationTable verify[]){
	char *buffer;
	size_t buffer_len =0;
	FILE *fp = fopen("Verification_Database.txt","r");
	int j =0;
    while( getline(&buffer,&buffer_len,fp) != -1){
    	//extract technology and subscriber number from each line
    	char *pch = strtok(buffer," ");
    	char word[2][255];
    	int i =0;
    	while (pch!=NULL){
    		strcpy(word[i],pch);
    		pch = strtok(NULL," ");
    		i++;
    	}
    	verify[j].subscriberNo = atol(word[0]);
    	verify[j].technology = atoi(word[1]);
    	verify[j].paid = atoi(word[2]);
    	j++;
    }
    return j;
}

int main(){
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;
	verficationTable verify[5];
	idRequestedPacket req;
	idResponsePacket res;
	int socketFd;
	int bindFd;
	int n =0;
	//create a socket
	socketFd = socket(AF_INET, SOCK_DGRAM, 0); 
	if(socketFd < 0){		// exit the program if server could not build the connection
		printf("Socket creation failed.");
		exit(EXIT_FAILURE);
	} else{
		printf("Staring Server..\n");
	}

	//configure the server address
	memset(&serverAddr,0,sizeof(serverAddr));	//clear the serverAddr before configuring
	memset(&clientAddr,0,sizeof(clientAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY; 
    serverAddr.sin_port = htons(PORT);

    //binding the server address to the socket created
    bindFd = bind(socketFd, (const struct sockaddr *) &serverAddr,sizeof(serverAddr)); 
    if (bindFd < 0){
    	printf("Failure to bind sucessfully");
    	exit(EXIT_FAILURE);
    }

    int noOfRows = readFile(verify);

    while(1){
    	socklen_t len = sizeof(clientAddr);
    	n = recvfrom(socketFd,&req, sizeof(idRequestedPacket), 0, ( struct sockaddr *) &clientAddr, &len); 
    	res = createResponsePacket();
    	res.subscriberNo = req.subscriberNo;
    	res.technology = req.technology;
    	res.segNo = req.segNo;
    	res.len = req.len;
    	bool existsFlag = false;
    	//find the number on the database
    	for(int i =0; i<noOfRows; i++){
    		if(req.subscriberNo == verify[i].subscriberNo && req.technology == verify[i].technology){
    			if(verify[i].paid == 1){
    				res.permit = PAID;
    				//printf("\n\nSubscriber %ld is allowed to use the network\n",req.subscriberNo);
    			} else{
    				//printf("\n\nSubscriber %ld is not allowed to use the network\n",req.subscriberNo);
    				res.permit = NOTPAID;
    			}
    			existsFlag = true;
    			break;
    		}
    	}
    	if(!existsFlag)
    	{
    		//printf("\n\nSubscriber %ld does not exist\n",req.subscriberNo);
    		res.permit = NOTEXIST;
    	}
    	
    	displayResponsePacket(res);

    	sendto(socketFd, &res, sizeof(res),0, (const struct sockaddr *) &clientAddr,sizeof(clientAddr));
    }
}