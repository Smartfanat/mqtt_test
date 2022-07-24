cmake_minimum_required(VERSION 3.15)

SET(CMAKE_CXX_STANDARD 14)

find_package(PahoMqttCpp REQUIRED)

add_executable(data_generator data_generator.cpp)
target_link_libraries(data_generator PahoMqttCpp::paho-mqttpp3)

add_executable(processor tools.hpp processor.cpp)
target_link_libraries(processor PahoMqttCpp::paho-mqttpp3)
