#include <iostream>
#include<string>
#include<vector>
#include<ctime>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <alibabacloud/oss/OssClient.h>
#include <nlohmann/json.hpp>


using namespace std;
using namespace nlohmann;
using namespace AlibabaCloud::OSS;

const std::string AccessKeyId = "xxxxx";
const std::string AccessKeySecret = "xxxxx";
const long time_useable = 40;//签名时间 second
//int runtime = 0;
struct message_info
{
    std::string _Endpoint;
    std::string _Bucket;
    std::string _GetobjectUrlName;
    std::string _GenedUrl;
    long _request_time;
    long _time_useable= time_useable;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(message_info, _Endpoint,_Bucket,_GetobjectUrlName)
};
vector<message_info> cache;



typedef websocketpp::server<websocketpp::config::asio> server;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;
// Define a callback to handle incoming messages

string subreplace(string resource_str, string sub_str, string new_str)
{
    string::size_type pos = 0;
    while ((pos = resource_str.find(sub_str)) != string::npos)   //替换所有指定子串
    {
        resource_str.replace(pos, sub_str.length(), new_str);
    }
    return resource_str;
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
    for (auto it = begin(cache); it != end(cache); ++it, ++i)
        {
            if (temp._GetobjectUrlName == cache[i]._GetobjectUrlName && (time(&now) - cache[i]._request_time) < time_useable)
            {
                cout << "**Hit cache**\a" << endl;
                if_cache = 1;
                break;
            }
            else{}
            if ((time(&now) - cache[i]._request_time) < time_useable)
            {
                cache.erase(cache.begin() + i); // 删除下标为 i 的元素
                cout << "delete vector" << i << endl;
            }
        }
    //cout << info._Bucket <<endl<< info._Endpoint << endl << info._GetobjectUrlName << endl;
    string buf;
    if (if_cache != 1)
    {
        InitializeSdk();
        ClientConfiguration conf;
        OssClient client(temp._Endpoint, AccessKeyId, AccessKeySecret, conf);
        std::time_t t = std::time(nullptr) + time_useable;

        auto genOutcome = client.GeneratePresignedUrl(temp._Bucket, temp._GetobjectUrlName, t, Http::Get);
        if (genOutcome.isSuccess()) {
            std::cout << "GeneratePresignedUrl for " << hdl.lock().get() << " success, Gen url:" << genOutcome.result().c_str() << std::endl;
            push_temp._GenedUrl= genOutcome.result().c_str();
            push_temp._Bucket = temp._Bucket;
            push_temp._GetobjectUrlName = temp._GetobjectUrlName;
            push_temp._Endpoint = temp._Endpoint;
            push_temp._request_time = (time(&now));
            cache.push_back(push_temp);
            //cache[i]._GenedUrl = genOutcome.result().c_str();//save gened Url  something wrong
            buf= genOutcome.result().c_str();//pass data

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
        s->send(hdl,e.what(), msg->get_opcode());
    }
}

int main(int argc, char* argv[]){
    int port;
    if (argc < 2 || argc > 3) {
        std::cout << "Too few or too many arguments" << std::endl;
        std::cout << "Usage: " << argv[0] << " -p <PORT> to specify server port" << std::endl;
        return 1;
    }
    else {
        if (strcmp(argv[1], "-p") != 0 && strcmp(argv[1], "--help") != 0) {
            std::cout << "Usage: " << argv[0] << " -p <PORT> to specify server port" << std::endl;
            return 1;
        }
    }

    if (argc == 3) {
        if (strcmp(argv[1], "-p") == 0) {
            int value = std::atoi(argv[2]);
            if (value != 0) {
                port = value; // Assign the port value
            }
            else {
                std::cout << "The port number must be an integer." << std::endl;
                return 1;
            }
        }
    }
    else if (argc == 2) {
        if (strcmp(argv[1], "--help") == 0) {
            std::cout <<"Usage: " << argv[0] << " -p <PORT> to specify server port" << std::endl;
            return 0;
        }
        else {
            std::cout << "Usage: " << argv[0] << " -p <PORT> to specify server port" << std::endl;
            return 1;
        }
    }

    std::cout << "Specified port: " << port << std::endl;

    cout << "server listen on:" << port << endl;
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
        //bind()
        // Listen on port 9002
        echo_server.listen(port);

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
