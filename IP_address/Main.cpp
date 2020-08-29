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
        cout << ip.getBroadcastIP() << endl;
        cout << ip.getNetworkID() << endl;
        cout << ip.getInt() << endl;
        // unsigned int ipeq;
        // cin >> ipeq;
        // cout << IPAddress::getABCDFormat(ipeq) << endl;        
    }
    return 0;
}