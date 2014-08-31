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

#ifndef NDNS_QUERY_UPDATE_HPP
#define NDNS_QUERY_UPDATE_HPP

#include "query.hpp"
#include "response.hpp"
#include "ndn-cxx/security/key-chain.hpp"
#include <sstream>

namespace ndn {
namespace ndns {

/*
 *
 */
class QueryUpdate : public Query
{
public:
  QueryUpdate ();
  virtual
  ~QueryUpdate ();

  using Query::toInterest;
  virtual Interest
  toInterest (KeyChain& keyChain);

  using Query::fromInterest;
  bool
  fromInterest (const Name &name, const Interest& interest);

  bool
  fromInterest (const Interest& interest);

  bool
  fromInterest (const Interest& interest, std::map<std::string, std::string>& map);

  void
  setUpdateRrs(const std::vector<RR>& rrs)
  {
    m_update.setRrs(rrs);
  }

  const Response&
  getUpdate () const
  {
    return m_update;
  }

  void
  setUpdate (const Response& update)
  {
    m_update = update;
  }

  void
  setUpdateZone(const Zone& zone)
  {
    m_update.setRrZones(zone);
  }


private:
  Response m_update;
  Block m_wiredUpdate;
  //KeyChain m_keyChain;
};

inline std::ostream&
operator<< (std::ostream& os, const QueryUpdate& update)
{
  os << "QueryUpdate: authorityZone=(" << update.getAuthorityZone ().toUri () <<")"
     << " queryType=" << toString (update.getQueryType ())
     << " rrType=" << toString (update.getRrType ())
     << " Update=$" << update.getUpdate() << "$";

  return os;
}

} // namespace ndns
} // namespace ndn

#endif // QUERY_UPDATE_HPP
