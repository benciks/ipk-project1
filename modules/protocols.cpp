#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <csignal>
#include <cstring>
#include <iostream>

#define BUFSIZE_UDP 512
#define BUFSIZE_TCP 1024

int tcpSoc;

void signal_handler(int signal) {
  char buf[BUFSIZE_UDP];
  memset(buf, 0, BUFSIZE_UDP);

  send(tcpSoc, "BYE\n", 4, 0);
  recv(tcpSoc, buf, BUFSIZE_TCP, 0);
  std::cout << std::endl << buf;

  close(tcpSoc);
  exit(EXIT_SUCCESS);
}

void handleTCP(int clientSoc, sockaddr_in servAddress) {
  // We need global variable in order to close the connection in signal handler
  signal(SIGINT, signal_handler);
  tcpSoc = clientSoc;

  char buf[BUFSIZE_TCP];
  int bytesTx, bytesRx;

  // Estabilish the TCP connection
  if (connect(clientSoc, (const struct sockaddr*)&servAddress,
              sizeof(servAddress)) != 0) {
    std::cerr << "ERROR: connect";
    exit(EXIT_FAILURE);
  }

  while (true) {
    // Clear buffer
    memset(buf, 0, BUFSIZE_TCP);
    fgets(buf, BUFSIZE_TCP, stdin);

    // Simply ignore empty lines - continue
    if (strcmp(buf, "\n") == 0) {
      continue;
    }

    // Send message
    bytesTx = send(clientSoc, buf, strlen(buf), 0);

    if (bytesTx < 0) {
      std::cerr << "ERROR: sendto";
    }

    // Clear buffer
    memset(buf, 0, BUFSIZE_TCP);

    // Receive some response!
    bytesRx = recv(clientSoc, buf, BUFSIZE_TCP, 0);

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
  char buf[BUFSIZE_UDP];
  char packet[BUFSIZE_UDP + 2];
  int bytesTx, bytesRx;

  while (true) {
    // Fill buffer and packet with zeros - memset is preffered over bzero
    memset(buf, 0, BUFSIZE_UDP);
    memset(packet, 0, BUFSIZE_UDP + 2);

    // Get input
    fgets(buf, BUFSIZE_UDP, stdin);

    // Simply ignore empty lines - continue
    if (strcmp(buf, "\n") == 0) {
      continue;
    }

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
    memset(packet, 0, BUFSIZE_UDP + 2);

    // Receive some response!
    bytesRx = recvfrom(clientSoc, packet, BUFSIZE_UDP, 0,
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
