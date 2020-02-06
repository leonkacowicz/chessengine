//
// Created by leon on 2019-11-24.
//

#include "message_queue.h"
#include <boost/process.hpp>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

std::string to_json(const request_engine_config& rec) {
    std::stringstream ss;
    ss << "{"
       << R"("exec": ")" << rec.exec << "\""
       << R"(, "options_file": ")" << rec.options_file << "\""
       << R"(, "weights_file": ")" << rec.weights_file << "\""
       << R"(, "movetime": )" << rec.movetime
       << R"(, "depth": )" << rec.depth
       << R"(, "initial_time": )" << rec.initial_time
       << R"(, "time_increment": )" << rec.time_increment
       << "}";
    return ss.str();
}

std::string to_json(const request_message& rm) {
    std::stringstream ss;
    ss << "{"
        << R"("id": )" << rm.id
        << R"(, "generation": )" << rm.generation
        << R"(, "array_index": )" << rm.array_index
        << R"(, "bucket": ")" << rm.bucket << "\""
        << R"(, "queue": ")" << rm.queue << "\""
        << R"(, "outputdir": ")" << rm.outputdir << "\""
        << R"(, "game_id": ")" << rm.game_id << "\""
        << R"(, "initial_pos": ")" << rm.initial_pos << "\""
        << R"(, "white": )" << to_json(rm.white)
        << R"(, "black": )" << to_json(rm.black)
        << "}";
    return ss.str();
}

void message_queue::send_message(const request_message& rm) {
    using namespace boost::process;
    ipstream ips;
    system(search_path("aws"), "sqs", "send-message",
            "--region", "us-west-2",
            "--queue-url", queue_send,
            "--message-body", to_json(rm),
            "--message-group-id", "1",
            std_out > ips);
}

bool message_queue::receive_message(response_message& rm, std::chrono::seconds timeout) {
    using namespace boost::process;
    if (timeout.count() > 0) {
        ipstream ips;
        ipstream ips2;
        auto aws = search_path("aws");
        system(aws, "sqs", "receive-message", "--region", "us-west-2", "--queue-url", queue_receive,
                "--visibility-timeout", "30", "--wait-time-seconds", "10", "--max-number-of-messages", "1", std_out > ips);

        try {
            boost::property_tree::ptree json;
            boost::property_tree::read_json(ips, json);

            std::string receipt = json.get_child("Messages").front().second.get_child("ReceiptHandle").data();
            std::stringstream ss;
            ss << json.get_child("Messages").front().second.get_child("Body").data();
            std::cout << ss.str() << std::endl << std::endl;
            boost::property_tree::read_json(ss, json);

            rm.id = json.get<int>("id");
            rm.generation = json.get<int>("generation");
            rm.array_index = json.get<int>("array_index");
            rm.result = json.get<std::string>("result");
            rm.white = json.get<std::string>("white");
            rm.black = json.get<std::string>("black");

            system(aws, "sqs", "delete-message", "--region", "us-west-2", "--queue-url", queue_receive, "--receipt-handle", receipt, std_out > ips2);
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    return false;
}
