// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003-2004 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#ifndef SERVER_COMM_PSQL_SOCKET_H
#define SERVER_COMM_PSQL_SOCKET_H

#include "common/CommSocket.h"

#include <boost/asio.hpp>

class Database;

/// \brief Handle polling the socket used to communicate with the PostgreSQL
/// RDBMS.
/// \ingroup ServerSockets
class CommPSQLSocket {
  protected:

    boost::asio::io_service& m_io_service;
    boost::asio::ip::tcp::socket* m_socket;
    boost::asio::deadline_timer m_vacuumTimer;
    boost::asio::deadline_timer m_reindexTimer;
    boost::asio::deadline_timer m_reconnectTimer;


    /// Reference to the low level database management object.
    Database & m_db;

    /// Flag indicating whether the next vacuum job should be vacuum full.
    bool m_vacuumFull;

    void do_read();
    int read();
    void dispatch();

    void vacuum();
    void reindex();

    void tryReConnect();
  public:
    /// Interval between database vacuum jobs.
    static const int vacFreq = 25 * 60;
    /// Interval between database reindex jobs.
    static const int reindexFreq = 30 * 60;

    CommPSQLSocket(boost::asio::io_service& io_service, Database & db);
    virtual ~CommPSQLSocket();
};

#endif // SERVER_COMM_PSQL_SOCKET_H
