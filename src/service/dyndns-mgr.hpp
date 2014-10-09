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

#ifndef NDNS_SERVICE_DYNDNS_MGR_HPP
#define NDNS_SERVICE_DYNDNS_MGR_HPP

#include <stdexcept>

#include "zone.hpp"
#include "query.hpp"
#include "response.hpp"
#include "rr.hpp"
#include "db/db-mgr.hpp"
#include "query-update.hpp"
#include "db/rrset-mgr.hpp"
#include "db/rr-mgr.hpp"

namespace ndn {
namespace ndns {

class DyndnsMgr : public DBMgr
{
public:
  DyndnsMgr(Zone& zone, Response& re, QueryUpdate& queryUpdate);

  virtual
  ~DyndnsMgr();

  void
  update() throw ();

  int
  callback_update(int argc, char **argv, char **azColName);

  static int
  static_callback_update(void *param, int argc, char ** argv, char **azColName)
  {

    return 0;
  }

  Zone& m_zone;
  Response& m_response;
  QueryUpdate& m_queryUpdate;

};

} /* namespace ndns */
} /* namespace ndn */

#endif
