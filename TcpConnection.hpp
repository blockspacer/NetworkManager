/*
 * Tcp Connection
 * Li Ao
 * hi@leeleo.me
 */

#ifndef __NETWORKSERVICE__TCPCONNECTION__
#define __NETWORKSERVICE__TCPCONNECTION__

#include <boost/format.hpp>

#include "Connection.hpp"

namespace NetworkService {

    class TcpConnection: public Connection<TcpConnection> {
        public:
            typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketPointer;

        private:
            TcpConnection(boost::asio::io_service & service,
                    SocketPointer socket,
                    std::string client_id,
                    ProcessorMap & processors);

            TcpConnection(boost::asio::io_service & service,
                    SocketPointer socket,
                    ProcessorMap & processors)
                : Connection<TcpConnection>(service, processors), 
                socket_(socket),
                timer_(service) {}

            void Start(boost::asio::ip::tcp::endpoint & endpoint);
        public:

            //
            //Run this Start to start a client
            //

            static Pointer Start(boost::asio::ip::tcp::endpoint & endpoint,
                    boost::asio::io_service & service,
                    const std::string & client_id,
                    ProcessorMap & processors);

            //
            //Run this Start to start a server
            //

            static Pointer Start(boost::asio::io_service & service,
                    SocketPointer socket,
                    ProcessorMap & processors);

            //
            //Stop the connection, close the socket
            //

            void Stop();

            //
            //Send message
            //

            void PostMessage(const Message::Pointer message);

            std::string client_id() const { return client_id_; }
            void set_client_id(const std::string & client_id) { client_id_ = client_id; }
            SocketPointer socket() { return socket_; }
        private:

            //
            //Connection Callback
            //

            void OnConnect(const ErrorCode & error);

            //
            //Timer Callback, Check if connection timeout
            //

            void TimerExpire();

            //
            //Message receiving callback
            //

            void DoReadHeader();
            void DoReadBody();

            //
            //Processing the message recieved
            //

            void HandleMessage(const MessageBuffer message_buffer);

            //
            //Write Message Handler
            //

            void DoWrite();
            void OnWrite(const ErrorCode & error, std::size_t bytes);

        private:
            SocketPointer socket_;
            std::string client_id_;

            boost::asio::deadline_timer timer_;
    };
}

#endif
