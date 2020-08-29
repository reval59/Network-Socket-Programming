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
    static string getABCDFormat(unsigned int intEq);
    string getBroadcastIP();
    string getNetworkID();
    unsigned int getInt();

private:
    void initMask(int mask);
    void initIPAddress(const string ipaddr);

    uint8_t ipaddrArr[4];
    uint8_t maskArr[4];
};
#endif // __IPADDRESS_H__