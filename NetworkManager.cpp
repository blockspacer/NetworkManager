/*
 * Network Manager
 * Li AO
 * hi@leeleo.me
 */

#include "NetworkManager.hpp"

namespace NetworkService {
    NetworkManager * NetworkManager::m_pInstance = NULL;

    NetworkManager * NetworkManager::Instance() {
        if (!m_pInstance) {
            m_pInstance = new NetworkManager();
        } 

        return m_pInstance;
    }

    
    void NetworkManager::RunServer() {
        server_ = Server::Start(io_service_);
        server_->set_config_data(config_data_);
        if (! server_) return;

        thread_ = ThreadPointer(new boost::thread(
                boost::bind(&boost::asio::io_service::run, &io_service_)));
        thread_->detach();
        started_ = true;
    }


    void NetworkManager::RunClient(const std::string & client_id) {
        if (remote_address_.empty()) return; 

        client_ = Client::Start(client_id, remote_address_, io_service_);
        if (! client_) return;

        thread_ = ThreadPointer(new boost::thread(
                boost::bind(&boost::asio::io_service::run, &io_service_)));
        thread_->detach();
        
        started_ = true;
    }

    void NetworkManager::TcpSendDataToServer(const Message::Pointer message) {
        client_->TcpPost(message); 
    }

    void NetworkManager::TcpSendDataToAllClient(const Message::Pointer message) {
        server_->TcpDeliverToAllClients(message);
    }

    void NetworkManager::TcpSendDataToAllClient(const Message::Pointer message, const std::string & excepted_client_id) {
        server_->TcpDeliverToAllClients(message, excepted_client_id);
    }

    void NetworkManager::UdpSendDataToAllClient(const Message::Pointer message) {
        server_->UdpDeliverToAllClients(message);
    }

    void NetworkManager::RegistServerTcpConnectionProcessor(MessageType type,
            const TcpConnection::ProcessorType & processor) {
        server_->AddTcpProcessor(type, processor);
    }

    void NetworkManager::RegistServerUdpConnectionProcessor(MessageType type,
            const UdpConnection::ProcessorType & processor) {
        server_->AddUdpProcessor(type, processor);
    }
    void NetworkManager::RegistClientTcpConnectionProcessor(MessageType type,
            const TcpConnection::ProcessorType & processor) {
        client_->AddTcpProcessor(type, processor);
    }

    void NetworkManager::RegistClientUdpConnectionProcessor(MessageType type,
            const UdpConnection::ProcessorType & processor) {
        client_->AddUdpProcessor(type, processor); 
    }
}

