/*
 * The Game Client
 * Li Ao
 * hi@leeleo.me
 */

#ifndef __NETWORKSERVICE__CLIENT__
#define __NETWORKSERVICE__CLIENT__

#include "Common.hpp"
#include "UdpConnection.hpp"
#include "TcpConnection.hpp"

namespace NetworkService {

    class Client : public boost::enable_shared_from_this<Client>,
    boost::noncopyable {
        typedef Client SelfType;  
        Client(const std::string & client_id,
                boost::asio::io_service & service, 
                const std::string & destination) 
            : io_service_(service), 
            destination_(destination),
            client_id_(client_id){}

        public:
        typedef boost::shared_ptr<Client> Pointer;

        //
        //Initialize service, this function won't start service
        //You are supposed to start service by calling `StartService()`
        //

        static Pointer Start(const std::string & client_id,
                const std::string & destination, 
                boost::asio::io_service & service);

        //
        //Post message
        //

        void TcpPost(const Message::Pointer message);

        //
        //Start Service, the processors should be set before start service
        //

        void StartService();

        //
        //Stop service
        //

        void Stop();


        //
        //Add Processors
        //

        void AddTcpProcessor(MessageType type, const TcpConnection::ProcessorType & processor);
        void AddUdpProcessor(MessageType type, const UdpConnection::ProcessorType & processor);

        void set_client_id(const std::string & client_id) {
            client_id_ = client_id;
        }

        private:
        void SetProcessors();

        //
        //These are some basic event handlers, helping you to deal
        //some network problems
        //

        void TcpConnectionTimeOut();
        void TcpDisconnect();

        private: 
        UdpConnection::Pointer udp_connection_;
        TcpConnection::Pointer tcp_connection_;

        TcpConnection::ProcessorMap tcp_processors_;
        UdpConnection::ProcessorMap udp_processors_;

        boost::asio::io_service & io_service_;
        std::string destination_;
        std::string client_id_;
    };
}

#endif 
