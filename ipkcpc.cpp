#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>

#define BUFSIZE_UDP 512
#define BUFSIZE_TCP 1024

void handleTCP(int clientSoc, sockaddr_in servAddress, socklen_t servLen) {
  char buf[BUFSIZE_TCP];
  int bytesTx, bytesRx;

  // Connect
  if (connect(clientSoc, (const struct sockaddr*)&servAddress,
              sizeof(servAddress)) != 0) {
    std::cerr << "ERROR: connect";
    exit(EXIT_FAILURE);
  }

  while (true) {
    // Clear buffer
    bzero(buf, BUFSIZE_TCP);
    fgets(buf, BUFSIZE_TCP, stdin);

    // Send message
    bytesTx = send(clientSoc, buf, strlen(buf), 0);

    if (bytesTx < 0) {
      std::cerr << "ERROR: sendto";
    }

    // Clear buffer
    bzero(buf, BUFSIZE_TCP);

    // Receive some response!
    bytesRx = recv(clientSoc, buf, BUFSIZE_TCP, 0);

    if (bytesRx < 0) {
      std::cerr << "ERROR: recvfrom";
    }

    std::cout << strlen(buf);

    if (strcmp(buf, "BYE\n") == 0) {
      break;
    }
  }

  // Close the socket
  close(clientSoc);
  exit(EXIT_SUCCESS);
}

int main(int argc, const char* argv[]) {
  int port, bytesTx, bytesRx;
  const char* hostname;
  const char* mode;
  socklen_t serverLength;

  // Process arguments
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

  // Check if all parameters are set
  if (!port || !hostname || !mode) {
    std::cerr << "ERROR: Invalid amount of arguments \n";
    std::cout << "Usage: ./ipkcpc -h <host> -p <port> -m <mode> \n";
    exit(EXIT_FAILURE);
  }

  // Get hostname
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

  // Create socket
  std::cout << "INFO: Server socket: " << inet_ntoa(serverAddress.sin_addr)
            << ":" << ntohs(serverAddress.sin_port) << "\n";

  int socketType = strcmp(mode, "udp") == 0 ? SOCK_DGRAM : SOCK_STREAM;
  int clientSocket = socket(AF_INET, socketType, 0);

  if (clientSocket <= 0) {
    std::cerr << "ERROR: socket";
    exit(EXIT_FAILURE);
  }

  // Connect if tcp
  if (strcmp(mode, "tcp") == 0) {
    handleTCP(clientSocket, serverAddress, serverLength);
  }

  return EXIT_SUCCESS;
}
