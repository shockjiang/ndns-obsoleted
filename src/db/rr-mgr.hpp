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

#ifndef NDNS_DB_RR_MGR_HPP
#define NDNS_DB_RR_MGR_HPP

#include "db-mgr.hpp"
#include "rr.hpp"
#include "zone.hpp"
#include "rrset.hpp"
//#include <boost/smart_ptr/shared_ptr.hpp>
//#include <boost/ptr_container/ptr_container.hpp>

namespace ndn {
namespace ndns {

class RRMgr : public DBMgr
{
public:
  RRMgr ();
  virtual
  ~RRMgr ();

  void
  lookupIds();

  void
  insert ();


  void
  remove();

  void
  replace_all();

  inline void
  addRR (RR& rr)
  {
    //boost::shared_ptr<RR> rrP(&rr);
    m_rrs.push_back (&rr);
  }

  inline void
  addRR (RR* rr)
  {
    m_rrs.push_back (rr);
  }

  const std::vector<RR*>&
  getRrs () const
  {
    return m_rrs;
  }

private:
  std::vector<RR*> m_rrs;

};

} /* namespace ndns */
} /* namespace ndn */

#endif
