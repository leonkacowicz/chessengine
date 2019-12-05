//
// Created by leon on 2019-11-24.
//

#include <gtest/gtest.h>
#include <algorithm.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace chess::optimizer;

TEST(algorithm_test, add_generation_test) {

    algorithm a(3, 4);
    a.run();
}

TEST(ptree_test, ptree_test1) {
    using namespace boost::property_tree;

    ptree json;
    std::stringstream ss;
    ss << R"({
    "Messages": [
        {
            "MessageId": "13fa6165-a669-4d91-8a4d-f69c63a98b4b",
            "ReceiptHandle": "AQEBquqQzliyjPRkdAxi/bIvxqKUjrsTcPbxuJDWDGyPxHmLs4hmP+mhxuPaqJ7lZyY2A3Htv5f+nZ2Y1PWZRdAZTHgeiZ0yFhRa1bCID1s/Fa7MHcMgrKp3/vVo+No+kd7P9eFoZSh1nx8S/B6+3i2k74fdAm0cRCn8daW3FJNrPDdd4xd3aXKquIDdIKtVJRJ5tXk6/atXEYfkmddtXgUWKdIKxNJhlQYwZEWp4PcZOtzvz2fJbv0XffcIj/SBhT2NpCTml2oIt+6BiM7g3SIPufCT+de3hMJLnzt7Bb+TNYKBze9Jh8Six5RploBi3I2/D/qYtjk8tAEVCTqYWy8tdo6dxsTwvClgpEl+RqPeIPyM7lkcrVziim0qcSuiTm9amNj/QrUpCL1zov/sADM3tg==",
            "MD5OfBody": "096b20222eda1e0fa35c9ef6a157795e",
            "Body": "{\n  \"bucket\": \"leonkacowicz\",\n  \"outputdir\": \"chess/generations/\",\n  \"white\": \"3f0c559dfa7954d375835967edb155587c0091dcb62c2b5de44b194a25934268\",\n  \"black\": \"bccc97094406a7dd19fa5f785301f1fd34bf2b7d9aafd2a91942981e74bc7d48\",\n  \"movetime\": \"100\",\n  \"result\": \"1-0\"\n}"
        }
    ]
}
)";
    read_json(ss, json);
    //std::cout << json.get_child("Messages.0.ReceiptHandle").data();
    for (auto& msg : json.get_child("Messages")) {
        std::cout << msg.second.get<std::string>("ReceiptHandle").data() << std::endl;
        ptree body;

        std::stringstream ss2(msg.second.get<std::string>("Body"));
        read_json(ss2, body);
        for (auto& field : body) {
            std::cout << field.first << " = " << field.second.data() << std::endl;
        }
    }

    std::cout << json.size();
}