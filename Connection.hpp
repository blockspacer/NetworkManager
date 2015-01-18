/*
 * Connection Base class
 * Li Ao
 * hi@leeleo.me
 */

#ifndef __NETWORKSERVICE__CONNECTION__
#define __NETWORKSERVICE__CONNECTION__

#include <unordered_map>

#include "Common.hpp"
#include "Message.hpp"
#include "MessageBuffer.hpp"

//
//This is a base cass, all connection class 
//should be inherited from this class
//


namespace NetworkService {

    template <class ConnectionType>
        class Connection: public boost::enable_shared_from_this<ConnectionType>,
        boost::noncopyable {
            public:

                // 
                // self pointer
                //

                typedef boost::shared_ptr<ConnectionType> Pointer;

                //
                //Basic type
                //

                typedef std::deque<Message::Pointer> MessageDeque;
                typedef std::function<void(Message::Pointer, Pointer)> ProcessorType;
                typedef std::unordered_map<int, ProcessorType> ProcessorMap;

                Connection(boost::asio::io_service & service,
                        ProcessorMap & processors)
                    : started_(true), io_service_(service), processors_(processors){}

                virtual void Stop() {};
                virtual void PostMessage(const Message::Pointer) {};
                bool started() { return started_; }

            protected:
                typedef ConnectionType SelfType;

                virtual void OnConnect(const ErrorCode &) {};

                virtual void DoReadHeader() {}
                virtual void DoReadBody() {}
                virtual void HandleMessage(const MessageBuffer) {}

                virtual void OnWrite(const ErrorCode &, std::size_t) {}
                virtual void DoWirte() {}

            protected:
                MessageBuffer read_buffer_;
                MessageBuffer write_buffer_;

                bool started_;
                boost::asio::io_service & io_service_;

                ProcessorMap & processors_;
                MessageDeque message_deque_;
        };
}

#endif
