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

#include "zone-mgr.hpp"

namespace ndn {
namespace ndns {

ZoneMgr::ZoneMgr(Zone& zone)
  : m_zone(zone)
{
  //this->lookupId();
}

void
ZoneMgr::lookupId(const Name& name)
{
  this->open();
  std::string sql = "SELECT id FROM zones WHERE name=\'" + name.toUri() + "\'";
  //sql = "SELECT * FROM ZONES";
  //std::cout << "sql=" << sql << std::endl;
  //std::cout<<"*this="<<this<<" m_zone.id="<<m_zone.getId()<<" zoneId="<<&m_zone<<std::endl;
  this->execute(sql, static_callback_setId, this);
  //std::cout<<"*this="<<this<<" m_zone.id="<<m_zone.getId()<<" zoneId="<<&m_zone<<std::endl;
  this->close();

}
int
ZoneMgr::callback_setId(int argc, char **argv, char **azColName)
{
  //Zone zone = this->getZone();
  this->addResultNum();

  Zone zone = this->m_zone;

  if (argc < 1) {
    this->setErr("No Zone with Name " + zone.getAuthorizedName().toUri());
    return -1;
  }
  //std::cout<<"id="<<(uint32_t)std::atoi(argv[0])<<" "<<std::endl;
  int t1 = std::atoi(argv[0]);

  m_zone.setId(t1);

  return 0;
}
void
ZoneMgr::lookupId()
{
  lookupId(this->m_zone.getAuthorizedName());
}

void
ZoneMgr::insert()
{
  this->open();
  std::string sql = "INSERT INTO zones (name) VALUES (\'";
  sql += m_zone.getAuthorizedName().toUri() + "\')";
  this->executeOnly(sql, this->static_callback_doNothing, this);
  m_zone.setId(sqlite3_last_insert_rowid(this->m_conn));
  //std::cout<<"zone info: "<<m_zone<<std::endl;
  this->close();
}

//
//int
//ZoneMgr::callback_insert (int argc, char** argv, char** azColName)
//{
//  std::cout << "insert a zone: " << m_zone << std::endl;
//  return 0;
//}

}//namepsace ndns
} //namespace ndn

