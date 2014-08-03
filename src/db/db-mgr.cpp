/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014, Regents of the University of California.
 *
 * This file is part of NDNS (Named Data Networking Domain Name Service).
 * See AUTHORS.md for complete list of NDNS authors and contributors.
 *
 * NDNS is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NDNS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NDNS, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <fstream>

#include "db-mgr.hpp"

namespace ndn {
namespace ndns {

DBMgr::DBMgr ()
  : m_dbfile ("src/db/ndns-local.db")
  , m_conn (0)
  , m_reCode (-1)
  , m_status (DBClosed)
  , m_resultNum (0)
  , m_tempInt (-1)
{
  std::fstream file;
  file.open (m_dbfile, std::ios::in);
  if (file) {
    //std::cout << "database file " << m_dbfile << " does exist" << std::endl;
  }
  else {
    std::cout << "database file " << m_dbfile << " does not exist" << std::endl;
  }

}

DBMgr::~DBMgr ()
{
}

void
DBMgr::open ()
{
  if (m_status == DBConnected)
    return;

  m_reCode = sqlite3_open (this->m_dbfile.c_str (), &(this->m_conn));
  if (m_reCode != SQLITE_OK) {
    m_err = "Cannot connect to the db: " + this->m_dbfile;
    m_status = DBError;
  }
  else {
    //std::cout << "connect to the db: " << m_dbfile << std::endl;
  }
  m_status = DBConnected;
}

void
DBMgr::close ()
{
  if (m_status == DBClosed)
    return;

  m_reCode = sqlite3_close (this->m_conn);
  if (m_reCode != SQLITE_OK) {
    m_err = "Cannot close the db: " + this->m_dbfile;
    m_status = DBError;
  }
  m_status = DBClosed;
}

void
DBMgr::execute (std::string sql, int
(*callback) (void*, int, char**, char**), void * paras)
{
  if (m_status == DBClosed)
    this->open ();

  this->executeOnly (sql, callback, paras);

  this->close ();
}

void
DBMgr::executeOnly (std::string sql, int
(*callback) (void*, int, char**, char**), void * paras)
{
  if (m_status == DBClosed) {
    std::cout << " the DB is closed " << std::endl;
    return;
  }

  this->printSQL (sql);
  clearResultNum ();
  char *err_msg;
  //std::cout<<"to execute"<<std::endl;
  m_reCode = sqlite3_exec (m_conn, sql.c_str (), callback, paras, &err_msg);

  //std::cout<<"finish execute. reCode="<<m_reCode<<" SQLITE_OK="<<SQLITE_OK<<std::endl;
  if (m_reCode != SQLITE_OK) {
    m_status = DBError;
    this->m_err.append (err_msg);
    std::cout << "DBError: " << this->m_err << std::endl;
  }
}

/*@brief execute a single sql command with opening and closing the db connection
 * only the last sql execute call the callback function
 */
void
DBMgr::execute (std::vector<std::string> sqls, int
(*callback) (void*, int, char**, char**), void * paras)
{
  if (m_status == DBClosed)
    this->open ();

  this->executeOnly (sqls, callback, paras);

  this->close ();
}

/*
 * @brief only the last sql execute call the callback function, the db
 * connection should be opened and closed mannually
 */
void
DBMgr::executeOnly (std::vector<std::string> sqls, int
(*callback) (void*, int, char**, char**), void * paras)
{
  char *err_msg;
  std::vector<std::string>::size_type num = sqls.size ();
  std::string sql;
  for (int i = 0; i < num - 2; i++) {
    sql = sqls[i];
    this->printSQL (sql);
    m_reCode = sqlite3_exec (m_conn, sql.c_str (), this->static_callback_doNothing, paras,
                             &err_msg);
    if (m_reCode != SQLITE_OK) {
      m_status = DBError;
      this->m_err.append (err_msg);
      std::cout << "DBError: " << this->m_err << std::endl;
      return;
    } //if
  } //for

  //the last sql
  sql = sqls[num - 1];
  this->printSQL (sql);

  m_reCode = sqlite3_exec (m_conn, sql.c_str (), callback, paras, &err_msg);
  if (m_reCode != SQLITE_OK) {
    m_status = DBError;
    this->m_err.append (err_msg);
    std::cout << "DBError: " << this->m_err << std::endl;
  }
}

int
DBMgr::callback_getInt (int argc, char **argv, char** azColNames)
{
  this->m_tempInt = std::atoi (argv[0]);
  return 0;
}
int
DBMgr::callback_getStr (int argc, char **argv, char** azColNames)
{
  this->m_tempStr = std::atoi (argv[0]);
  return 0;
}

int
DBMgr::callback_get_insert_id (int argc, char **argv, char** azColNames)
{
  this->m_tempInt = std::atoi (argv[0]);
  return 0;
}

} //namespace ndns
} //namespace ndn
