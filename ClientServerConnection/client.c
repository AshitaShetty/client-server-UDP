/*
Name: Ashita Jayesh Shetty
Student ID: 00001607967
Topic: COEN 233 Programming Assignment 1
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


typedef struct recievedPacket{
	int packetID;
	int clientID;
	int type;
	int rejectCode;
	int segNo;
	int endPacketID;
} recievedPacket;

// create a data packet and set the primitives.
dataPacket createDataPacket(){
	dataPacket data;
	data.packetID = SPACKETID;
	data.clientID = CLIENTID;
	data.type = DATA;
	data.endPacketID = EPACKETID;
	return data;
}

void displayDataToSend(dataPacket data){
	printf("\n\nSending packet:\n");
	printf("Start of Packet ID : %x\n",data.packetID);
	printf("Client id : %x\n",data.clientID);
	printf("Data type : %x\n",data.type);
	printf("Segment no : %d\n",data.segNo);
	printf("Length : %d\n",data.len);
	printf("Payload : %s",data.payload);
	printf("End of packet ID : %x\n",data.endPacketID);
}


int main(){
	dataPacket data;
	recievedPacket recievedP;
	FILE *fp;
	char *filename = "sample.txt";
	struct sockaddr_in serverAddr;
	int socketFd;
	char *buffer;
	size_t buffer_len =0;

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

    //create data packets
    data = createDataPacket();
    int segmentNo =1; //initialize to 1;
    
    
    //read from the file and send each line to the server
    fp = fopen(filename,"r");
    while( getline(&buffer,&buffer_len,fp) != -1){
    	
    	bool sendRightPacket = false;
    	bool isAcked = false;
    	while(!isAcked){
    		data.segNo = segmentNo;
	    	strcpy(data.payload,buffer);
	    	data.len= strlen(data.payload);
	    	data.endPacketID = EPACKETID;
	    	int n =0;
	    	int counter =0;
    	//Simulate error codes
	    	if(!sendRightPacket){
		    	switch(segmentNo){
		    		//create a out of sequence packet
		       		case 6:
		    			data.segNo = segmentNo+10;
		    			break;

		    		//create length mismatch
		    		case 7:
		    			data.len++;
		    			break;

		    		//create end of packet missing error
		    		case 8:
		    			data.endPacketID = 0;
		    			break;

		    		//create duplicate packet
		    		case 9:
		    			data.segNo = 2;
		    			break;
		    	}
			}
	    	displayDataToSend(data);	  
	    	while(n <= 0 && counter < 3){
		    	sendto(socketFd, &data, sizeof(dataPacket), 0, (const struct sockaddr *) &serverAddr, sizeof(serverAddr));
				
				//recieve acknowledgement
				socklen_t len = sizeof(serverAddr);
				n = recvfrom(socketFd,&recievedP,sizeof(recievedPacket),0,NULL,NULL);
				
				//If no acknowledgment recieved, update the counter
				if(n <=0){
					printf("\nWaiting for server to respond, trying again..");
					counter++;
					continue;
				}
				switch(recievedP.type){
					case ACK:
						printf("Acknowledment recieved\n");
						//sendRightPacket = false;
						sleep(2);
						isAcked = true;
						segmentNo++;
						break;
					case REJECT:
						printf("Reject error recieved: %x",recievedP.rejectCode);
						switch(recievedP.rejectCode){
							case OUTOFSEQ:
								printf("\nError: Packets are out of sequence");
								break;
							case LENMISMATCH:
								printf("\nError: Length mismatch");
								break;
							case ENDOFPACKETMISSING:
								printf("\nError: End of packet ID is missing");
								break;
							case DUPPACKET:
								printf("\nError: Duplicate packets recieved");
								break;
						}
						sendRightPacket = true;
						printf("\nSending the packet again.\n");
						sleep(2);
						break;
				}
		    }

		    // If acknowledgement not recieved after 3 tries, exit the program.
		    if(counter == 3){
		    	printf("\n Failed connecting to the server, exiting the program\n");
		    	exit(EXIT_FAILURE);
		    }		
		}
	}
	fclose(fp);
    close(socketFd); 
    return 0; 
 }