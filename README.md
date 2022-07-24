# mqtt_test

**To install additional libraries on debian based systems(tested on Ubuntu 22.04):**

sudo apt install libpaho-mqtt*-dev

**To build applications:**

cmake -B build -S .

cmake --build build

**Example of usage**

./build/processor

./build/data_generator client_1 forklift_1
