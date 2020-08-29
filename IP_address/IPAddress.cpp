#include "IPAddress.h"

IPAddress::IPAddress(const string ipaddr, int mask) : ipaddrArr{0, 0, 0, 0}, maskArr{255, 255, 255, 255}
{
    initIPAddress(ipaddr);
    initMask(mask);
}

string IPAddress::getABCDFormat(unsigned int intEq)
{
    stringstream ss;
    unsigned int abcd[4];
    for (int i = 0; i < 4; i++)
    {
        abcd[3 - i] = intEq & 0xFF;
        intEq = intEq >> 8;
    }
    ss << abcd[0];
    for (int i = 1; i < 4; i++)
    {
        ss << '.' << abcd[i];
    }
    return ss.str();
}

string IPAddress::getBroadcastIP()
{
    stringstream ss;
    for (int i = 0; i < 4; i++)
    {
        uint8_t oct = (ipaddrArr[i] & maskArr[i]) | (~maskArr[i]);
        if (i > 0)
            ss << '.';
        ss << (unsigned int)oct;
    }
    return ss.str();
}

string IPAddress::getNetworkID()
{
    stringstream ss;
    for (int i = 0; i < 4; i++)
    {
        uint8_t oct = (ipaddrArr[i] & maskArr[i]);
        if (i > 0)
            ss << '.';
        ss << (unsigned int)oct;
    }
    return ss.str();
}

unsigned int IPAddress::getInt()
{
    uint32_t intEq = 0;
    for (int i = 0; i < 4; i++)
    {
        intEq = intEq ^ ipaddrArr[i];
        if (i < 3)
            intEq = intEq << 8;
    }
    return (unsigned int)intEq;
}

void IPAddress::initMask(int mask)
{
    for (auto &n : maskArr)
    {
        if (mask >= 8)
        {
            mask -= 8;
            continue;
        }
        else
        {
            n = n << (8 - mask);
            mask = 0;
        }
    }
}

void IPAddress::initIPAddress(const string ipaddr)
{
    uint8_t interim = 0;
    for (int i = 0, j = 0; i < ipaddr.size(); i++)
    {
        if (isdigit(ipaddr[i]))
        {
            interim = 10 * interim + (ipaddr[i] - '0');
        }
        else
        {
            ipaddrArr[j++] = interim;
            interim = 0;
        }
    }
    ipaddrArr[3] = interim;
}
