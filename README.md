# computer_networks_project

## overview
This project was made for my computer networks course. It simulates several layers of a computer network such as Ethernet, ARP, broadcast routing, and a transport layer protocol that uses concurrent logical channels to ensure data transmission between hosts.

## usage
To get the source code run:

```git clone https://github.com/fclarkdel/computer_networks_project```

Use a C++ compiler that supports C++ 20.

Make sure to have at least CMake 3.26.

Make sure you are in the computer_networks_project directory, then run the following commands.

```cmake -DCMAKE_BUILD_TYPE=Release -S ./ -B ./build```

```cmake --build ./build```

```cd build/source/computer_networks_project```

Example scenarios are in scripts/scenarios.

After running a scenario, make sure to run ```kill_and_clean.sh``` to kill the processes that were started and to clean up the files that were used as links.

If you do not run ```kill_and_clean.sh``` after running a scenario, many programs will be left running by the scenarios, and subsequent running of scenarios will have weird behavior because of the files left behind.
