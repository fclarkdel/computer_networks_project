cd ../../build/source/computer_networks_project

./host 1 5 25 0 0 1 1 1 6 "(1,5) talks to (1,6)" &
./host 1 6 75 0 0 1 3 1 5 "This is (1,6)" &
./host 1 8 18 0 0 1 2 1 6 "(1,8) chats with (1,6)" &
./bridge 1 3 &
