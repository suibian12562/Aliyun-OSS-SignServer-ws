#include "main.h"

using namespace std;
using namespace nlohmann;
using namespace AlibabaCloud::OSS;

vector<message_info> cache;
vector<bucket_info> bucket_permissions;
Config config;


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

void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
    try
    {
        std::cout << "on_message called with hdl: " << hdl.lock().get()
            << " and message: " << msg->get_payload()
            << std::endl;
        message_info temp;
        message_info push_temp;
        int i = 0;
        int if_cache = 0;
        time_t now;
        json data = json::parse(msg->get_payload());//get data from client
        //cout << data << endl;
       temp = json::parse(msg->get_payload()).get<message_info>();//json to message_info(temp)
       auto it = begin(cache);
       while (it != end(cache))
       {
           if ((time(&now) - it->_request_time) < config.sign_time)
           {
               cout << "**Hit cache**\a" << endl;
               if_cache = 1;
               break;
           }
           else
           {
               cout << "*********************delete vector*************************\a" << i << endl;
               it = cache.erase(it);
           }
       }

       if (if_cache != 1)
       {
           ++i;
       }

        string buf;
        if (if_cache != 1)
        {
            InitializeSdk();
            ClientConfiguration conf;
            OssClient client(temp._Endpoint, config.AccessKeyId, config.AccessKeySecret, conf);
            std::time_t t = std::time(nullptr) + config.sign_time;

            auto genOutcome = client.GeneratePresignedUrl(temp._Bucket, temp._GetobjectUrlName, t, Http::Get);
            if (genOutcome.isSuccess()) {
                std::cout << "GeneratePresignedUrl for " << hdl.lock().get() << " success, Gen url:" << genOutcome.result().c_str() << std::endl;
                push_temp._GenedUrl = genOutcome.result().c_str();
                push_temp._Bucket = temp._Bucket;
                push_temp._GetobjectUrlName = temp._GetobjectUrlName;
                push_temp._Endpoint = temp._Endpoint;
                push_temp._request_time = (time(&now));
                cache.push_back(push_temp);
                //cache[i]._GenedUrl = genOutcome.result().c_str();//save gened Url  something wrong
                buf = genOutcome.result().c_str();//pass data

            }
            else {
                cout << "GeneratePresignedUrl fail" <<
                    ",code:" << genOutcome.error().Code() <<
                    ",message:" << genOutcome.error().Message() <<
                    ",requestId:" << genOutcome.error().RequestId() << std::endl;
            }
        }
        else
        {
            buf = cache[i]._GenedUrl;
        }
        string a = "+";
        string b = "%20";
        subreplace(buf, a, b);
        //s->send(hdl, msg->get_payload(), msg->get_opcode());//复读机
        s->send(hdl, buf, msg->get_opcode());
        // s->send(hdl, cache[i]._GenedUrl, msg->get_opcode());
    }
    catch (websocketpp::exception const& e) {
        std::cerr << "Echo failed because: "
            << "(" << e.what() << ")" << std::endl;
    }
    catch (json::exception const& e) {
        std::cerr << "Echo failed because: "
            << "(" << e.what() << ")" << std::endl;
        s->send(hdl, e.what(), msg->get_opcode());
    }
}//websocket server

void on_close(websocketpp::connection_hdl) {
    std::cout << "Close handler" << std::endl;
}


