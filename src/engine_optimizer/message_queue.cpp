//
// Created by leon on 2019-11-24.
//

#include "message_queue.h"
#include <boost/process.hpp>
#include <sstream>


void message_queue::send_message(const request_message& rm) {
    using namespace boost::process;
    std::stringstream msg;
    msg << "{"
        << R"("id": )" << rm.id
        << R"("generation": )" << rm.generation
        << R"(, "bucket": ")" << rm.bucket << "\""
        << R"(, "outputdir": ")" << rm.outputdir << "\""
        << R"(, "white": ")" << rm.white << "\""
        << R"(, "black": ")" << rm.black << "\""
        << "}";
    //aws sqs send-message --region us-west-2 --queue-url "${QUEUE_URL}" --message-body "$MSG" --message-group-id 1 --message-deduplication-id "${DEDUP}"
    const std::string queue_url = "https://sqs.us-west-2.amazonaws.com/284217563291/games.fifo";
    ipstream ips;
    system(search_path("aws"), "send-message", "--region", "us-west-2",
            "--queue-url", queue_url, "--message-body", msg.str(), "--message-group-id", "1", std_out > ips);
}

bool message_queue::receive_message(response_message& rm, std::chrono::seconds timeout) {
    using namespace boost::process;
    const std::string queue_url = "https://sqs.us-west-2.amazonaws.com/284217563291/game_results";
    if (timeout.count() > 0) {
        system(search_path("aws"), "receive-message", "--region", "us-west-2", "--queue-url", queue_url);
    }
    return false;
}
