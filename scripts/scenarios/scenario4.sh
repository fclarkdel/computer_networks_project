cd ../../build/source/computer_networks_project

./host 1 1 56 1 2 1 1 4 1 "Howdy Partner!" &
./bridge 1 3 &
./router 1 2 1 3 59 2 1 2 1 59 &
./router 1 3 1 2 64 3 1 3 1 64 &
./bridge 2 2 &
./bridge 3 3 &
./router 2 2 2 2 92 3 2 3 2 92 &
./router 3 3 3 3 19 4 2 4 1 19 &
./bridge 4 2 &
./host 4 1 75 4 2 4 2 &
