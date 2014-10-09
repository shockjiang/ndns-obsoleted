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

#include "dyndns-mgr.hpp"

namespace ndn {
namespace ndns {

DyndnsMgr::DyndnsMgr(Zone& zone, Response& re, QueryUpdate& queryUpdate)
  : m_zone(zone),
    m_response(re),
    m_queryUpdate(queryUpdate)
{
}

DyndnsMgr::~DyndnsMgr()
{
}

void
DyndnsMgr::update() throw ()
{
  std::vector<RR> rrs = m_queryUpdate.getUpdate().getRrs();

  RR& rr = rrs[0];
  rr.setZone(m_zone);
  std::string sql;
  RRSet rrset = rr.getRrset();
  RRSetMgr rrsetMgr;
  rrsetMgr.addRRSet(rrset);
  rrsetMgr.lookupIds();
  rr.setRrset(rrset);
  RRMgr rrMgr;
  rrMgr.addRR(rr);
  rrMgr.lookupIds();

  if (rr.getUpdateAction() == UPDATE_ACTION_NONE) {
    std::cout << "None action" << std::endl;
  }
  else if (rr.getUpdateAction() == UPDATE_ACTION_ADD) {
    if (rr.getId() != 0) {
      ; // already there, no more ADD
    }
    else {
      if (rrset.getId() == 0) {
        rrsetMgr.insert();
        rr.setRrset(rrset);
      }

      rrMgr.insert();
    }
  }
  else if (rr.getUpdateAction() == UPDATE_ACTION_REMOVE) {
    if (rr.getId() > 0) {
      std::cout << " to remove rr: " << rr << std::endl;
      rrMgr.remove();
    }
    else {
      std::cout << "rr.getId() == 0. do not have to remove" << std::endl;
      ;
    }
  }
  else if (rr.getUpdateAction() == UPDATE_ACTION_REPLACE_ALL) {
    if (rrset.getId() > 0) {
      std::cout << "remove RR related to " << rrset << std::endl;
      rrsetMgr.removeRelatedRRs();
    }
    else {
      rrsetMgr.insert();
      rr.setRrset(rrset);
    }

    rrMgr.insert();
  }
  else if (rr.getUpdateAction() == UPDATE_ACTION_REPLACE) {
    throw std::runtime_error("Not support " + toString(rr.getUpdateAction()));
  }
  else if (rr.getUpdateAction() == UPDATE_ACTION_UNKNOWN) {
    throw std::runtime_error("Not support " + toString(rr.getUpdateAction()));
  }

  std::cout << rr << std::endl;
  m_queryUpdate.setUpdateRrs(rrs);

}

int
DyndnsMgr::callback_update(int argc, char **argv, char **azColName)
{

  return 0;
}

} /* namespace ndns */
} /* namespace ndn */
