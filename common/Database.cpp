// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include "Database.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <libpq++/pgdatabase.h>

#include <Atlas/Message/Encoder.h>
#include <Atlas/Codecs/XML.h>

// It seems that GNU libstdc++ does not yet include sstream.h
// Included is a version written explicitly for gcc, which will be used
// if the header is not present.
#include <common/stringstream.h>
#include <common/debug.h>
#include <common/globals.h>

#include <varconf/Config.h>

static const bool debug_flag = false;

Database * Database::m_instance = NULL;

Database::Database() : account_db("account"),
                       world_db("world"),
                       mind_db("mind"),
                       server_db("server"),
                       m_connection(NULL)
{
    
}

bool Database::initConnection(bool createDatabase)
{
    std::stringstream conninfos;
    if (global_conf->findItem("cyphesis", "dbserver")) {
        conninfos << "host=" << std::string(global_conf->getItem("cyphesis", "dbserver")) << " ";
    }

    std::string dbname = "cyphesis";
    if (global_conf->findItem("cyphesis", "dbname")) {
        dbname = std::string(global_conf->getItem("cyphesis", "dbname"));
    }
    conninfos << "dbname=" << dbname << " ";

    std::string dbuser = "cyphesis";
    if (global_conf->findItem("cyphesis", "dbuser")) {
        dbuser = std::string(global_conf->getItem("cyphesis", "dbuser"));
    }
    conninfos << "user=" << dbuser << " ";

    const std::string cinfo = conninfos.str();
    char * conninfo = new char [cinfo.size() + 1]; // , sizeof(char));
    strcpy(conninfo, cinfo.c_str());

    if (createDatabase) {
        std::cerr << "Attempting unsupported database creation operation"
                  << std::endl << std::flush;
    }

    m_connection = new PgDatabase(conninfo);
    delete [] conninfo;

    if (m_connection->Status() != CONNECTION_OK) {
        std::cerr << "Database connection failed" << std::endl << std::flush;
        return false;
    } else {
        std::cerr << "Database connection okay" << std::endl << std::flush;
    }

    return true;
}

bool Database::initAccount(bool createTables)
{
    int status = 0;
    // status = m_connection->ExecCommandOk("SELECT * FROM account WHERE id = 'test' AND type = 'test' AND password = 'test';");
    status = m_connection->ExecTuplesOk("SELECT * FROM account WHERE id = 'admin';");
    
    if (!status) {
        debug(std::cout << "Account table does not exist"
                        << std::endl << std::flush;);
        reportError();
        if (createTables) {
            debug(std::cout << "Account table does not exist"
                            << std::endl << std::flush;);
            // status = m_connection->ExecCommandOk("CREATE TABLE account ( id varchar(80), type varchar(80), password varchar(80) };");
            status = m_connection->ExecCommandOk("CREATE TABLE account ( id varchar(80), contents text );");
            if (!status) {
                std::cerr << "Error creating account table in database"
                          << std::endl << std::flush;
                reportError();
                return false;
            }
        } else {
            std::cerr << "Account table does not exist in database"
                      << std::endl << std::flush;
            return false;
        }
    }
    return true;
}

bool Database::initWorld(bool createTables)
{
    int status = 0;
    status = m_connection->ExecTuplesOk("SELECT * FROM world WHERE id = 'test' AND contents = 'test';");
    
    if (!status) {
        debug(std::cout << "World table does not exist"
                        << std::endl << std::flush;);
        if (createTables) {
            status = m_connection->ExecCommandOk("CREATE TABLE world ( id varchar(80), contents text );");
            if (!status) {
                std::cerr << "Error creating world table in database"
                          << std::endl << std::flush;
                reportError();
                return false;
            }
        } else {
            std::cerr << "World table does not exist in database"
                      << std::endl << std::flush;
            return false;
        }
    }
    return true;
}

bool Database::initMind(bool createTables)
{
    int status = 0;
    status = m_connection->ExecTuplesOk("SELECT * FROM mind WHERE id = 'test' AND contents = 'test';");
    
    if (!status) {
        debug(std::cout << "Mind table does not exist"
                        << std::endl << std::flush;);
        if (createTables) {
            status = m_connection->ExecCommandOk("CREATE TABLE mind ( id varchar(80), contents text );");
            if (!status) {
                std::cerr << "Error creating mind table in database"
                          << std::endl << std::flush;
                reportError();
                return false;
            }
        } else {
            std::cerr << "Mind table does not exist in database"
                      << std::endl << std::flush;
            return false;
        }
    }
    return true;
}
bool Database::initServer(bool createTables)
{
    int status = 0;
    status = m_connection->ExecTuplesOk("SELECT * FROM server WHERE id = 'test' AND contents = 'test';");
    
    if (!status) {
        debug(std::cout << "Server table does not exist"
                        << std::endl << std::flush;);
        if (createTables) {
            status = m_connection->ExecCommandOk("CREATE TABLE server ( id varchar(80), contents text );");
            if (!status) {
                std::cerr << "Error creating server table in database"
                          << std::endl << std::flush;
                reportError();
                return false;
            }
        } else {
            std::cerr << "Server table does not exist in database"
                      << std::endl << std::flush;
            return false;
        }
    }
    return true;
}

