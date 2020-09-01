#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <errno.h>
#define DEST_PORT 3000
#define SERVER_IP_ADDRESS "127.0.0.1" // localhost
struct test_struct_t
{
    unsigned int a;
    unsigned int b;
};
struct result_struct_t
{
    unsigned int c;
};
test_struct_t client_data;
result_struct_t result;

void SetupTCPClient()
{
    // Step 1. Initialization
    int sock_fd = 0, sent_recv_bytes = 0, addr_len = 0;
    addr_len = sizeof(struct sockaddr);
    struct sockaddr_in dest;

    // Step 2. Specify server information
    dest.sin_family = AF_INET;
    dest.sin_port = DEST_PORT;
    struct hostent *host = (struct hostent *)gethostbyname(SERVER_IP_ADDRESS);
    dest.sin_addr = *((struct in_addr *)host->h_addr_list[0]);

    // Step 3. Create a TCP socket
    sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // OS automatically assigns random port number to client
    if (connect(sock_fd, (struct sockaddr *)&dest, sizeof(struct sockaddr)) < 0)
    {
        exit(EXIT_FAILURE);
    }

    while (true)
    {
        // Step 4. Receive user input
        std::cout << "Enter a: ";
        std::cin >> client_data.a;
        std::cout << "Enter b: ";
        std::cin >> client_data.b;

        // Step 5. Send data to server
        sent_recv_bytes = sendto(sock_fd, &client_data, sizeof(test_struct_t), 0, (struct sockaddr *) &dest, sizeof(struct sockaddr));
        std::cout << "# of bytes sent: " << sent_recv_bytes << std::endl;

        // Step 6. Receive response from server
        sent_recv_bytes = recv(sock_fd, &result, sizeof(result_struct_t), 0);
        std::cout << "# of bytes received: " << sent_recv_bytes << std::endl;
        std::cout << "Result: " << result.c << std::endl;
    }
}
int main()
{
    SetupTCPClient();
    return EXIT_SUCCESS;
}