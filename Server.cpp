/*
 * Server
 * Li Ao
 * hi@leeleo.me
 */
#include "Server.hpp"

namespace NetworkService {
    Server::Pointer Server::Start(boost::asio::io_service & service) {
        try {
            Pointer server(new Server(service));
            server->SetProcessors();
            server->StartTcpListening();
            return server;
        }
        catch(std::exception & exception) {
            std::cout << exception.what() << std::endl; 
            return NULL;
        }
    }
    
    void Server::TcpDeliverToAllClients(const Message::Pointer message) {
        TcpDeliverToAllClients(message, std::string());
    }

    void Server::TcpDeliverToAllClients(const Message::Pointer message, const std::string & excepted_client_id) {
        for (auto it = tcp_connections_.begin(); it != tcp_connections_.end(); ) {
            if (! CheckConnection(*it)) {
                it = tcp_connections_.erase(it); 
            } else {
                if ((*it)->client_id() == excepted_client_id) continue;
                (*it)->PostMessage(message);
                it++;
            }
        } 
    }
    
    void Server::UdpDeliverToAllClients(const Message::Pointer message) {
        for (auto it = tcp_connections_.begin(); it != tcp_connections_.end(); ) {
            if (! CheckConnection(*it)) {
                it = tcp_connections_.erase(it); 
            } else {
                std::string client_id = (*it)->client_id();
                udp_connections_[client_id]->PostMessage(message);
                it++;
            }
        } 
    }
    
    bool Server::CheckConnection(Connection<TcpConnection>::Pointer connection) {
        if (! connection->started()) {
            std::string client_id = connection->client_id();
            if (udp_connections_.find(client_id) != udp_connections_.end()) {
                udp_connections_[client_id]->Stop();
                udp_connections_.erase(client_id);
            }
            return false;
        } 
        return true;
    }
    
    void Server::StartTcpListening() {
        try {
            TcpConnection::SocketPointer socket(new boost::asio::ip::tcp::socket(io_service_));
            acceptor_.async_accept(*socket,
                    MEM_FN2(TcpConnect, socket, _1));
        }
        catch(std::exception & exception) {
            std::cout << "Error: ";
            std::cout << exception.what() << std::endl;
        }
    }
    
    void Server::TcpConnect(TcpConnection::SocketPointer socket, const ErrorCode & error) {
        if (! error) {
            TcpConnection::Pointer connection = TcpConnection::Start(io_service_, socket, tcp_processors_);
            tcp_connections_.push_back(connection);
            
            StartTcpListening();
        } else {
            std::cout << error.message() << std::endl;
        }
    }

    void Server::AddTcpProcessor(MessageType type, const TcpConnection::ProcessorType & processor) {
        int type_id = static_cast<int>(type);
        tcp_processors_[type_id] = processor;
    }

    void Server::AddUdpProcessor(MessageType type, const UdpConnection::ProcessorType & processor) {
        int type_id = static_cast<int>(type);
        udp_processors_[type_id] = processor; 
    }
    
    void Server::SetProcessors() {
        int type = static_cast<int>(MessageType::kLogin);

        auto connect_processor = [&](Message::Pointer message, TcpConnection::Pointer connection) { 
            std::string client_id = message->GetData<std::string>("client_id");

            if ((connection_alive_.find(client_id) != connection_alive_.end()) &&
                    connection_alive_[client_id]) {
                Message::Pointer reply = Message::Create();
                int type = static_cast<int>(MessageType::kUsernameFailed);
                reply->PushData("type", type);
                connection->PostMessage(reply);
                connection->Stop();
                return;
            }

            boost::asio::ip::tcp::endpoint tcp_endpoint = connection->socket()->remote_endpoint();
            boost::asio::ip::udp::endpoint udp_endpoint(boost::asio::ip::udp::v4(), Config::kServerPort);
            boost::asio::ip::udp::endpoint udp_remote_endpoint(tcp_endpoint.address(), Config::kClientPort);
            UdpConnection::Pointer udp_connection = UdpConnection::Start(udp_endpoint, udp_remote_endpoint, io_service_, udp_processors_);
            
            udp_connections_[client_id] = udp_connection;
            connection->set_client_id(client_id);
            connection_alive_[client_id] = true;

            int type = static_cast<int>(MessageType::kSuccess);
            Message::Pointer reply = Message::Create();
            reply->PushData("type", type); 
            reply->PushData("config_data", config_data_);
            
            udp_connection->PostMessage(reply);
            connection->PostMessage(reply);
        };
        tcp_processors_[type] = connect_processor;

        type = static_cast<int>(MessageType::kDisconnect);
        auto disconnect_processor = [&](Message::Pointer, TcpConnection::Pointer connection) {
            std::cout << "disconnect" << std::endl;
            std::string client_id = connection->client_id();
            connection_alive_[client_id] = false;
            udp_connections_[client_id]->Stop();
            udp_connections_.erase(client_id);

            auto self(shared_from_this());
            //wait 5 secondes
            timer_.expires_from_now(boost::posix_time::milliseconds(5000));
            timer_.async_wait(MEM_FN1(ClientDisconnect, client_id));
        };
        tcp_processors_[type] = disconnect_processor;
    }

    void Server::ClientDisconnect(const std::string & client_id) {
        
        if (! connection_alive_[client_id]) {
            std::cout << "True kDisconnect" << std::endl;
            connection_alive_.erase(client_id);
            Message::Pointer reply = Message::Create();
            int type = static_cast<int>(MessageType::kClientOffline);
            reply->PushData("type", type);
            reply->PushData("client_id", client_id);
            TcpDeliverToAllClients(reply);
        }
    }
}

