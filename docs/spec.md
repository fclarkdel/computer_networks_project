# Specification

# Bridge
## Arguments

Bridge ID

Number of ports

List of neighboring bridges

E.g. ```bridge 1 6 2``` denotes a bridge with ID 1, 6 ports, and a bridge with ID 2 as its neighbor

## Functionality

Forward ethernet messages to appropriate locations.

Ethernet messages with broadcast address will be forwarded over all ports, except for where it came from.

Messages with a specific address will be forwarded to the appropriate port.

Bridges learn about the location of specific ethernet addresses by observing the source address of each message.

They will keep a cache of addresses they have learned - which do not expire.

If the destination address is not in the cache, it is forwarded over all ports.

Ethernet message structure:

ethernet-destination-address ethernet-source-address packet-type encapsulated-packet

E.g. ```28 19 IP IP-packet\n```

Packets end with a newline character.

# Routers
## Arguments
## Functionality