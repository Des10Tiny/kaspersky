#include "client.hpp"
#include <iostream>
#include <string>

static std::string ip_for_client = "127.0.0.1";

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Use: " << argv[0] << " <path_to_file> <port>\n";
        return EXIT_FAILURE;
    }

    std::string file_path = argv[1];
    int port;

    try {
        port = std::stoi(argv[2]);

    } catch (const std::exception&) {
        std::cerr << "Client Error: port is invalid \n";

        return EXIT_FAILURE;
    }

    try {
        Client client(ip_for_client, port);
        std::cout << "Connection established.\n Sending the file for verification...\n";
        client.send_file(file_path);
        std::cout << "The file has been sent.\n Waiting for scan results...\n";
        std::string result = client.receive_result();
        std::cout << "\n ------------- Report from Server -------------\n" << result << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Critical failure: \n" << e.what() << "\n";

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}