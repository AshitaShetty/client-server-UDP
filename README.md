

# Description
Client connects to the server using customized protocol on top of UDP protocol

# Files in this repository
client.c - client code which will send packets to the server using socket connection.

server.c - server code which establishes the connection, accepts packets from the client and sends acknowledgement or reject packet.

sample.txt - a txt file containing data to send.

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
