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

#ifndef NDNS_SERVICE_QUERY_MGR_HPP
#define NDNS_SERVICE_QUERY_MGR_HPP

#include "db/db-mgr.hpp"

#include "rr.hpp"
#include "rrset.hpp"
#include "zone.hpp"
#include "query.hpp"
#include "response.hpp"
#include "db/zone-mgr.hpp"
#include "db/rrset-mgr.hpp"

namespace ndn {
namespace ndns {

class QueryMgr : public DBMgr
{
public:
  QueryMgr (Zone& zone, Query& query, Response& response);
  virtual
  ~QueryMgr ();

  void
  lookup ();
  int
  callback_getRr (int argc, char **argv, char **azColName);

  static int
  static_callback_getRr (void *param, int argc, char **argv, char **azColName)
  {
    QueryMgr *mgr = reinterpret_cast<QueryMgr*> (param);
    return mgr->callback_getRr (argc, argv, azColName);
  }

  /*@brief
   * whether we should reply NDNS_Auth instead of NDNS_Nack
   */
  int
  hasAuth ();

  inline void
  addRR (RR& rr)
  {
    m_rrs.push_back (rr);
  }

private:
  Zone & m_zone;
  Query& m_query;
  Response& m_response;

  RRSet m_rrset;
  RRSetMgr m_rrsetMgr;

  std::vector<RR> m_rrs;
};

} /* namespace ndns */
} /* namespace ndn */

#endif
