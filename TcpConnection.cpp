/*
 * Tcp Connection
 * Li Ao
 * hi@leeleo.me
 */ 

#include "TcpConnection.hpp"

namespace NetworkService {

    void TcpConnection::Start(boost::asio::ip::tcp::endpoint & endpoint) {
        timer_.expires_from_now(boost::posix_time::seconds(20));
        socket_->async_connect(endpoint, MEM_FN1(OnConnect, _1));
        timer_.async_wait(MEM_FN(TimerExpire));
    }


    TcpConnection::TcpConnection(boost::asio::io_service & service,
            SocketPointer socket,
            std::string client_id,
            ProcessorMap & processors)
        : Connection<TcpConnection>(service, processors), 
        socket_(socket), 
        client_id_(client_id), 
        timer_(service){
            timer_.expires_from_now(boost::posix_time::pos_infin);
        }

    TcpConnection::Pointer TcpConnection::Start(boost::asio::ip::tcp::endpoint & endpoint,
            boost::asio::io_service & service,
            const std::string & client_id,
            ProcessorMap & processors) {
        SocketPointer socket(new boost::asio::ip::tcp::socket(service));
        Pointer connection(new TcpConnection(service, socket, client_id, processors)); 
        connection->Start(endpoint);

        return connection;
    }

    TcpConnection::Pointer TcpConnection::Start(boost::asio::io_service & service,
            SocketPointer socket,
            ProcessorMap & processors) {
        Pointer connection(new TcpConnection(service, socket, processors));

        connection->DoReadHeader();
        return connection; 
    }

    void TcpConnection::Stop() {
        if ( !started_ ) return;
        started_ = false;
        timer_.cancel();
        socket_->close();
    }

    void TcpConnection::PostMessage(const Message::Pointer message) {
        io_service_.post(
                [this, message]() {
                bool write_in_progress = !message_deque_.empty();
                message_deque_.push_back(message);
                if (!write_in_progress) {
                DoWrite();
                }
                });
    }

    void TcpConnection::OnConnect(const ErrorCode & error) {
        if (! error) {
            Message::Pointer message = Message::Create();
            message->PushData("type", static_cast<int>(MessageType::kLogin));
            message->PushData("client_id", client_id_);
            PostMessage(message);
        } else {
            Log::Instance()->Write(error.message());
        }
    }

    void TcpConnection::TimerExpire() {
        if (!started_) return;     
//        if (timer_.expires_at() <= boost::asio::deadline_timer::traits_type::now()) {
//            Stop();
//            int type = static_cast<int>(MessageType::kConnectionTimeOut);
//            processors_[type](NULL, NULL);
//        }
//
        timer_.expires_at(boost::posix_time::pos_infin);
    }

    void TcpConnection::DoReadHeader() {
        auto self(shared_from_this());
        boost::asio::async_read(*socket_,
                boost::asio::buffer(read_buffer_.data() ,MessageBuffer::kHeaderLength), 
                [this, self](ErrorCode error, std::size_t) {
                if (!error && read_buffer_.DecodeHeader()) {
                DoReadBody();
                } else {
                if (!started_) return;
                int type = static_cast<int>(MessageType::kDisconnect);
                processors_[type](NULL, self);
                Stop(); 
                }
                });
    }

    void TcpConnection::HandleMessage(const MessageBuffer message_buffer) {
        std::string message_string(message_buffer.body(), message_buffer.body_length());   
        Message::Pointer message = Message::Create(message_string); 
        if (! message) return;
        int type = message->GetData<int>("type");
        if (processors_.find(type) != processors_.end())  {
            processors_[type](message, shared_from_this());
        }
    }

    void TcpConnection::DoReadBody() {
        auto self(shared_from_this());
        boost::asio::async_read(*socket_,
                boost::asio::buffer(read_buffer_.body(), read_buffer_.body_length()),
                [this, self](ErrorCode error, std::size_t) {
                if (! error) {
                HandleMessage(read_buffer_);
                DoReadHeader();
                } else {
                if (!started_) return;
                int type = static_cast<int>(MessageType::kDisconnect);
                processors_[type](NULL, self);
                Stop();
                }
                });
    }


    void TcpConnection::DoWrite() {
        if ( !started_ ) return;
        if (message_deque_.empty()) return;

        const char * message_string = message_deque_.front()->GetJSONString().c_str();
        write_buffer_.set_body_length(std::strlen(message_string));
        std::memcpy(write_buffer_.body(), message_string, write_buffer_.body_length());
        write_buffer_.EncodeHeader();


        socket_->async_write_some(boost::asio::buffer(write_buffer_.data(), write_buffer_.length()),
                MEM_FN2(OnWrite, _1, _2));
    }

    void TcpConnection::OnWrite(const ErrorCode & error, std::size_t bytes) {
        if (error) {
            Log::Instance()->Write((boost::format("Error: %1%\n") % error.message()).str());
        } else {
            message_deque_.pop_front();
            if (!message_deque_.empty()) {
                DoWrite();
            } else {
                DoReadHeader();
            }
            Log::Instance()->Write((boost::format("Write %1% bytes\n") % bytes).str());
        }
    }
}
