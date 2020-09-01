#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <errno.h>

#define MAX_CLIENT_SUPPORTED 32
#define SERVER_PORT 3000
int monitored_fd_set[MAX_CLIENT_SUPPORTED];
struct test_struct_t
{
    unsigned int a;
    unsigned int b;
};
struct result_struct_t
{
    unsigned int c;
};
void AddToFDSet(int fd)
{
    for (int i = 0; i < MAX_CLIENT_SUPPORTED; i++)
    {
        if (monitored_fd_set[i] != -1)
            continue;
        monitored_fd_set[i] = fd;
        break;
    }
}
void InitMonitorFDSet()
{
    for (int i = 0; i < MAX_CLIENT_SUPPORTED; i++)
    {
        monitored_fd_set[i] = -1;
    }
}
void ReinitFDSet(fd_set *read_fds)
{
    FD_ZERO(read_fds);
    for (int i = 0; i < MAX_CLIENT_SUPPORTED; i++)
    {
        if (monitored_fd_set[i] != -1)
        {
            FD_SET(monitored_fd_set[i], read_fds);
        }
    }
}
int GetMaxFD()
{

    int i = 0;
    int max = -1;

    for (; i < MAX_CLIENT_SUPPORTED; i++)
    {
        if (monitored_fd_set[i] > max)
            max = monitored_fd_set[i];
    }

    return max;
}
void RemoveFromFDSet(int fd)
{

    for (int i = 0; i < MAX_CLIENT_SUPPORTED; i++)
    {

        if (monitored_fd_set[i] != fd)
            continue;
        monitored_fd_set[i] = -1;
        break;
    }
}
void SetupTCPServer()
{

    // Step 1. Init
    int master_sock_fd = 0, sent_recv_bytes = 0, addr_len = 0, opt = 1;
    int comm_sock_fd = 0;
    fd_set read_fds;
    struct sockaddr_in server_addr, client_addr;

    InitMonitorFDSet();

    // Step 2. Master Socket Creation
    // AF_INET for IPv4, AF_INET6 = IPv6
    // SOCK_STREAM for TCP, SOCK_DGRAM for UDP
    // IPPROTO_TCP for TCP, IPPROTO_UDP for UDP
    master_sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (master_sock_fd == -1)
    {
        std::cout << "Socket creation failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Step 3. Specify Server Information
    // Socket will process only IPv4 packets arriving on the predefined port
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = SERVER_PORT;
    // Server's IP address INADDR_ANY means any IP address destined to any local interface of the server
    server_addr.sin_addr.s_addr = INADDR_ANY;
    addr_len = sizeof(struct sockaddr);

    // server tells OS which kind of data it is interested in with its information
    if (bind(master_sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        std::cout << "Socket bind failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Step 4. Max length of queue for incoming connections
    if (listen(master_sock_fd, 5) < 0)
    {
        std::cout << "listen failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    AddToFDSet(master_sock_fd);

    // Inifinite loop for handling requests
    while (true)
    {
        // Step 5. Reinit FDs
        ReinitFDSet(&read_fds);

        std::cout << "Blocked on select" << std::endl;

        // Step 6. Wait for client connection
        // 1 + Max value in read_fds
        // blocked until new data presents in read_fds
        select(GetMaxFD() + 1, &read_fds, NULL, NULL, NULL);

        if (FD_ISSET(master_sock_fd, &read_fds))
        {
            std::cout << "New connection is received and accepted, TCP 3 way handshake is completed at this point" << std::endl;

            // Step 7. Accpet an incoming conn request
            sockaddr_in client_addr;
            comm_sock_fd = accept(master_sock_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);
            if (comm_sock_fd < 0)
            {
                std::cout << "accept error: " << errno << std::endl;
                exit(EXIT_FAILURE);
            }
            AddToFDSet(comm_sock_fd);

            std::cout << "Connection accepted from client " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;
        }
        else
        {
            // Step 8. Server recieving the data from client. Client IP and PORT no will be stored in client_addr
            int comm_socket_fd = -1;
            for (int i = 0; i < MAX_CLIENT_SUPPORTED; i++)
            {
                if (FD_ISSET(monitored_fd_set[i], &read_fds))
                {
                    sockaddr_in client_addr;
                    std::cout << "Server ready to serve client msgs" << std::endl;
                    comm_socket_fd = monitored_fd_set[i];
                    test_struct_t client_data;
                    sent_recv_bytes = recvfrom(comm_socket_fd, &client_data, sizeof(test_struct_t), 0, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);
                    std::cout << "Server received " << sent_recv_bytes << " bytes from client " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;

                    if (sent_recv_bytes == 0)
                    {
                        close(comm_sock_fd);
                        RemoveFromFDSet(comm_sock_fd);
                        std::cout << "sent_recv_bytes == 0, close connection with client " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;
                        break;
                    }
                    std::cout << "Received data: a = " << client_data.a << " b = " << client_data.b << std::endl;

                    if (client_data.a == 0 && client_data.b == 0)
                    {
                        close(comm_sock_fd);
                        RemoveFromFDSet(comm_sock_fd);
                        std::cout << "a == 0 && b == 0, close connection with client " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;
                        break;
                    }

                    result_struct_t result;
                    result.c = client_data.a + client_data.b;
                    sent_recv_bytes = sendto(comm_sock_fd, &result, sizeof(result_struct_t), 0, (struct sockaddr *)&client_addr, sizeof(struct sockaddr));
                    std::cout << "Server sent " << sent_recv_bytes << " bytes to client" << std::endl;
                }
            }
        }
    }
}
int main()
{
    SetupTCPServer();
    return EXIT_SUCCESS;
}