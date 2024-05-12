cd ../../build/source/computer_networks_project

./host 1 3 59 0 0 1 1 1 2 "Hi to (1,2)" &
./bridge 1 1 2 3 &
./bridge 2 1 1 4 5 &
./bridge 3 1 1 6 7 &
./bridge 4 1 2 &
./bridge 5 1 2 &
./bridge 6 1 3 &
./bridge 7 1 3 &
./host 1 2 27 0 0 7 1 &
