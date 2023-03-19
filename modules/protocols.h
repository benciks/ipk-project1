// Handles the sigint signal
void signal_handler(int signal);

// Handles the TCP protocol communication - connect, send and receive messages
void handleTCP(int clientSoc, sockaddr_in servAddress);

// Handles UDP protocol communication
void handleUDP(int clientSoc, sockaddr_in servAddress, socklen_t servLen);