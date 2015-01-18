/*
 * UDP Connection
 * Li Ao
 * hi@leeleo.me
 */

#ifndef __NETWORKSERVICE__UDPCONNECTION__
#define __NETWORKSERVICE__UDPCONNECTION__

#include <boost/format.hpp>

#include "Connection.hpp"

namespace NetworkService {
    class UdpConnection: public Connection<UdpConnection> {
        UdpConnection(boost::asio::ip::udp::endpoint & endpoint,
                boost::asio::ip::udp::endpoint & remote_endpoint,
                boost::asio::io_service & service,
                ProcessorMap & processors)
            : Connection<UdpConnection>(service, processors), socket_(service, endpoint), 
            remote_endpoint_(remote_endpoint) {}  

        void Start();

        public:

        //
        //Create a UDP Connection
        //

        static Pointer Start(boost::asio::ip::udp::endpoint & endpoint,
                boost::asio::ip::udp::endpoint & remote_endpoint,
                boost::asio::io_service & service,
                ProcessorMap & processors);
        
        //
        //Stop the connection, close the socket
        //

        void Stop();

        //
        //Send a message
        //

        void PostMessage(const Message::Pointer message);
        
        private:

        //
        //Read Callback
        //

        void DoRead();
        size_t ReadComplete(const ErrorCode & error, size_t bytes);
        void OnRead(const ErrorCode & error, size_t bytes);

        //
        //Write Callback
        //

        void DoWrite();
        void OnWrite(const ErrorCode & error, std::size_t bytes);

        private:
            char write_buffer_[Config::kMaxMessage];
            char read_buffer_[Config::kMaxMessage];
            boost::asio::ip::udp::socket socket_;
            boost::asio::ip::udp::endpoint remote_endpoint_;
    };
}

#endif
