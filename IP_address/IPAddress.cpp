#include "IPAddress.h"

IPAddress::IPAddress(const string ipaddr, int mask) : ipaddr_arr{0, 0, 0, 0}, mask_arr{255, 255, 255, 255}
{
    InitIPAddress(ipaddr);
    InitMask(mask);
}

string IPAddress::GetABCDFormat(unsigned int intEq)
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

string IPAddress::GetBroadcastIP()
{
    stringstream ss;
    for (int i = 0; i < 4; i++)
    {
        uint8_t oct = (ipaddr_arr[i] & mask_arr[i]) | (~mask_arr[i]);
        if (i > 0)
            ss << '.';
        ss << (unsigned int)oct;
    }
    return ss.str();
}

string IPAddress::GetNetworkID()
{
    stringstream ss;
    for (int i = 0; i < 4; i++)
    {
        uint8_t oct = (ipaddr_arr[i] & mask_arr[i]);
        if (i > 0)
            ss << '.';
        ss << (unsigned int)oct;
    }
    return ss.str();
}

unsigned int IPAddress::GetInt()
{
    uint32_t intEq = 0;
    for (int i = 0; i < 4; i++)
    {
        intEq = intEq ^ ipaddr_arr[i];
        if (i < 3)
            intEq = intEq << 8;
    }
    return (unsigned int)intEq;
}

void IPAddress::InitMask(int mask)
{
    for (auto &n : mask_arr)
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

void IPAddress::InitIPAddress(const string ipaddr)
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
            ipaddr_arr[j++] = interim;
            interim = 0;
        }
    }
    ipaddr_arr[3] = interim;
}
