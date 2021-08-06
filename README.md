

# Description
Client connects to the server using customized protocol on top of UDP protocol

# Files in this repository
1. client.c - client code which will send packets to the server using socket connection.
2. server.c - server code which establishes the connection, accepts packets from the client and sends acknowledgement or reject packet.
3. sample.txt - a txt file containing data to send.

# Installation

For linux machines:
Install a gcc compiler to compile the C program and to run it.

For Windows:
Install a Virtual Machine with Linux as guest OS. Install a gcc complier to the guest OS to complie the C program.

# Steps to compile and run the programs:

1. Open two terminals on Linux
2. On terminal 1 compile the server program using following command:

  `gcc server.c -o server`

3. On terminal 2 compile the client program:

  `	gcc client.c -o client`
  
4. Make sure that sample.txt is in the same folder as the client.
5. Run the server program on terminal 1:

  `	./server`
  
6. Run the client program on terminal 2:

  `	./client`
  
7. The packets will be sent from client to server.
