#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <iostream>
#include <string>

#define BUFSIZE 512

int main(int argc, const char *argv[]) {
  int port;
  const char *hostname;
  const char *mode;
  char buf[BUFSIZE];

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
  struct hostent *server = gethostbyname(hostname);

  if (server == NULL) {
    std::cerr << "ERROR: Invalid host";
    exit(EXIT_FAILURE);
  }

  // Initialise serverAddress struct
  struct sockaddr_in serverAddress;

  bzero((char *)&serverAddress, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serverAddress.sin_addr.s_addr,
        server->h_length);
  serverAddress.sin_port = htons(port);

  // Print info about socket
  std::cout << "INFO: Server socket: " << inet_ntoa(serverAddress.sin_addr)
            << ":" << ntohs(serverAddress.sin_port);

  int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

  if (clientSocket <= 0) {
    std::cerr << "ERROR: socket";
    exit(EXIT_FAILURE);
  }

  bzero(buf, BUFSIZE);
  std::cout << "\nPlease enter message: ";
  fgets(buf, BUFSIZE, stdin);

  std::cout << "BUFFER: " << buf;

  if (strcmp(mode, "udp") == 0) {
    // UDP
    std::cout << "udp";

  } else {
    // TCP
    std::cout << "tcp";
  }

  return EXIT_SUCCESS;
}
