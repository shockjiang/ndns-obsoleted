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

#include "rrset-mgr.hpp"

namespace ndn {
namespace ndns {

RRSetMgr::RRSetMgr ()
{

}

RRSetMgr::~RRSetMgr ()
{
}

bool
RRSetMgr::insert ()
{
  std::vector<RRSet*>::iterator iter = m_rrsets.begin ();
  std::string sql;
  this->open ();

  while (iter != m_rrsets.end ()) {
    sql = "INSERT INTO rrsets (zone_id, label, class, type, ndndata) VALUES";

    RRSet& rrset = **iter;
    sql += " (";
    sql += std::to_string (rrset.getZone ().getId ()) + ", ";
    sql += "\'" + rrset.getLabel ().toUri() + "\', ";
    sql += "\'" + rrset.getClass () + "\', ";
    sql += "\'" + ndn::ndns::toString (rrset.getType ()) + "\', ";
    sql += "NULL";
    sql += ")";


    this->executeOnly (sql, static_callback_doNothing, this);
    rrset.setId(sqlite3_last_insert_rowid (this->m_conn));
    //std::cout<<"just insert: "<<rrset<<std::endl;

    iter++;
  }

  this->close ();

  return true;
}

void
RRSetMgr::lookupIds ()
{
  std::string sql;
  std::vector<RRSet*>::iterator iter = m_rrsets.begin ();
  this->open();
  while (iter != m_rrsets.end ()) {
    RRSet& rrset = **iter;
    sql = "SELECT id FROM rrsets WHERE zone_id=";
    sql += std::to_string (rrset.getZone ().getId ());
    sql += " AND label=\'" + rrset.getLabel ().toUri() + "\'";
    sql += " AND type=\'" + toString (rrset.getType ()) + "\'";

    this->setTempInt(0);
    this->executeOnly (sql, this->static_callback_getInt, this);
    rrset.setId (this->getTempInt ());

    iter++;
  }

  this->close();
}


void
RRSetMgr::removeRelatedRRs ()
{
  std::string sql;
  std::vector<RRSet*>::iterator iter = m_rrsets.begin ();
  this->open();
  while (iter != m_rrsets.end ()) {
    RRSet& rrset = **iter;
    sql = "DELETE FROM rrs where rrset_id=";
    sql += std::to_string (rrset.getId ());
    this->executeOnly(sql, this->static_callback_doNothing, this);
    iter++;
  }

  this->close();
}

} /* namespace ndns */
} /* namespace ndn */

