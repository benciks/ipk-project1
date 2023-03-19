#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

#include "modules/protocols.h"

int main(int argc, const char* argv[]) {
  int port;
  const char* hostname;
  const char* mode;
  socklen_t serverLength;

  // Process arguments - loop as they can be in different order
  for (int i = 1; i < argc; i++) {
    if (!argv[i + 1]) {
      continue;
    }
    if (strcmp(argv[i], "-h") == 0) {
      hostname = argv[i + 1];
    }
    if (strcmp(argv[i], "-p") == 0) {
      port = std::stoi(argv[i + 1]);
      if (port > 65535 || port < 0) {
        std::cerr << "ERROR: Port out of range: " << port;
        exit(EXIT_FAILURE);
      }
    }
    if (strcmp(argv[i], "-m") == 0) {
      if (strcmp(argv[i + 1], "udp") != 0 && strcmp(argv[i + 1], "tcp") != 0) {
        std::cerr << "ERROR: Invalid mode: " << argv[i + 1];
        exit(EXIT_FAILURE);
      }

      mode = argv[i + 1];
    }
  }

  // Check if all parameters are set - if not, display usage and quit
  if (!port || !hostname || !mode) {
    std::cerr << "ERROR: Invalid amount of arguments \n";
    std::cout << "Usage: ./ipkcpc -h <host> -p <port> -m <mode> \n";
    exit(EXIT_FAILURE);
  }

  // Get hostname and check if it's valid
  struct hostent* server = gethostbyname(hostname);

  if (server == NULL) {
    std::cerr << "ERROR: Invalid host";
    exit(EXIT_FAILURE);
  }

  // Initialise serverAddress struct
  struct sockaddr_in serverAddress;
  serverLength = sizeof(serverAddress);

  bzero((char*)&serverAddress, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  bcopy((char*)server->h_addr, (char*)&serverAddress.sin_addr.s_addr,
        server->h_length);
  serverAddress.sin_port = htons(port);

  // Create socket, socket type determined by protocol
  std::cout << "INFO: Server socket: " << inet_ntoa(serverAddress.sin_addr)
            << ":" << ntohs(serverAddress.sin_port) << "\n";

  int socketType = strcmp(mode, "udp") == 0 ? SOCK_DGRAM : SOCK_STREAM;
  int clientSocket = socket(AF_INET, socketType, 0);

  if (clientSocket <= 0) {
    std::cerr << "ERROR: socket";
    exit(EXIT_FAILURE);
  }

  // Handle communication based on mode
  if (strcmp(mode, "tcp") == 0) {
    handleTCP(clientSocket, serverAddress);
  } else {
    handleUDP(clientSocket, serverAddress, serverLength);
  }

  return EXIT_SUCCESS;
}
