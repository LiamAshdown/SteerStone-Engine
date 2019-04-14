/*
* Liam Ashdown
* Copyright (C) 2019
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "HabboSocket.h"
#include "Hotel.h"
#include "RoomManager.h"
#include "Opcode/Opcodes.h"
#include "TriggerEventManager.h"

bool LoadDatabase();

int main()
{
    /// Initialize our logger
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::verbose, &consoleAppender);

    LOG_NONE << " ______     ______   ______     ______     ______     ______     ______   ______     __   __     ______ ";
    LOG_NONE << "/\\  ___\\   /\\__  _\\ /\\  ___\\   /\\  ___\\   /\\  == \\   /\\  ___\\   /\\__  _\\ /\\  __ \\   /\\ ' - .\\ \ / \\  ___\\     ";
    LOG_NONE << "\\ \\___  \\  \\/_/\\ \\/ \\ \\  __\\   \\ \\  __\\   \\ \\  __<   \\ \\___  \\  \\/_/\\ \\/ \\ \\ \\/\\ \\  \\ \\ \\-.  \\  \\ \\  __\\     ";
    LOG_NONE << " \\/\\_____\\    \\ \\_\\  \\ \\_____\\  \\ \\_____\\  \\ \\_\\ \\_\\  \\/\\_____\\    \\ \\_\\  \\ \\_____\\  \\ \\_\\\\'\\_\\  \\ \\_____\\   ";
    LOG_NONE << "  \\/_____/     \\/_/   \\/_____/   \\/_____/   \\/_/ /_/   \\/_____/     \\/_/   \\/_____/   \\/_/ \\/_/   \\/_____/    ";
    LOG_NONE << "                                                                    Powered by Boost & MySQL";


    if (!sConfig->SetFile("server.conf"))
    {
        LOG_ERROR << "Could not find server.conf";
        Sleep(5000);
        return 1;
    }

    if (!LoadDatabase())
    {
        Sleep(5000);
        return -1;
    }

    sHotel->LoadConfigs();
    sOpcode->InitializePackets();
    sItemMgr->LoadPublicRoomItems();
    sTriggerMgr->LoadTriggerEvents();
    sRoomMgr->LoadRoomModels();
    sRoomMgr->LoadRoomUrls();
    sRoomMgr->LoadRoomCategories();
    sRoomMgr->LoadRoomWalkWays();
    sRoomMgr->LoadRooms();

    SteerStone::Listener<SteerStone::HabboSocket> listener(sConfig->GetStringDefault("BindIP", "127.0.0.1"), sHotel->GetIntConfig(SteerStone::IntConfigs::CONFIG_SERVER_PORT),
        sHotel->GetIntConfig(SteerStone::IntConfigs::CONFIG_NETWORK_PROCESSORS));

    LOG_INFO << "Successfully booted up SteerStone! Listening on " << sConfig->GetStringDefault("BindIP", "127.0.0.1") << " " << sHotel->GetIntConfig(SteerStone::IntConfigs::CONFIG_SERVER_PORT);

    uint32 realCurrTime = 0;
    uint32 realPrevTime = sHotelTimer->Tick();
    uint32 prevSleepTime = 0;

    while (!SteerStone::Hotel::StopWorld())
    {
        realCurrTime = sHotelTimer->GetServerTime();

        uint32 const& diff = sHotelTimer->Tick();

        sHotel->UpdateWorld(diff);

        realPrevTime = realCurrTime;

        // Update the world every 500 ms
        if (diff <= UPDATE_WORLD_TIMER + prevSleepTime)
        {
            prevSleepTime = UPDATE_WORLD_TIMER + prevSleepTime - diff;
            std::this_thread::sleep_for(std::chrono::milliseconds((uint32)(prevSleepTime)));
        }
        else
            prevSleepTime = 0;
    }
   
    sHotel->CleanUp();

    return 0;
}

bool LoadDatabase()
{
    if (sDatabase->CreateDatabase(sConfig->GetStringDefault("UserDatabaseInfo").c_str(), sConfig->GetIntDefault("UserDatabaseInfo.WorkerThreads", 1)) &&
        sDatabase->CreateDatabase(sConfig->GetStringDefault("RoomDatabaseInfo").c_str(), sConfig->GetIntDefault("RoomDatabaseInfo.WorkerThreads", 1)))
        return true;
    else
        return false;
}