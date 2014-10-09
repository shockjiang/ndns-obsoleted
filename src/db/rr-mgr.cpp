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

#include "rr-mgr.hpp"

namespace ndn {
namespace ndns {

RRMgr::RRMgr()
{

}

RRMgr::~RRMgr()
{
}

void
RRMgr::lookupIds()
{
  std::vector<RR*>::iterator iter = m_rrs.begin();
  std::string sql;

  this->open();
  while (iter != m_rrs.end()) {
    RR& rr = **iter;
    if (rr.getRrset().getZone().getId() == 0 || rr.getRrset().getId() == 0) {
      std::cout << " One Id is not initialized. Zone.id=" << rr.getRrset().getZone().getId()
                << " RRSet.id=" << rr.getRrset().getId() << std::endl;
    }
    else {
      sql = "SELECT id FROM rrs WHERE rrset_id=";
      sql += std::to_string(rr.getRrset().getId());
      sql += " AND rrdata=\'" + rr.getRrData() + "\'";
      this->setTempInt(0);
      this->executeOnly(sql, static_callback_getInt, this);
      rr.setId(this->m_tempInt);
    }
    iter++;
  }

  this->close();
}

void
RRMgr::remove()
{
  std::string sql;

  std::vector<RR*>::iterator iter = m_rrs.begin();

  this->open();
  while (iter != m_rrs.end()) {
    RR& rr = **iter;
    sql = "DELETE from rrs WHERE id=";
    sql += std::to_string(rr.getId());

    this->executeOnly(sql, this->static_callback_doNothing, this);
    iter++;
  }

  this->close();

}

void
RRMgr::insert()
{
  std::string sql;

  std::vector<RR*>::iterator iter = m_rrs.begin();

  this->open();
  while (iter != m_rrs.end()) {
    RR& rr = **iter;
    sql = "INSERT INTO rrs(rrset_id, ttl, rrdata) VALUES (";
    sql += std::to_string(rr.getRrset().getId()) + ", ";
    sql += std::to_string(rr.getTtl()) + ", ";
    sql += "\'" + rr.getRrData() + "\'";
    sql += ")";

    this->executeOnly(sql, this->static_callback_insert_id, this);

    rr.setId(sqlite3_last_insert_rowid(this->m_conn));

    iter++;
  }

  this->close();

}

} // namespace ndns
} // namespace ndn
