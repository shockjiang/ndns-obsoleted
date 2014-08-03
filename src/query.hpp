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

#ifndef NDNS_QUERY_HPP
#define NDNS_QUERY_HPP

#include "rr.hpp"
#include "ndns-label.hpp"

#include "ndns-enum.hpp"
#include <ndn-cxx/name.hpp>
#include <regex>
#include <map>

namespace ndn {
namespace ndns {

class Query
{
public:
  Query ();

  virtual
  ~Query ();

public:
  virtual Interest
  toInterest () const;

  virtual bool
  fromInterest (const Name &name, const Interest& interest);

  virtual bool
  fromInterest (const Interest& interest);

  ///////////////////////////////////////////////////////////

  const Name&
  getAuthorityZone () const
  {
    return m_authorityZone;
  }

  void
  setAuthorityZone (const Name& authorityZone)
  {
    m_authorityZone = authorityZone;
  }

  time::milliseconds
  getInterestLifetime () const
  {
    return m_interestLifetime;
  }

  void
  setInterestLifetime (time::milliseconds interestLifetime)
  {
    m_interestLifetime = interestLifetime;
  }

  enum QueryType
  getQueryType () const
  {
    return m_queryType;
  }

  void
  setQueryType (enum QueryType queryType)
  {
    m_queryType = queryType;
  }

  const Name&
  getRrLabel () const
  {
    return m_rrLabel;
  }

  void
  setRrLabel (const Name& rrLabel)
  {
    m_rrLabel = rrLabel;
  }

  const RRType&
  getRrType () const
  {
    return m_rrType;
  }

  void
  setRrType (const RRType& rrType)
  {
    m_rrType = rrType;
  }


  const Name&
  getFowardingHint () const
  {
    return m_forwardingHint;
  }

  void
  setFowardingHint (const Name& fowardingHint)
  {
    m_forwardingHint = fowardingHint;
  }


public:
  Name m_authorityZone;
  Name m_forwardingHint;

  enum QueryType m_queryType;
  time::milliseconds m_interestLifetime;
  Name m_rrLabel;
  enum RRType m_rrType;

  mutable Block m_wire;
};

inline std::ostream&
operator<< (std::ostream& os, const Query& query)
{
  os << "Query: authorityZone=" << query.getAuthorityZone ().toUri () << " queryType="
     << toString (query.getQueryType ()) << " rrLabel=" << query.getRrLabel ().toUri ()
     << " rrType=" << toString (query.getRrType ());
  return os;
}

} // namespace ndns
} // namespace ndn

#endif
