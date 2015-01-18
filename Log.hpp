/*
 * Log System
 * Li Ao
 * hi@leeleo.me
 */

#ifndef __NETWORKSERVICE__LOG__
#define __NETWORKSERVICE__LOG__

#include "Common.hpp"

namespace NetworkService {

    class Log {
        public:
            static Log * Instance();

            void Write(const std::string & log_string) {};

        private:
            static Log * m_pInstance;
    };

}

#endif
