/*
 * Message Buffer
 * Li Ao
 * hi@leeleo.me
 */

#ifndef __NETWORKSERVICE__MESSAGEBUFFER__
#define __NETWORKSERVICE__MESSAGEBUFFER__

#include "Common.hpp"

//
//Use the message buffer to ensure that 
//the message is complete without damaged 
//during the transportation
//

class MessageBuffer {
public:

    //
    //Set the buffer size
    //

    enum { kHeaderLength = 4 };
    enum { kMaxBodyLength = 1028 };

    MessageBuffer()
        : body_length_(0) {}

    char * data() {
        return data_;
    }

    std::size_t length() const {
        return kHeaderLength + body_length_;
    }

    const char * body() const {
        return data_ + kHeaderLength;
    }

    char * body() {
        return data_ + kHeaderLength;
    }

    std::size_t body_length() const {
        return body_length_;
    }

    void set_body_length(std::size_t length) {
        body_length_ = length;
        if (body_length_ > kMaxBodyLength) {
            body_length_ = kMaxBodyLength;
        }
    }

    bool DecodeHeader();

    void EncodeHeader();

private:
    char data_[kHeaderLength + kMaxBodyLength];
    std::size_t body_length_;
};

#endif

