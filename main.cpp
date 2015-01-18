//
//  main.cpp
//  NetworkService
//
//  Created by leo on 12/23/14.
//  Copyright (c) 2014 leo. All rights reserved.
//
#include "NetworkManager.hpp"

int main() {
    std::string str;
    std::cin >> str;
    if (str == "server") {
        NetworkService::NetworkManager::Instance()->RunServer();
    } else {
        std::string client_id(str);
        NetworkService::NetworkManager::Instance()->set_remote_address("127.0.0.1");
        NetworkService::NetworkManager::Instance()->RunClient(client_id); 

        NetworkService::Message::Pointer pointer = NetworkService::Message::Create();

        pointer->PushData("type",123);
        pointer->PushData("leo", "leo");

        NetworkService::NetworkManager::Instance()->TcpSendDataToServer(pointer);
        
        NetworkService::NetworkManager::Instance()->thread()->join();
    }
}
