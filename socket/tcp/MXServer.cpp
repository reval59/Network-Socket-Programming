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
        AddToMonitoredFDSet(master_sock_fd_);
        std::cout << "listen sucessful" << std::endl;
    }
    void Start()
    {
        while (true)
        {
            ReinitMonitoredFDSet();
            std::cout << "blocked on select system call" << std::endl;
            select(GetMaxFD() + 1, &read_fdset_, nullptr, nullptr, nullptr);
            const int activated_fd = GetActivatedFD();
            if (activated_fd == master_sock_fd_)
            {
                AcceptConn();
            }
            else
            {
                HandleRequest(activated_fd);
            }
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

    bool AddToMonitoredFDSet(int fd)
    {
        assert(monitored_fdset_.size() == max_client_no_);
        for (int i = 0; i < monitored_fdset_.size(); i++)
        {
            if (monitored_fdset_[i] != -1)
                continue;
            monitored_fdset_[i] = fd;
            cur_fd_no++;
            return true;
        }
        return false;
    }

    bool RemoveFromMonitoredFDSet(int fd)
    {
        assert(monitored_fdset_.size() == max_client_no_);
        for (int i = 0; i < monitored_fdset_.size(); i++)
        {
            if (monitored_fdset_[i] != fd)
                continue;
            monitored_fdset_[i] = -1;
            cur_fd_no--;
            return true;
        }
        return false;
    }
    void ReinitMonitoredFDSet()
    {
        assert(monitored_fdset_.size() == max_client_no_);
        FD_ZERO(&read_fdset_);
        for (int i = 0; i < monitored_fdset_.size(); i++)
        {
            if (monitored_fdset_[i] == -1)
                continue;
            FD_SET(monitored_fdset_[i], &read_fdset_);
        }
    }
    int GetMaxFD()
    {
        assert(monitored_fdset_.size() == max_client_no_);
        int maxfd = monitored_fdset_[0];
        for (int i = 1; i < monitored_fdset_.size(); i++)
        {
            if (monitored_fdset_[i] > maxfd)
                maxfd = monitored_fdset_[i];
        }
        return maxfd;
    }
    int GetActivatedFD()
    {
        assert(monitored_fdset_.size() == max_client_no_);
        for (int i = 0; i < monitored_fdset_.size(); i++)
        {
            if (FD_ISSET(monitored_fdset_[i], &read_fdset_))
                return monitored_fdset_[i];
        }
        return -1;
    }
    void AcceptConn()
    {
        std::cout << "new connection request received" << std::endl;
        sockaddr_in client_addr;
        socklen_t client_addr_len;
        int comm_sock_fd = accept(master_sock_fd_, (sockaddr *)&client_addr, &client_addr_len);
        assert(comm_sock_fd >= 0);
        AddToMonitoredFDSet(comm_sock_fd);
        std::cout << "connection request accepted from client " << inet_ntoa(client_addr.sin_addr) << ':' << ntohs(client_addr.sin_port) << std::endl;
    }
    void HandleRequest(int fd)
    {
        std::cout << "ready to serve the connected client" << std::endl;
        sockaddr_in client_addr;
        socklen_t client_addr_len;
        Request req;
        // serialization lib should be used in practice
        recv_bytes_ = recvfrom(fd, &req, sizeof(req), 0, (sockaddr *)&client_addr, &client_addr_len);
        std::cout << "received data from the client:"
                  << " a: " << req.a << " b: " << req.b << std::endl;
        if (req.a == 0 && req.b == 0)
        {
            std::cout << "connection closed with client " << inet_ntoa(client_addr.sin_addr) << ':' << ntohs(client_addr.sin_port) << std::endl;
            close(fd);
            RemoveFromMonitoredFDSet(fd);
        }
        Response res;
        res.c = req.a + req.b;
        sent_bytes_ = sendto(fd, &res, sizeof(res), 0, (sockaddr *)&client_addr, sizeof(sockaddr));
        std::cout << "sent response back to the client " << inet_ntoa(client_addr.sin_addr) << ':' << ntohs(client_addr.sin_port) << std::endl;
    }
};

int main()
{
    Server server(2000, 32);
    server.Start();
}
