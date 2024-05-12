cd ../../build/source/computer_networks_project

./host 1 2 25 1 1 1 1 2 2 "Message from (1,2) to (2,2)" &
./bridge 1 2 &
./router 1 1 1 2 19 2 5 2 2 19 &
./bridge 2 2 &
./host 2 2 87 2 5 2 1 &
