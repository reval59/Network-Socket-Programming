#include <iostream>
#include <sys/select.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "CommData.h"
#include <string>
class Client
{
public:
    Client() : sent_bytes_(0), recv_bytes_(0)
    {

    }
    bool Connect(std::string ipaddr, int port){
        server_addr_.sin_family = AF_INET;
        server_addr_.sin_port = port;
        hostent *server = (hostent *) gethostbyname(ipaddr.c_str());
        server_addr_.sin_addr = *((in_addr *) server->h_addr_list[0]);
        sock_fd_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        while (true) {
            Request req;
            std::cout << "Enter a: ";
            std::cin >> req.a;
            std::cout << "Enter b: ";
            std::cin >> req.b;
            sent_bytes_ = sendto(sock_fd_, reinterpret_cast<const char*>(&req), sizeof(req), 0, (sockaddr *) &server_addr_, sizeof(sockaddr));
            std::cout << "No. of bytes sent: " << sent_bytes_ << std::endl;
            Response res;
            recv_bytes_ = recvfrom(sock_fd_, reinterpret_cast<char *>(&res), sizeof(res), 0, (sockaddr *) &server_addr_, (socklen_t *) &server_addr_len_);
            std::cout << "No. of bytes recvd: " << recv_bytes_ << " c: " <<  res.c <<std::endl;
        }
        return true;
    }

private:
    sockaddr_in server_addr_;
    socklen_t server_addr_len_;
    int sock_fd_;
    int sent_bytes_;
    int recv_bytes_;
};

int main() {
    Client client;
    client.Connect("127.0.0.1", 2000);
}