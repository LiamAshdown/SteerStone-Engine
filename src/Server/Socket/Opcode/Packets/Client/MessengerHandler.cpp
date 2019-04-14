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
#include "Habbo.h"

#include "Opcode/Packets/Server/MessengerPackets.h"

namespace SteerStone
{
    void HabboSocket::HandleMessengerInitialize(std::unique_ptr<ClientPacket> p_Packet)
    {
        m_Habbo->SendInitializeMessenger();
    }

    void HabboSocket::HandleMessengerUpdate(std::unique_ptr<ClientPacket> p_Packet)
    {
        m_Habbo->SendMessengerUpdate();
    }

    void HabboSocket::HandleMessengerAcceptBuddy(std::unique_ptr<ClientPacket> p_Packet)
    {
        uint32 l_SenderId = p_Packet->ReadWiredUint();

        m_Habbo->MessengerAcceptRequest(l_SenderId);
    }

    void HabboSocket::HandleMessengerFindUser(std::unique_ptr<ClientPacket> p_Packet)
    {
        std::string l_Name = p_Packet->ReadString();

        m_Habbo->SendSearchUserResults(l_Name);
    }

    void HabboSocket::HandleMessengerSendRequest(std::unique_ptr<ClientPacket> p_Packet)
    {
        std::string l_Name = p_Packet->ReadString();

        m_Habbo->MessengerBuddyRequest(l_Name);
    }

    void HabboSocket::HandleMessengerRemoveBuddy(std::unique_ptr<ClientPacket> p_Packet)
    {
        m_Habbo->MessengerRemoveBuddy(std::move(p_Packet));
    }

    void HabboSocket::HandleMessengerRejectBuddy(std::unique_ptr<ClientPacket> p_Packet)
    {
        m_Habbo->MessengerRejectRequest(std::move(p_Packet));
    }

    void HabboSocket::HandleMessengerSendMessage(std::unique_ptr<ClientPacket> p_Packet)
    {
        m_Habbo->MessengerSendMessage(std::move(p_Packet));
    }

    void HabboSocket::HandleMessengerMarkRead(std::unique_ptr<ClientPacket> p_Packet)
    {
        uint32 l_MessageId = p_Packet->ReadWiredUint();

        m_Habbo->MessengerReply(l_MessageId);
    }

    void HabboSocket::HandleConsoleMotto(std::unique_ptr<ClientPacket> p_Packet)
    {
        /// TODO; Filter motto
        std::string l_Motto = p_Packet->ReadString();

        /// Set our new motto
        m_Habbo->m_ConsoleMotto = l_Motto;

        /// Update client with our new motto
        HabboPacket::Messenger::PersistentMessage l_Packet;
        l_Packet.Motto = l_Motto;
        SendPacket(l_Packet.Write());

        /// Time to update the database
        /// I will just call the default SaveToDb which will save all player details
        m_Habbo->SaveToDB();
    }
} /// NAMESPACE STEERSTONE
