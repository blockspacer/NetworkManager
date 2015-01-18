#include "Message.hpp"

namespace NetworkService {

    Message::Pointer Message::Create(const std::string & JSONString) {
        boost::property_tree::ptree ptree;
        std::cout << JSONString << std::endl;
        std::istringstream stream(JSONString);
        try {
            boost::property_tree::json_parser::read_json(stream, ptree);
            Message::Pointer message(new Message(ptree));
            return message;
        } catch (std::exception & e) {
            std::cout << e.what() << std::endl;
            return NULL;
        }
    }

    Message::Pointer Message::Create() {
        Message::Pointer message(new Message());
        return message;
    }


    std::string Message::GetJSONString() const{
        std::ostringstream stream;
        boost::property_tree::write_json(stream, ptree_);
        std::string string = stream.str();
        return string;
    }

}
