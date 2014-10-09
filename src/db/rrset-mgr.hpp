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

#ifndef NDNS_DB_RRSET_MGR_HPP
#define NDNS_DB_RRSET_MGR_HPP

#include "db-mgr.hpp"
#include "rrset.hpp"
#include "rr.hpp"
//#include <boost/smart_ptr/shared_ptr.hpp>
//#include <boost/ptr_container/ptr_container.hpp>
namespace ndn {
namespace ndns {

class RRSetMgr : public DBMgr
{
public:
  RRSetMgr();
  virtual
  ~RRSetMgr();

  void
  addRRSet(RRSet& rrset)
  {

    m_rrsets.push_back(&rrset);
  }

  bool
  insert();

  void
  lookupIds();

  void
  removeRelatedRRs();

private:
  std::vector<RRSet*> m_rrsets;
};

} /* namespace ndns */
} /* namespace ndn */

#endif
