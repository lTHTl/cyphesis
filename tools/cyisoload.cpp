// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Message/Encoder.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codecs/XML.h>

#include <common/config.h>
#include <common/database.h>

#include <string>
#include <fstream>

#include <coal/isoloader.h>

#ifdef HAVE_LIBDB_CXX

class WorldBase : public Database {
  protected:
    WorldBase() { }

  public:
    static WorldBase * instance() {
        if (m_instance == NULL) {
            m_instance = new WorldBase();
        }
        return (WorldBase *)m_instance;
    }

    void storeInWorld(const Atlas::Message::Object & o, const char * key) {
        putObject(world_db, o, key);
    }
};

class TemplatesLoader : public Atlas::Message::DecoderBase {
    ifstream m_file;
    Atlas::Message::Object::MapType m_db;
    Atlas::Codecs::XML m_codec;
    int m_count;

    virtual void ObjectArrived(const Atlas::Message::Object& o) {
        Atlas::Message::Object obj(o);
        if (!obj.IsMap()) {
            cerr << "ERROR: Non map object in file" << endl << flush;
            return;
        }
        Atlas::Message::Object::MapType & omap = obj.AsMap();
        if (omap.find("graphic") == omap.end()) {
            cerr<<"WARNING: Template Object in file has no graphic. Not stored."
                << endl << flush;
            return;
        }
        m_count++;
        const string & id = omap["graphic"].AsString();
        m_db[id] = obj;
    }
  public:
    TemplatesLoader(const string & filename) :
                m_file(filename.c_str()),
                m_codec((iostream&)m_file, this), m_count(0) {
    }

    void read() {
        while (!m_file.eof()) {
            m_codec.Poll();
        }
    }

    void report() {
        std::cout << m_count << " template objects loaded."
                  << endl << flush;
    }

    const Atlas::Message::Object & get(const string & graphic) {
        return m_db[graphic];
    }
};

void usage(char * progname)
{
    cout << "usage: " << progname << "filename" << endl << flush;
    return;
}

int main(int argc, char ** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    WorldBase * db = WorldBase::instance();
    db->initWorld(true);

    TemplatesLoader f("templates.xml");
    f.read();
    f.report();

    CoalDatabase map_database;
    CoalIsoLoader loader (map_database);
    if (!loader.LoadMap(argv[1])) {
        cout << "Map load failed." << endl << flush;
    } else {
        int count = map_database.GetObjectCount();
        for(int i = 0; i < count; i++) {
            CoalObject * object = (CoalObject*)map_database.GetObject (i);
            if (object != NULL) {
                const string & graphic = object->graphic->filename;
                // Get basename, lookup custumise and load into database
                cout << graphic;
            }
        }
    }
    
    db->shutdownWorld();
    delete db;
}

#else // HAVE_LIBDB_CXX

int main(int argc, char ** argv)
{
    std::cerr << "This version of cyphesis was built without persistant world support" << endl << flush;
    exit(0);
}

#endif // HAVE_LIBDB_CXX
