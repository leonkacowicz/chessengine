//
// Created by leon on 2019-11-24.
//

#include "message_queue.h"
#include <boost/process.hpp>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>


void message_queue::send_message(const request_message& rm) {
    using namespace boost::process;
    std::stringstream msg;
    msg << "{"
        << R"("id": )" << rm.id
        << R"(, "generation": )" << rm.generation
        << R"(, "bucket": ")" << rm.bucket << "\""
        << R"(, "outputdir": ")" << rm.outputdir << "\""
        << R"(, "white": ")" << rm.white << "\""
        << R"(, "black": ")" << rm.black << "\""
        << R"(, "movetime": ")" << rm.movetime << "\""
        << "}";
    ipstream ips;
    system(search_path("aws"), "sqs", "send-message",
            "--region", "us-west-2",
            "--queue-url", queue_send,
            "--message-body", msg.str(),
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
            std::cout << ss.str();
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
