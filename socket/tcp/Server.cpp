#include <iostream>
#include <sys/select.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "CommData.h"
class Server
{
public:
    Server(int port) : sent_bytes_(0), recv_bytes_(0)
    {
        master_sock_fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        std::cout << "socket created" << std::endl;
        assert(master_sock_fd_ != -1);
        addr_.sin_family = AF_INET;
        addr_.sin_port = port;
        addr_.sin_addr.s_addr = INADDR_ANY;
        assert(bind(master_sock_fd_, (sockaddr *)&addr_, sizeof(sockaddr)) != -1);
        std::cout << "bind sucessful" << std::endl;
        int queue_sz = 5;
        assert(listen(master_sock_fd_, queue_sz) >= 0);
        std::cout << "listen sucessful" << std::endl;
    }
    void Start()
    {
        while (true)
        {
            FD_ZERO(&read_fdset_);
            FD_SET(master_sock_fd_, &read_fdset_);
            std::cout << "blocked on select system call" << std::endl;
            select(master_sock_fd_ + 1, &read_fdset_, nullptr, nullptr, nullptr);
            if (FD_ISSET(master_sock_fd_, &read_fdset_))
            {
                std::cout << "new connection request received" << std::endl;
                sockaddr_in client_addr;
                socklen_t client_addr_len;
                int comm_sock_fd = accept(master_sock_fd_, (sockaddr *)&client_addr, &client_addr_len);
                std::cout << "connection request accepted from client " << inet_ntoa(client_addr.sin_addr) << ':' << ntohs(client_addr.sin_port) << std::endl;
                while (true)
                {
                    std::cout << "ready to serve the connected client" << std::endl;
                    // serialization lib should be used in practice
                    memset(recv_buffer_, 0, sizeof(recv_buffer_));
                    recv_bytes_ = recvfrom(comm_sock_fd, reinterpret_cast<char *>(recv_buffer_), sizeof(recv_buffer_), 0, (sockaddr *)&client_addr, &client_addr_len);
                    Request *req = reinterpret_cast<Request *>(recv_buffer_);
                    std::cout << "received data from the client:"
                              << " a: " << req->a << " b: " << req->b << std::endl;
                    if (req->a == 0 && req->b == 0)
                    {
                        std::cout << "connection closed with client " << inet_ntoa(client_addr.sin_addr) << ':' << ntohs(client_addr.sin_port) << std::endl;
                        close(comm_sock_fd);
                        break;
                    }
                    Response res;
                    res.c = req->a + req->b;
                    sent_bytes_ = sendto(comm_sock_fd, reinterpret_cast<const char *>(&res), sizeof(res), 0, (sockaddr *)&client_addr, sizeof(sockaddr));
                    std::cout << "sent response back to the client " << inet_ntoa(client_addr.sin_addr) << ':' << ntohs(client_addr.sin_port) << std::endl;
                }
            }
        }
    }

private:
    sockaddr_in addr_;
    fd_set read_fdset_;
    int master_sock_fd_;
    int sent_bytes_;
    int recv_bytes_;
    char recv_buffer_[1024];
};

int main()
{
    Server server(2000);
    server.Start();
}
