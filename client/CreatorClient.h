// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef CREATOR_CLIENT_H
#define CREATOR_CLIENT_H

#include "CharacterClient.h"

class Entity;

class CreatorClient : public CharacterClient {
  private:
    Entity * sendLook(RootOperation & op);
  public:
    CreatorClient(const std::string&, const std::string&, ClientConnection&);

    Entity * make(const Element &);
    void sendSet(const std::string &, const Element &);
    Entity * look(const std::string &);
    Entity * lookFor(const Element &);
    bool runScript(const std::string & package, const std::string & function);
};

#endif // CREATOR_CLIENT_H
