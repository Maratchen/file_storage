# Test task solution

The task is to write two console applications Client and Server that exchange files via UDP socket with TCP handshake.

## 1. Server.
The first parameter is IP, the second is the port number, the third is the directory for storing files.
The server starts listening on the IP address of the port and waits for the client to connect via TCP.
Example:
> Server.exe 127.0.0.1 5555 temp

## 2. Client.
Running with 5 parameters. Parameters 1 and 2 are IP address and port for connecting to the server. The third is the port for sending UDP packets. The fourth is the path to the file. The fifth one is the timeout for acknowledgment of UDP packets in milliseconds.
Example:
> Client.exe 127.0.0.1 5555 6000 test.txt 500

## 3. Interaction between server and client.
At the beginning, the server opens a TCP socket with the IP address and port from the command parameters (parameters 1 and 2), takes it to listen and waits for connections from the client.

The client is trying to connect to the server via TCP to the IP address and port from the command parameters (parameters 1 and 2) until a connection is established. After the connection is established, the client loads the file into memory (parameter 3, the file size is no more than 10 MB) and sends the file name and UDP port to the server over a TCP connection.

After that, the client starts sending the file in blocks (Each data block in udp packets must contain its own id) udp datagrams and receive acknowledgment of acceptance by the server via TCP.
If during the timeout (parameter 5) no confirmation is received for the packet, then the packet is resent via UDP. In the case when all packets are acknowledged, the client notifies the TCP server of the end of the file transfer, closes the TCP connection, exits.

The server, after establishing a client connection via TCP, receiving the file name and udp port, opens an udp socket and starts receiving udp packets outgoing from the client's IP address and the port transmitted by the client.
For each received udp packet, the server sends an acknowledgment via a tcp socket to the client. Received packets are stored in memory. After receiving a notification from the client about the end of the transfer (all packets are acknowledged on the client), the server saves the file to the directory (parameter 3).

# How to build
1. Create a build directory to store the build artefacts.
> mkdir build && cd build
2. Prepare all dependencies. Using conan or other package manager is recommended.
> conan install .. --build=missing
3. Build the project.
> cmake .. -DCMAKE_TOOLCHAIN_FILE=./conan_paths.cmake
> cmake --build .