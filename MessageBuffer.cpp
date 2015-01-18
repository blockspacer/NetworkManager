/*
 * Message Buffer
 * Li Ao
 * hi@leeleo.me
 */

#include "MessageBuffer.hpp"

bool MessageBuffer::DecodeHeader() {
    char header[kHeaderLength + 1] = "";
    std::strncat(header, data_, kHeaderLength);
    body_length_ = std::atoi(header);
    if (body_length_ > kMaxBodyLength) {
        body_length_ = 0;
        return false;
    }
    return true;
}

void MessageBuffer::EncodeHeader() {
    char header[kHeaderLength + 1] = "";
    std::sprintf(header, "%4d", static_cast<int>(body_length_));
    std::memcpy(data_, header, kHeaderLength);
}
