/*
 * Enumeration class
 * Li Ao
 * hi@leeleo.me
 */

#ifndef __NETWORKSERVICE__ENUM__
#define __NETWORKSERVICE__ENUM__

#include <string>

namespace NetworkService {

    enum Config {
        kMaxMessage = 1024, 
        kClientPort = 3535,
        kServerPort = 3536,
    };

    enum class MessageType: int {

        kClientData,
        kConfigFile,
        kClientOffline,
        kPing,
        kDisconnect,
        kUsernameFailed,
        kConnectionTimeOut,

        //
        //特殊的方块（炸弹啊什么的）
        //Request is from client to server
        //
        kCubeCreateRequest,
        //
        //Operation is from server to client
        //
        kCubeCreateOperation,

        //
        //Normal Cube 
        //普通的方块
        //
        kCubeData,

        kLogin,
        kSuccess,
    };

}


#endif
