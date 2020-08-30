#ifndef __IPADDRESS_H__
#define __IPADDRESS_H__

#include <iostream>
#include <string>
#include <sstream>
using namespace std;

class IPAddress
{

public:
    IPAddress(const string ipaddr, int mask);
    static string GetABCDFormat(unsigned int intEq);
    string GetBroadcastIP();
    string GetNetworkID();
    unsigned int GetInt();

private:
    void InitMask(int mask);
    void InitIPAddress(const string ipaddr);

    uint8_t ipaddr_arr[4];
    uint8_t mask_arr[4];
};
#endif // __IPADDRESS_H__