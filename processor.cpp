#include <cmath>

#include "mqtt/async_client.h"
#include "tools.hpp"

const std::string SERVER_ADDRESS { "tcp://localhost:1883" };
const std::string CLIENT_ID		 { "processor" };
const std::string TOPIC 		 { "test_topic" };

struct ForkliftData {
    int x{0}, y{0}, z{0}, color{31}, distance{0};
};

int main(int argc, char* argv[])
{
    mqtt::async_client cli(SERVER_ADDRESS, CLIENT_ID);

    auto connOpts = mqtt::connect_options_builder()
        .clean_session(false)
        .finalize();

    std::map<std::string, ForkliftData> forklifts_info;

    try {
        cli.start_consuming();

        std::cout << "Connecting to the MQTT server..." << std::endl;
        auto tok = cli.connect(connOpts);

        auto rsp = tok->get_connect_response();

        cli.subscribe(TOPIC, 1)->wait();

        std::cout << "Waiting for messages on topic: '" << TOPIC << "'" << std::endl;

        while (true) {
            auto msg = cli.consume_message();
            if (!msg) break;
            auto data = Split(msg->to_string(), ';');
            time_t timestamp = std::stoll(data[0]);

            auto tag_id = data[1];
            int x = std::stoi(data[2]);
            int y = std::stoi(data[3]);
            int z = std::stoi(data[4]);

            if (forklifts_info.find(tag_id) == forklifts_info.end()) {
                forklifts_info[tag_id].color += forklifts_info.size();
            }

            auto diff = std::sqrt(pow(x - forklifts_info[tag_id].x, 2) +
                                  pow(y - forklifts_info[tag_id].y, 2) +
                                  pow(z - forklifts_info[tag_id].z, 2));
            forklifts_info[tag_id].x = x;
            forklifts_info[tag_id].y = y;
            forklifts_info[tag_id].z = z;
            forklifts_info[tag_id].distance += diff;

            std::cout << "\033[1;" << forklifts_info[tag_id].color << "m"
                      << GetDataFromTimestamp(timestamp)
                      << "\tForklift: " << tag_id << "\tDistance(meters): "
                      << forklifts_info[tag_id].distance
                      << ", Speed(kph): " << (diff * 18 / 5) << std::endl;
        }

        if (cli.is_connected()) {
            std::cout << "\nShutting down and disconnecting from the MQTT server..." << std::flush;
            cli.unsubscribe(TOPIC)->wait();
            cli.stop_consuming();
            cli.disconnect()->wait();
            std::cout << "OK" << std::endl;
        }
        else {
            std::cout << "\nClient was disconnected" << std::endl;
        }
    }
    catch (const mqtt::exception& exc) {
        std::cerr << "\n  " << exc << std::endl;
        return 1;
    }

    return 0;
}
