//
// Created by leon on 2019-11-24.
//

#ifndef CHESSENGINE_MESSAGE_QUEUE_H
#define CHESSENGINE_MESSAGE_QUEUE_H

#include <string>
#include <chrono>
#include <utility>

struct request_engine_config {
    std::string exec;
    std::string options_file;
    std::string weights_file;
    int movetime = 0;
    int depth = 0;
    int initial_time = 0;
    int time_increment = 0;
};

struct request_message {
    std::string bucket;
    std::string queue;
    request_engine_config white;
    request_engine_config black;
    std::string outputdir;
    std::string game_id;
    std::string initial_pos;
    int generation = 0;
    int id = 0;
    int array_index = 0;
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
    message_queue(std::string queue_send, std::string queue_receive): queue_send(std::move(queue_send)), queue_receive(std::move(queue_receive)) {};
    void send_message(const request_message& rm);
    bool receive_message(response_message& rm, std::chrono::seconds timeout = std::chrono::seconds(0));
};

#endif //CHESSENGINE_MESSAGE_QUEUE_H
