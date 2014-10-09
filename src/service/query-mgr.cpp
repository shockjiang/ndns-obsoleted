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

#include "query-mgr.hpp"

namespace ndn {
namespace ndns {

QueryMgr::QueryMgr(Zone& zone, Query& query, Response& response)
  : m_zone(zone),
    m_query(query),
    m_response(response),
    m_rrset(m_zone)
{
}

QueryMgr::~QueryMgr()
{
}

int
QueryMgr::hasAuth()
{
  std::string sql;
  sql = "SELECT count(*) FROM rrs INNER JOIN rrsets ON rrs.rrset_id=rrsets.id";
  sql += " WHERE rrsets.zone_id=";
  sql += std::to_string(m_zone.getId());
  sql += " AND ";
  sql += "rrsets.type=\'";
  sql += toString(m_query.getRrType());
  sql += "\' AND ";
  sql += "rrsets.label LIKE \'";
  sql += m_query.getRrLabel().toUri() + "/%\'";

  this->setTempInt(0);
  this->execute(sql, static_callback_getInt, this);

  return this->m_tempInt;
}

void
QueryMgr::lookup()
{
  m_rrset.setLabel(m_query.getRrLabel().toUri());
  m_rrset.setType(m_query.getRrType());

  m_rrsetMgr.addRRSet(m_rrset);
  m_rrsetMgr.lookupIds();

  std::string sql;

  sql = "SELECT id, ttl, rrdata FROM rrs WHERE rrset_id=";
  sql += std::to_string(m_rrset.getId());
  sql += " ORDER BY id";

  this->execute(sql, static_callback_getRr, this);

  m_response.setRrs(m_rrs);
}

int
QueryMgr::callback_getRr(int argc, char** argv, char** azColName)
{
  RR rr(m_rrset);
  rr.setId(std::atoi(argv[0]));
  rr.setTtl(std::atoi(argv[1]));
  rr.setRrData(argv[2]);

  this->addRR(rr);
  return 0;
}

} /* namespace ndns */
} /* namespace ndn */
