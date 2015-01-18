/*
 * Network Manager
 * Li Ao
 * hi@leeleo.me
 */

#ifndef __NETWORKSERVICE__NETWORKMANAGER__
#define __NETWORKSERVICE__NETWORKMANAGER__

#include <boost/thread.hpp>

#include "Common.hpp"
#include "Server.hpp"
#include "Client.hpp"

namespace NetworkService {
    class NetworkManager: public boost::noncopyable {
        public:
        static NetworkManager * Instance();
        typedef boost::shared_ptr<boost::thread> ThreadPointer;

        //
        //Run the service
        //
        
        void RunClient(const std::string & client_id);
        void RunServer();
        
        //
        //You have to set remote address before Start service
        //

        void set_remote_address(const std::string & address) {
            remote_address_ = address;
        }

        //
        //These are apis to send data to server
        //

        void TcpSendDataToServer(const Message::Pointer message);
        /* void UdpSendDataToServer(const Message::Pointer message); */ // Not supported yet
        void TcpSendDataToAllClient(const Message::Pointer message);
        void TcpSendDataToAllClient(const Message::Pointer message, const std::string & excepted_client_id);
        void UdpSendDataToAllClient(const Message::Pointer message);

        //
        //Regester the Processor
        //
        
        void RegistServerTcpConnectionProcessor(MessageType type, 
                const TcpConnection::ProcessorType & processor);
        void RegistServerUdpConnectionProcessor(MessageType type,
                const UdpConnection::ProcessorType & processor);
        void RegistClientTcpConnectionProcessor(MessageType type,
                const TcpConnection::ProcessorType & processor);
        void RegistClientUdpConnectionProcessor(MessageType type,
                const UdpConnection::ProcessorType & processor);

        //
        //check whether service started
        //

        bool started() { return started_; }

        void set_config_data(const std::string & config_data) { config_data_ = config_data; }
        
        ThreadPointer thread() { return thread_; }

        private:
        NetworkManager()
            : started_(false) {};
        ~NetworkManager();
        void SetProcessors();
        void RunServerBackground();
        void RunClientBackground(const std::string & client_id);



        private:
        bool started_;
        static NetworkManager * m_pInstance;

        std::string remote_address_;
        Client::Pointer client_;
        Server::Pointer server_;

        ThreadPointer thread_;
         
        boost::asio::io_service io_service_;
        std::string config_data_;
        std::string remote_ip_;
    };
}

#endif
