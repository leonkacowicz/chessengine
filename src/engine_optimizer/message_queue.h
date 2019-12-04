//
// Created by leon on 2019-11-24.
//

#ifndef CHESSENGINE_MESSAGE_QUEUE_H
#define CHESSENGINE_MESSAGE_QUEUE_H

#include <string>
#include <chrono>

struct request_message {
    std::string bucket;
    std::string white;
    std::string black;
    std::string outputdir;
    int generation;
    int id;
    int array_index;
    int movetime;
};

struct response_message {
    std::string bucket;
    std::string white;
    std::string black;
    std::string outputdir;
    int generation;
    int id;
    int movetime;
    int array_index;
    std::string result;
};

struct message_queue {
    const std::string queue_send;
    const std::string queue_receive;
    message_queue(const std::string& queue_send, const std::string& queue_receive): queue_send(queue_send), queue_receive(queue_receive) {};
    void send_message(const request_message& rm);
    bool receive_message(response_message& rm, std::chrono::seconds timeout = std::chrono::seconds(0));
};

#endif //CHESSENGINE_MESSAGE_QUEUE_H
