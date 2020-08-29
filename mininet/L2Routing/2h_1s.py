from mininet.net import Mininet
from mininet.cli import CLI
from mininet.link import Link

if '__main__' == __name__:
    net = Mininet()

    # Add Host
    h1 = net.addHost('h1')
    h2 = net.addHost('h2')

    # Add L2 switch
    s5 = net.addHost('s5')

    # Link names are generated automatically
    Link(h1, s5) # h1, eth0 -- s5, eth0
    # Link(h1, s5, intfName1 = 'any name', intfName2 = 'any name2')
    Link(h2, s5)

    net.build()

    # Remove deafult IP addr, 0 indicates NULL
    h1.cmd("ifconfig h1-eth0 0") 
    h2.cmd("ifconfig h2-eth0 0")

    s5.cmd("ifconfig s5-eth0 0")
    s5.cmd("ifconfig s5-eth1 0")

    # Create a vlan 10 on Switch s5
    s5.cmd("brctl addbr vlan10")

    # Bring vlan10 interfaces
    s5.cmd("ifconfig vlan10 up")

    # Add s5-th0 to vlan
    s5.cmd("brctl addif vlan10 s5-eth0")
    s5.cmd("brctl addif vlan10 s5-eth1")

    # Assign custom IP addr to Hosts
    h1.cmd("ifconfig h1-eth0 10.0.10.1 netmask 255.255.255.0")
    h2.cmd("ifconfig h2-eth0 10.0.10.2 netmask 255.255.255.0")

    # Set Default Routing
    h1.cmd("ip route add default via 10.0.10.254 dev h1-eth0")
    h2.cmd("ip route add default via 10.0.10.254 dev h2-eth0")

    # Start Mininet Cli Prompt
    CLI(net)
    net.stop()

    # Routing info h1 route -n
    # Arp info h1 arp -n