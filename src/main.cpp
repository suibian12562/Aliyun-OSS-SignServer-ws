#include "main.h"

int main(int argc, char* argv[]){
    config = readConfigFromFile("config.json");
    bucket_permissions = readBucketInfoFromFile("bucket.json");

    for (const auto &bucket : bucket_permissions)
    {
            std::cout << "Bucket: " << bucket._Bucket << ", Allowed: " << bucket.allowed << ", Password: " << bucket.password << std::endl;
    }
    std::cout << "Specified port: " << config.port << std::endl;
    std::cout << "AccessKeyId: " << config.AccessKeyId << std::endl;
    std::cout << "AccessKeySecret: " << config.AccessKeySecret << std::endl;
    std::cout << "sign_time: " << config.sign_time << std::endl;
    cout << "\n\n\n" <<endl;///log

    // Create a server endpoint
    server echo_server;

    try {
        // Set logging settings
        echo_server.set_access_channels(websocketpp::log::alevel::all);
        echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize Asio
        echo_server.init_asio();

        // Register our message handler
        echo_server.set_message_handler(bind(&on_message, &echo_server, ::_1, ::_2));
        echo_server.set_close_handler(&on_close);

        //bind()
        echo_server.listen(config.port);

        // Start the server accept loop
        echo_server.start_accept();

        // Start the ASIO io_service run loop
        echo_server.run();
    }
    catch (websocketpp::exception const& e) {
        std::cerr << e.what() << std::endl;
        ShutdownSdk();
    }
    catch (json::exception const& a) {
        std::cerr << a.what() << std::endl;
        ShutdownSdk();
    }
    catch (...) {
        std::cerr << "other exception" << std::endl;
        ShutdownSdk();
    }
}
