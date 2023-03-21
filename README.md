# IPK Project 1 - TCP/UDP Client
Author: Šimon Benčík, xbenci01

## Intro
The goal of the project was to create a CLI client that allows communication with a server using TCP and UDP protocols. This documentation provides details about the project, including its implementation, design and testing.

## Theory
TCP (Transmission Control Protocol) and UDP (User Datagram Protocol) are protocols used in network communication, both of which operate on layer 4 of the OSI model. TCP provides reliable communication at the cost of being slower. On the other hand, UDP is conectionless, less reliable, but requires less moderation, which results in faster communication. Although you may question the purpose of UDP, it is largely used in video streaming or online gaming.

## Design
The client is written in C++ and uses socket programming. It is divided into a main entry file `ipkcpc.cpp` and one module called `protocols.cpp`, which includes functions for handling communication for each protocol respectively. The main file's purpose is to parse arguments and prepare the socket.

## Implementation
The application uses the standard socket library. After parsing arguments, which can be in any order, a hostname is checked for  correctness. The base data is then assigned to structure for handling access to internet. The last preparation step is creating the socket. Depending on the selected mode, either the **handleTCP** or **handleUDP** function is called. Both functions send and receive messages in the format required by the server. The TCP function connects to the server before sending messages. These functions also provide error handling at each step of communication. Lastly, a signal handler was implemented for SIGINT (Control-C) to gracefully end the TCP connection and close the socket in both protocols. Support for Windows OS was not implemented. All of the implementation was done on macOs and tested on provided virtual environment.

## Testing
Testing was done manually throughout the entire development process. You can find sample results of the testing in the images below.

<table>
  <tr>
    <th> TCP </th>
    <th> UDP </th>
  </tr>
  <tr>
  <td>
  <img src="images/tcp.png">
  </td>
  <td>
  <img src="images/udp.png">
  </td>
  </tr>
</table>

## Sources
- [IPK Projekty - Stubs](https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs)
- [Socket programming in C](https://www.geeksforgeeks.org/socket-programming-cc/)
- [A Simple Makefile Tutorial](https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/)
- [C++ Signal Handling](https://www.tutorialspoint.com/cplusplus/cpp_signal_handling.htm)
- [How to set socket timeout in C when making multiple connections?](https://stackoverflow.com/questions/4181784/how-to-set-socket-timeout-in-c-when-making-multiple-connections)