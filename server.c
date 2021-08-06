/*
Name: Ashita Jayesh Shetty
Student ID: 00001607967
Topic: COEN 233 Programming Assignment 1
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

//Define the packet type
#define DATA 0XFFF1
#define ACK  0XFFF2
#define REJECT 0XFFF3

//Define the primitives
#define SPACKETID 0XFFFF
#define EPACKETID 0XFFFF
#define CLIENTID 0XFF

//Define the reject codes
#define OUTOFSEQ 0XFFF4
#define LENMISMATCH 0XFFF5
#define ENDOFPACKETMISSING 0XFFF6
#define DUPPACKET 0XFFF7

typedef struct dataPacket{
	int packetID;
	int clientID;
	int type;
	int segNo;
	int len;
	char payload[255];
	int endPacketID;
} dataPacket;


typedef struct ackPacket{
	int packetID;
	int clientID;
	int type;
	int segNo;
	int endPacketID;
} ackPacket;

typedef struct rejectPacket{
	int packetID;
	int clientID;
	int type;
	int rejectCode;
	int segNo;
	int endPacketID;
} rejectPacket;

//display the received packet
void displayData(dataPacket data){
	printf("\n\nReceived packet:\n");
	printf("Start of Packet ID : %x\n",data.packetID);
	printf("Client id : %x\n",data.clientID);
	printf("Data type : %x\n",data.type);
	printf("Segment no : %d\n",data.segNo);
	printf("Length : %d\n",data.len);
	printf("Payload : %s",data.payload);
	printf("End of packet ID : %x\n",data.endPacketID);
}

//create an acknowledgment packet
ackPacket createAckPacket(){
	ackPacket ack;
	ack.packetID = SPACKETID;
	ack.clientID = CLIENTID;
	ack.type = ACK;
	ack.endPacketID = EPACKETID;
	return ack;
}

//create a reject packet
rejectPacket createRejPacket(){
	rejectPacket rejectP;
	rejectP.packetID = SPACKETID;
	rejectP.clientID = CLIENTID;
	rejectP.type = REJECT;
	rejectP.endPacketID = EPACKETID;
	return rejectP;
}

int main(){
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;
	int socketFd;
	int bindFd;
	int n;
	int receivedSeg = 1;
	char buffer[255];
	dataPacket data;
	ackPacket ack;
	rejectPacket rejectP;
	bool receivedFlag[20] = {false}; //intialize the flag to false

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

    while(1){
    	socklen_t len = sizeof(clientAddr);
    	n = recvfrom(socketFd,&data, sizeof(dataPacket), 0, ( struct sockaddr *) &clientAddr, &len); 
    	displayData(data);

    	//error handling

    	// duplicate packet
    	if(receivedFlag[data.segNo]){
    		rejectP = createRejPacket();
    		rejectP.rejectCode = DUPPACKET;
    		printf("Error: Duplicate packet received. Sending a reject packet to client\n");
    		sendto(socketFd, &rejectP, sizeof(rejectP),0, (const struct sockaddr *) &clientAddr,sizeof(clientAddr));
    	}

    	// packet is out of sequence
    	else if(receivedSeg != data.segNo){
    		rejectP = createRejPacket();
    		rejectP.rejectCode = OUTOFSEQ;
    		printf("Error: Packets recieved are out of sequence. Sending a reject packet to client\n");
    		sendto(socketFd, &rejectP, sizeof(rejectP),0, (const struct sockaddr *) &clientAddr,sizeof(clientAddr));
    	}

    	// length does not match with the payload sent.
    	else if(strlen(data.payload) != data.len){
    		rejectP = createRejPacket();
    		rejectP.rejectCode = LENMISMATCH;
    		printf("Error: Length of the payload does not match. Sending a reject packet to client\n");
    		sendto(socketFd, &rejectP, sizeof(rejectP),0, (const struct sockaddr *) &clientAddr,sizeof(clientAddr));
    	}

    	// end of packet ID is missing
    	else if(data.endPacketID != EPACKETID){
    		rejectP = createRejPacket();
    		rejectP.rejectCode = ENDOFPACKETMISSING;
    		printf("Error: End of packet ID is missing. Sending a reject packet to client\n");
    		sendto(socketFd, &rejectP, sizeof(rejectP),0, (const struct sockaddr *) &clientAddr,sizeof(clientAddr));
    	}

    	// send acknowledgement
    	else{
    		if(data.segNo == 10){	//simulate ACK timer
    			sleep(10);
    		}
			ack = createAckPacket();
	    	ack.segNo = data.segNo;
	    	printf("Sending acknowledgment\n");
	    	sendto(socketFd, &ack, sizeof(ack),0, (const struct sockaddr *) &clientAddr,sizeof(clientAddr));
	    	receivedSeg++;
	    	receivedFlag[data.segNo] = true;	// keep tab of all the packets received.
    	}	
    	
    }  
    return 0;
}