#include <iostream>
#include <sys/select.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <vector>
#include "CommData.h"
class Server
{
public:
    Server(int port, int max_client_no) : sent_bytes_(0), recv_bytes_(0), max_client_no_(max_client_no), monitored_fdset_(max_client_no, -1), cur_fd_no(0)
    {
        assert(max_client_no > 0 && port > 0);
        master_sock_fd_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        std::cout << "socket created" << std::endl;
        assert(master_sock_fd_ != -1);
        addr_.sin_family = AF_INET;
        addr_.sin_port = port;
        addr_.sin_addr.s_addr = INADDR_ANY;
        assert(bind(master_sock_fd_, (sockaddr *)&addr_, sizeof(sockaddr)) != -1);
        std::cout << "bind sucessful" << std::endl;
    }
    void Start()
    {
        while (true)
        {
            HandleRequest(master_sock_fd_);
        }
    }

private:
    sockaddr_in addr_;
    fd_set read_fdset_;
    int max_client_no_;
    int cur_fd_no;
    std::vector<int> monitored_fdset_;
    int master_sock_fd_;
    int sent_bytes_;
    int recv_bytes_;
    char recv_buffer_[1024];
    void HandleRequest(int fd)
    {
        std::cout << "ready to serve the connected client" << std::endl;
        sockaddr_in client_addr;
        socklen_t client_addr_len;
        // serialization lib should be used in practice
        memset(recv_buffer_, 0, sizeof(recv_buffer_));
        recv_bytes_ = recvfrom(fd, reinterpret_cast<char *>(recv_buffer_), sizeof(recv_buffer_), 0, (sockaddr *)&client_addr, &client_addr_len);
        Request *req = reinterpret_cast<Request *>(recv_buffer_);
        std::cout << "received data from the client:"
                  << " a: " << req->a << " b: " << req->b << std::endl;
        Response res;
        res.c = req->a + req->b;
        sent_bytes_ = sendto(fd, reinterpret_cast<const char*>(&res), sizeof(res), 0, (sockaddr *) &client_addr, sizeof(sockaddr));
        std::cout << "sent response back to the client " << inet_ntoa(client_addr.sin_addr) << ':' << ntohs(client_addr.sin_port) << std::endl;
    }
};

int main()
{
    Server server(2000, 32);
    server.Start();
}
