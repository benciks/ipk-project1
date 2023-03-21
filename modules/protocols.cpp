#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <csignal>
#include <cstring>
#include <iostream>

#define BUFSIZE 1024

int soc;
bool tcpMode = false;

void signal_handler(int) {
  // Send bye if TCP
  if (tcpMode) {
    char buf[BUFSIZE];
    memset(buf, 0, BUFSIZE);

    send(soc, "BYE\n", 4, 0);
    recv(soc, buf, BUFSIZE, 0);
    std::cout << std::endl << buf;
  }

  // In both cases, close the socket
  close(soc);
  exit(EXIT_SUCCESS);
}

void handleTCP(int clientSoc, sockaddr_in servAddress) {
  // We need global variable in order to close the connection in signal handler
  signal(SIGINT, signal_handler);
  soc = clientSoc;
  tcpMode = true;

  char buf[BUFSIZE];
  int bytesTx, bytesRx;

  // Estabilish the TCP connection
  if (connect(clientSoc, (const struct sockaddr*)&servAddress,
              sizeof(servAddress)) != 0) {
    std::cerr << "ERROR: connect";
    exit(EXIT_FAILURE);
  }

  while (true) {
    // Clear buffer
    memset(buf, 0, BUFSIZE);
    fgets(buf, BUFSIZE, stdin);

    // Send message
    bytesTx = send(clientSoc, buf, strlen(buf), 0);

    if (bytesTx < 0) {
      std::cerr << "ERROR: sendto";
    }

    // Clear buffer
    memset(buf, 0, BUFSIZE);

    // Receive some response!
    bytesRx = recv(clientSoc, buf, BUFSIZE, 0);

    if (bytesRx < 0) {
      std::cerr << "ERROR: recvfrom";
    }

    std::cout << buf;

    // Break the loop if BYE is received
    if (strcmp(buf, "BYE\n") == 0) {
      break;
    }
  }

  // Close connection and exit
  close(clientSoc);
}

void handleUDP(int clientSoc, sockaddr_in servAddress, socklen_t servLen) {
  signal(SIGINT, signal_handler);
  soc = clientSoc;
  char buf[BUFSIZE];
  char packet[BUFSIZE + 2];
  int bytesTx, bytesRx;

  while (true) {
    // Fill buffer and packet with zeros - memset is preffered over bzero
    memset(buf, 0, BUFSIZE);
    memset(packet, 0, BUFSIZE + 2);

    // Get input
    fgets(buf, BUFSIZE, stdin);

    // Set first two bytes to Opcode and Payload length
    size_t len = strlen(buf) + 2;
    packet[0] = 0x00;
    packet[1] = len;
    memcpy(packet + 2, buf, len);

    // Send message
    bytesTx = sendto(clientSoc, packet, len, 0, (struct sockaddr*)&servAddress,
                     servLen);

    if (bytesTx < 0) {
      std::cerr << "ERROR: sendto";
    }

    // Clear packet before receiving response
    memset(packet, 0, BUFSIZE + 2);

    // Receive some response!
    bytesRx = recvfrom(clientSoc, packet, BUFSIZE, 0,
                       (struct sockaddr*)&servAddress, &servLen);

    if (bytesRx < 0) {
      std::cerr << "ERROR: recvfrom";
    }

    if (!packet[1]) {
      printf("OK:%s\n", packet + 3);
    } else {
      printf("ERR:%s\n", packet + 3);
    }
  }
}
