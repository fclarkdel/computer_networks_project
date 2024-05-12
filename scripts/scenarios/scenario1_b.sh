cd ../../build/source/computer_networks_project

# The description of this test scenario is wrong.
# Since there is no other host, this host will never get an ARP REP,
# and will thus never try to send data, because it would not even know
# where to send it.

./host 1 1 23 1 9 1 1 1 3 "Hi, how are you?" &
./bridge 1 2 &
