/*
 * UDP Connection
 * Li Ao
 * hi@leeleo.me
 */

#include "UdpConnection.hpp"

namespace NetworkService {

    void UdpConnection::Start() {
        DoRead();
    }

    UdpConnection::Pointer UdpConnection::Start(boost::asio::ip::udp::endpoint & endpoint,
            boost::asio::ip::udp::endpoint & remote_endpoint,
            boost::asio::io_service & service,
            ProcessorMap & processors) {
        Pointer connection(new UdpConnection(endpoint, remote_endpoint, service, processors));
        connection->Start();

        return connection;
    }

    void UdpConnection::DoRead() {
        socket_.async_receive_from(
                boost::asio::buffer(read_buffer_), remote_endpoint_,
                MEM_FN2(OnRead, _1, _2));
    }

    void UdpConnection::OnRead(const ErrorCode & error, std::size_t bytes) {
        if (error) Stop();
        if (! started_ ) return;

        std::string message_string(read_buffer_, bytes);
        Message::Pointer message = Message::Create(message_string);
        int type = message->GetData<int>("type");
        if (processors_.find(type) != processors_.end()) {
            processors_[type](message, shared_from_this());
        }

        DoRead();
    }
    
    void UdpConnection::PostMessage(const Message::Pointer message) {
        io_service_.post(
                [this, message]() {
                    bool write_in_progress = !message_deque_.empty();
                    message_deque_.push_back(message);
                    if (!write_in_progress) {
                        DoWrite();
                    }
                    
                });
    }

    void UdpConnection::Stop() {
        socket_.close();
        started_ = false;
    }

    void UdpConnection::DoWrite() {
        if ( ! started_ ) return;
        if (message_deque_.empty()) return;

        std::string message_string = message_deque_.front()->GetJSONString();
        std::copy(message_string.begin(), message_string.end(), write_buffer_);

        socket_.async_send_to(boost::asio::buffer(write_buffer_, message_string.size()),
                remote_endpoint_,
                MEM_FN2(OnWrite, _1, _2));
    }

    void UdpConnection::OnWrite(const ErrorCode & error, std::size_t bytes) {
        if (error) {
            Log::Instance()->Write((boost::format("Error: %1%\n") % error.message()).str());
        }
        message_deque_.pop_front();
        if (! message_deque_.empty()) {
            DoWrite();
        }
        Log::Instance()->Write((boost::format("Write %1% bytes\n") % bytes).str());
    }

}

