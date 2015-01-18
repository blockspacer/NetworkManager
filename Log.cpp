/*
 * Log System
 * Li Ao
 * hi@leeleo.me
 */

#include "Log.hpp"

namespace NetworkService {

    Log * Log::m_pInstance = NULL;

    Log * Log::Instance() {
        if (! m_pInstance)
            m_pInstance = new Log();
        return m_pInstance;
    }

}
