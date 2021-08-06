

# Description
## Client Server Connection
Client connects to the server using customized protocol on top of UDP protocol

## Client Server Access Permission 
Client uses customized protocol on top of UDP protocol for requesting identification from server for access permission to the network. 

# Files in this repository
## Client Server Connection
client.c - client code which will send packets to the server using socket connection.

server.c - server code which establishes the connection, accepts packets from the client and sends acknowledgement or reject packet.

sample.txt - a txt file containing data to send.

## Client Server Access Permission 
client.c - client code which will send packets with subscriber number and technology to the server using socket connection.

server.c - server code which establishes the connection, accepts packets from the client and checks if the subscriber is a paid subscriber or not. Sends the data back to client in a response packet.

listOfUsers.txt - a txt file containing list of subscribers to send.

Verification_Database.txt - a txt file containing list of subscribers with paid or not paid status for a technology.

# Installation

For linux machines:
Install a gcc compiler to compile the C program and to run it.

For Windows:
Install a Virtual Machine with Linux as guest OS. Install a gcc complier to the guest OS to complie the C program.

# Steps to compile and run the programs:

Open two terminals on Linux

On terminal 1 compile the server program using following command:

  `gcc server.c -o server`

On terminal 2 compile the client program:

  `	gcc client.c -o client`
  
Make sure that sample.txt is in the same folder as the client.

Run the server program on terminal 1:

  `	./server`
  
Run the client program on terminal 2:

  `	./client`
  
The packets will be sent from client to server.
