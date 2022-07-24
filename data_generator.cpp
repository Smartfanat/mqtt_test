#include <iostream>
#include <memory>
#include <thread>

#include "mqtt/client.h"

const std::string SERVER_ADDRESS{"tcp://localhost:1883"};
const std::string TOPIC          { "test_topic" };

class Generator
{
public:
    void connect(const std::string& address, const std::string& client_id) {
        client_ = std::make_unique<mqtt::client>(address, client_id);

        mqtt::connect_options connOpts;
        connOpts.set_keep_alive_interval(20);
        connOpts.set_clean_session(true);
        std::cout << "Connecting" << std::endl;
        client_->connect(connOpts);
        std::cout << "Connected..." << std::endl;
    }
    void disconnect()
    {
        std::cout << "Disconnecting" << std::endl;
        client_->disconnect();
        std::cout << "Disconnected..." << std::endl;
    }
    void publishMessage(const std::string& topic, const std::string& payload) {
        if (!client_) {
            std::cerr << "No client" << std::endl;
            return;
        }

        auto pubmsg = mqtt::make_message(topic, payload);
        pubmsg->set_qos(1);
        client_->publish(pubmsg);
    }

private:
    std::unique_ptr<mqtt::client> client_;
};

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cout << "Usage: data_generator <client_id> <forklift>"
                  << std::endl;
        return 1;
    }

    std::string client_id = argv[1];
    std::string forklift = argv[2];

    std::cout << "Setting up: client_id - " << client_id << ", forklift - "
              << forklift << std::endl;

    std::cout << "Initialzing MQTT data generator..." << std::endl;

    Generator generator;
    generator.connect(SERVER_ADDRESS, client_id);

    std::cout << "Press 's' to stop sending data" << std::endl;

    char c;
    std::thread t([&c, &generator, &forklift]() -> void {
        int x{0}, y{0}, z{0};

        while (true) {
            try {
                auto payload =
                    std::to_string(
                        std::chrono::duration_cast<std::chrono::seconds>(
                            std::chrono::system_clock::now().time_since_epoch())
                            .count()) +
                    ";" + forklift + ";" + std::to_string(x) + ";" +
                    std::to_string(y) + ";" + std::to_string(z);
                x += 3;
                y += 3;

                generator.publishMessage(TOPIC, payload);
            } catch (const mqtt::exception& exc) {
                std::cerr << exc.what() << std::endl;
                return;
            }

            if (c == 's') {
                break;
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        generator.disconnect();
    });

    while (c != 's') {
        std::cin >> c;
        std::cout << "Press 's' to stop sending data: pressed '" << c << "'"
                  << std::endl;
    }
    t.join();

    std::cout << "\Exiting" << std::endl;
    return 0;
}
