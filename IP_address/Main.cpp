#include <string>
#include "IPAddress.h"

using namespace std;
int main()
{
    while (true)
    {
        string ipaddr;
        int mask;
        cin >> ipaddr;
        cin >> mask;
        IPAddress ip(ipaddr, mask);
        cout << ip.GetBroadcastIP() << endl;
        cout << ip.GetNetworkID() << endl;
        cout << ip.GetInt() << endl;
        cout << IPAddress::GetABCDFormat(ip.GetInt()) << endl;        
    }
    return 0;
}