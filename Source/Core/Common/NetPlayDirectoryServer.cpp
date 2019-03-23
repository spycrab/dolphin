// Copyright 2019 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include <iostream>

#include "Common/ENetUtil.h"
#include "Common/NetPlayDirectoryProto.h"

int client()
{
    ENetHost* client = enet_host_create(NULL, 1, 0, 0, 0);
    ENetAddress addr;

    enet_address_set_host(&addr, DIRECTORY_DEFAULT_SERVER);
    addr.port = DIRECTORY_DEFAULT_PORT;

    ENetPeer* peer = enet_host_connect(client, &addr, 2, 0);

    if (!peer)
    {
        std::cerr << "Failed to create peer!" << std::endl;
        return 1;
    }


    ENetEvent event;
    
    if (enet_host_service(client, &event, 100) > 0)
    {
        std::cout << "Connected!" << std::endl;
        enet_host_flush(client);
    }

    enet_peer_disconnect(peer, 0);
    enet_host_service(client, &event, 100);
    
    return 0;
}

int server()
{
  ENetAddress server_addr;
  server_addr.host = ENET_HOST_ANY;
  server_addr.port = DIRECTORY_DEFAULT_PORT;

  ENetHost* server = enet_host_create(&server_addr, 10, 3, 0, 0);

  if (server == nullptr)
  {
      std::cerr << "Failed to create server!" << std::endl;
      return 0;
  }

  std::cout << "Listening on port " << server_addr.port << "..." << std::endl;

  while (true) {
      ENetEvent event;
      int net = enet_host_service(server, &event, 1000);

      if (net < 0)
          std::cerr << "Error occured: " << net << std::endl;
      
      if (net > 0)
      {
          std::cout << "Data" << std::endl;
          switch (event.type) {
          case ENET_EVENT_TYPE_CONNECT:
              std::cout << "Connected" << std::endl;
              break;
          case ENET_EVENT_TYPE_RECEIVE:
              std::cout << "Received" << std::endl;
              break;
          case ENET_EVENT_TYPE_DISCONNECT:
              std::cout << "Disconnected" << std::endl;
              break;
          }
      }
  }
  
  return 0;
}

int main(int argc, char** argv)
{
  if (enet_initialize() != 0)
  {
    std::cerr << "enet: Initialization failed" << std::endl;
    return 1;
  }

  if (argc != 2)
  {
      std::cout << "Not enough arguments!" << std::endl;
      return 1;
  }

  if (argv[1][0] == 'c')
      return client();
  else
      return server();
      
}