void Database::shutdownConnection()
{
    delete m_connection;
}

Database * Database::instance()
{
    if (m_instance == NULL) {
        m_instance = new Database();
    }
    return m_instance;
}

bool Database::decodeObject(const std::string & data,
                            Atlas::Message::Object::MapType &o)
{
    std::stringstream str(data, std::ios::in);

    Atlas::Codecs::XML codec(str, &m_d);
    Atlas::Message::Encoder enc(&codec);

    // Clear the decoder
    m_d.get();

    codec.Poll();

    if (!m_d.check()) {
        cerr << "WARNING: Database entry does not appear to be decodable"
             << endl << flush;
        return false;
    }
    
    o = m_d.get();
    return true;
}

bool Database::getObject(const std::string & table, const std::string & key,
                         Atlas::Message::Object::MapType & o)
{
    std::string query = std::string("SELECT * FROM ") + table + " WHERE id = '" + key + "';";

    int status = m_connection->ExecTuplesOk(query.c_str());

    if (!status) {
        debug(std::cout << "Error accessing " << key << " in " << table
                        << " table" << endl << flush;);
        return false;
    }
    int results = m_connection->Tuples();
    if (results < 1) {
        debug(std::cout << "No entry for " << key << " in " << table
                        << " table" << std::endl << std::flush;);
        return false;
    }
    const char * data = m_connection->GetValue(0, 1);
    debug(cout << "Got record " << key << " from database, value " << data
               << std::endl << std::flush;);
    
    return decodeObject(data, o);
}

bool Database::putObject(const std::string & table,
                         const std::string & key,
                         const Atlas::Message::Object::MapType & o)
{
    std::stringstream str;

    Atlas::Codecs::XML codec(str, &m_d);
    Atlas::Message::Encoder enc(&codec);

    codec.StreamBegin();
    enc.StreamMessage(o);
    codec.StreamEnd();

    debug(cout << "Encoded to: " << str.str().c_str() << " "
               << str.str().size() << endl << flush;);
#if 1
    std::string query = std::string("INSERT INTO ") + table + " VALUES ('" + key + "', '" + str.str() + "');";
    int status = m_connection->ExecCommandOk(query.c_str());
    if (!status) {
        std::cerr << "Failed to insert item " << key << " into " << table
                  << " table" << std::endl << std::flush;
        return false;
    }
    return true;
#else
    const std::string & s = str.str();

    Dbt key, data;

    key.set_data((void*)keystr);
    key.set_size(strlen(keystr) + 1);

    data.set_data((void*)s.c_str());
    data.set_size(s.size() + 1);

    int err;
    if ((err = db.put(NULL, &key, &data, 0)) != 0) {
        debug(cout << "db.put.ERROR! " << err << endl << flush;);
        return false;
    }
    return true;
#endif
}

bool Database::delObject(const std::string & table, const std::string & key)
{
#if 0
    Dbt key, data;

    key.set_data((void*)keystr);
    key.set_size(strlen(keystr) + 1);

    int err;
    if ((err = db.del(NULL, &key, 0)) != 0) {
        debug(cout << "db.del.ERROR! " << err << endl << flush;);
        return false;
    }
    return true;
#endif
    return true;
}

void Database::reportError()
{
    std::cerr << "DATABASE ERROR: " << m_connection->ErrorMessage()
              << std::endl << std::flush;
}

#if 0
DatabaseIterator::DatabaseIterator(Db & db ) : m_db(db)
{
    db.cursor(NULL, &m_cursor, 0);
}

bool DatabaseIterator::get(Atlas::Message::Object::MapType & o)
{
    Dbt key, data;

    int res = m_cursor->get(&key, &data, DB_NEXT);
    if (res == DB_NOTFOUND) {
        debug(cout << "No entries remain in database" << endl << flush;);
        return false;
    } else if (res != 0) {
        cerr << "WARNING: Error accessing database" << endl << flush;
        return false;
    }
    return Database::instance()->decodeObject(data, o);
}

bool DatabaseIterator::del()
{
    int res = m_cursor->del(0);
    if (res == 0) {
        return true;
    }
    cerr << "WARNING: Error deleting from database" << endl << flush;
    return false;
}
#endif
