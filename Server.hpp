/*
 * Server
 * Li Ao
 * hi@leeleo.me
 */

#ifndef __NETWORKSERVICE__SERVER__
#define __NETWORKSERVICE__SERVER__

#include "Common.hpp"

#include <unordered_map>
#include <vector>

#include "UdpConnection.hpp"
#include "TcpConnection.hpp"

namespace NetworkService {
    
    class Server: public boost::enable_shared_from_this<Server>,
    boost::noncopyable {

        //
        //Basic types
        //

        typedef Server SelfType;
        typedef std::vector<TcpConnection::Pointer> TcpConnectionsVector;
        typedef std::unordered_map<std::string, UdpConnection::Pointer> UdpConnectionsMap;
        typedef std::unordered_map<std::string, bool> TcpConnectionAlive;
        
        Server(boost::asio::io_service & service)
        : io_service_(service),
        acceptor_(service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), Config::kServerPort)),
        timer_(service){}
        
    public:
        typedef boost::shared_ptr<Server> Pointer;
        
        //
        //Start service
        //

        static Pointer Start(boost::asio::io_service & service);

        //
        //Send message with defferent
        //Protocol
        //

        void TcpDeliverToAllClients(const Message::Pointer message);
        void TcpDeliverToAllClients(const Message::Pointer message, const std::string & excepted_client_id);
        void UdpDeliverToAllClients(const Message::Pointer message);

        //
        //Stop the service
        //

        void Stop();

        //
        //Add Tcp/Udp Processors
        //

        void AddTcpProcessor(MessageType type, const TcpConnection::ProcessorType & processor);
        void AddUdpProcessor(MessageType type, const UdpConnection::ProcessorType & processor);

        //
        //Setter & Getter
        //
        
        void set_config_data(std::string config_data) { config_data_ = config_data_; }
        
    private:
        void StartTcpListening();
        void TcpConnect(TcpConnection::SocketPointer socket, const ErrorCode & error);
        void ClientDisconnect(const std::string & client_id);
        
        void SetProcessors();
        bool CheckConnection(Connection<TcpConnection>::Pointer connection);
        
    private:
        boost::asio::io_service & io_service_;
        
        boost::asio::ip::tcp::acceptor acceptor_;
        boost::asio::deadline_timer timer_;
        
        TcpConnection::ProcessorMap  tcp_processors_;
        UdpConnection::ProcessorMap  udp_processors_;
        
        TcpConnectionsVector tcp_connections_;
        UdpConnectionsMap udp_connections_;
        TcpConnectionAlive connection_alive_;

        std::string config_data_;
    };
}

#endif
