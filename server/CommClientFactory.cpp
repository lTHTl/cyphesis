// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

// $Id$

#include "CommClientFactory_impl.h"

#include "CommHttpClient.h"
#include "CommPythonClient.h"
#include "TrustedConnection.h"
#include "SlaveClientConnection.h"
#include "Peer.h"

int CommHttpClientFactory::newCommClient(CommServer & svr,
                                         int asockfd,
                                         const std::string & address)
{
    CommHttpClient * newcli = new CommHttpClient(svr, asockfd);

    // Add this new client to the list.
    svr.addSocket(newcli);

    return 0;
}

int CommPythonClientFactory::newCommClient(CommServer & svr,
                                         int asockfd,
                                         const std::string & address)
{
    CommPythonClient * newcli = new CommPythonClient(svr, asockfd);

    // Add this new client to the list.
    svr.addSocket(newcli);

    return 0;
}

template class CommClientFactory<Connection>;
template class CommClientFactory<SlaveClientConnection>;
template class CommClientFactory<TrustedConnection>;
template class CommClientFactory<Peer>;
