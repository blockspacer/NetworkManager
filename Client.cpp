/*
 * The Game Client
 * Li Ao
 * hi@leeleo.me
 */

#include "Client.hpp"

namespace NetworkService {

    void Client::StartService() {
        boost::asio::ip::tcp::endpoint tcp_endpoint(boost::asio::ip::address::from_string(destination_), Config::kServerPort); 
        boost::asio::ip::udp::endpoint udp_endpoint(boost::asio::ip::udp::v4(), Config::kClientPort);
        boost::asio::ip::udp::endpoint udp_remote_endpoint(boost::asio::ip::address::from_string(destination_), Config::kServerPort);

        udp_connection_ = UdpConnection::Start(udp_endpoint, udp_remote_endpoint, io_service_, udp_processors_);
        tcp_connection_ = TcpConnection::Start(tcp_endpoint, io_service_, client_id_, tcp_processors_);

    }

    Client::Pointer Client::Start(const std::string & client_id,
            const std::string & destination, 
            boost::asio::io_service & service) {
        Pointer client(new Client(client_id, service, destination));
        client->SetProcessors();
        client->StartService();
        return client;
    }

    void Client::TcpPost(const Message::Pointer message) {
        if (! tcp_connection_->started()) return;

        tcp_connection_->PostMessage(message);
    }

    void Client::Stop() {
        tcp_connection_->Stop();
        udp_connection_->Stop();
    }

    void Client::AddTcpProcessor(MessageType type, const TcpConnection::ProcessorType & processor) {
        int type_id = static_cast<int>(type);
        tcp_processors_[type_id] = processor;
    }

    void Client::AddUdpProcessor(MessageType type, const UdpConnection::ProcessorType & processor) {
        int type_id = static_cast<int>(type);
        udp_processors_[type_id] = processor;
    }

    void Client::SetProcessors() {
        int type = static_cast<int>(MessageType::kDisconnect);
        auto disconnect_processor = [&](Message::Pointer, TcpConnection::Pointer) {
            tcp_connection_->Stop(); 
            boost::asio::ip::tcp::endpoint tcp_endpoint(boost::asio::ip::address::from_string(destination_), Config::kServerPort); 
            tcp_connection_ = TcpConnection::Start(tcp_endpoint, io_service_, client_id_, tcp_processors_);
        };
        tcp_processors_[type] = disconnect_processor;

        type = static_cast<int>(MessageType::kConnectionTimeOut);
        auto time_out_processor = [&](Message::Pointer, TcpConnection::Pointer) {
            std::cout << "Time out" << std::endl;
            boost::asio::ip::tcp::endpoint tcp_endpoint(boost::asio::ip::address::from_string(destination_), Config::kServerPort); 
            tcp_connection_ = TcpConnection::Start(tcp_endpoint, io_service_, client_id_, tcp_processors_);
        };
        tcp_processors_[type] = time_out_processor;
    }
}
