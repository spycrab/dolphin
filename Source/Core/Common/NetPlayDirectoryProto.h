// Copyright 2019 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#pragma once

#include "Common/CommonTypes.h"

constexpr int DIRECTORY_DEFAULT_PORT = 31411;
constexpr char* DIRECTORY_DEFAULT_SERVER = "localhost";
constexpr int DIRECTORY_PROTOCOL_VERSION = 0;

enum class PacketID : u8 {
    Cli_Connect,
    Ser_Connect,
    Cli_List,
    Ser_List
};

enum class ResponseCode : u8 {
    Okay = 0, // No error
    Bad_Version = 1, // Incompatible version
};

namespace Client {
    struct Connect
    {
      u8 version;
    };

    struct RequestList {
    };
}

struct ServerEntry
{
    char name[256];
    u64 game_id;
    u8 player_count;
};

namespace Server
{
    struct Connect
    {
        ResponseCode code;
    };

    struct ListAnnounce {
        u32 length;
    };
}
