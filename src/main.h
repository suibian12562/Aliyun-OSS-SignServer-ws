#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <fstream>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <alibabacloud/oss/OssClient.h>
#include <nlohmann/json.hpp>

using namespace std;
using namespace nlohmann;
using namespace AlibabaCloud::OSS;

struct Config {
    std::string AccessKeyId;
    std::string AccessKeySecret;
    long sign_time;
    int port;
};

struct bucket_info
{
    string _Bucket;
    string allowed;
    string password;
};

struct message_info
{
    std::string _Endpoint;
    std::string _Bucket;
    std::string _GetobjectUrlName;
    std::string _GenedUrl;
    long _request_time;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(message_info, _Endpoint, _Bucket, _GetobjectUrlName)
};

typedef websocketpp::server<websocketpp::config::asio> server;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
typedef server::message_ptr message_ptr;
vector<message_info> cache;
vector<bucket_info> bucket_permissions;
Config config;

string subreplace(string resource_str, string sub_str, string new_str)
{
    string::size_type pos = 0;
    while ((pos = resource_str.find(sub_str)) != string::npos)
    {
        resource_str.replace(pos, sub_str.length(), new_str);
    }
    return resource_str;
}

void createDefaultConfig(const std::string& filename) {
    Config config;
    config.AccessKeyId = "your_access_key";
    config.AccessKeySecret = "your_access_secret";
    config.sign_time = 40;
    config.port = 1145;

    std::ofstream file(filename);
    if (file.is_open()) {
        nlohmann::json jsonData = {
            {"AccessKeyId", config.AccessKeyId},
            {"AccessKeySecret", config.AccessKeySecret},
            {"sign_time", config.sign_time},
            {"port",config.port}
        };
        file << jsonData.dump(4);  // Write JSON with indentation
        file.close();
    }
    else {
        std::cerr << "Error: Unable to create config file." << std::endl;
    }
}

Config readConfigFromFile(const string& filename) {
    Config config;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Config file not found. Creating a default config file.Please config and restart the program" << std::endl;
        createDefaultConfig(filename);
        return config;  // write default config
    }

    try {
        json jsonData;
        file >> jsonData;

        config.AccessKeyId = jsonData["AccessKeyId"];
        config.AccessKeySecret = jsonData["AccessKeySecret"];
        config.sign_time = jsonData["sign_time"];
        config.port = jsonData["port"];
    }
    catch (const std::exception& e) {
        std::cerr << "Error: Unable to parse config JSON: " << e.what() << std::endl;
    }

    file.close();
    return config;
}

void createDefaultBucketJson(const std::string& filename) {
    std::vector<bucket_info> default_buckets = {
        {"default_bucket1", "yes", "password1"},
        {"default_bucket2", "no", "password2"}
    };

    json json_data;

    for (const auto& bucket : default_buckets) {
        json_data["buckets"].push_back({
            {"_Bucket", bucket._Bucket},
            {"allowed", bucket.allowed},
            {"password", bucket.password}
        });
    }

    std::ofstream file(filename);
    if (file.is_open()) {
        file << json_data.dump(4);
        file.close();
    } else {
        std::cerr << "Error: Unable to create " << filename << " file." << std::endl;
    }
}

std::vector<bucket_info> readBucketInfoFromFile(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: " << filename << " not found. Creating a default file." << std::endl;
        createDefaultBucketJson(filename);
        return {};
    }

    try {
        json json_data;
        file >> json_data;

        std::vector<bucket_info> bucket_permissions;

        if (json_data.contains("buckets") && json_data["buckets"].is_array()) {
            for (const auto& bucket : json_data["buckets"]) {
                bucket_permissions.push_back({
                    bucket["_Bucket"],
                    bucket["allowed"],
                    bucket["password"]
                });
            }
        } else {
            std::cerr << "Error: Invalid JSON structure in file: " << filename << std::endl;
        }

        return bucket_permissions;
    } catch (const std::exception& e) {
        std::cerr << "Error: Unable to parse JSON in file " << filename << ": " << e.what() << std::endl;
        return {};
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