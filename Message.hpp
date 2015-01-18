/*
 * Message
 * Li Ao
 * hi@leeleo.me
 */

#ifndef __NETWORKSERVICE__MESSAGE__
#define __NETWORKSERVICE__MESSAGE__

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "Common.hpp"

namespace NetworkService {

    //
    //You should create a new message everytime
    //you want to delivery a message
    //

    class Message : public boost::enable_shared_from_this<Message>,
    boost::noncopyable{
        Message() {};
        Message(const boost::property_tree::ptree & ptree)
            :ptree_(ptree) {};

        public:
        typedef boost::shared_ptr<Message> Pointer;

        //
        //Create a empty message
        //you can set the data you want
        //

        static Pointer Create();

        //
        //Create a message with a JSONString
        //You can get the data from the string
        //

        static Pointer Create(const std::string & JSONString);


        //
        //Set data to string
        //

        template <class DataType> void PushData(const std::string & key, DataType data);

        //
        //Get data from string
        //

        template <class DataType> DataType GetData(const std::string & key) const;

        //
        //Get JSONString
        //

        std::string GetJSONString() const;

        private:
        boost::property_tree::ptree ptree_;
    };

    template <class DataType>
        inline void Message::PushData(const std::string & key, DataType data) {
            ptree_.put(key, data);
        }

    template <class DataType>
        inline DataType Message::GetData(const std::string & key) const{
            try {
                return ptree_.get<DataType>(key);
            }
            catch(std::exception &) {
                return DataType(); 
            }
        }
}

#endif
