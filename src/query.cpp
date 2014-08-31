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

#include "query.hpp"
#include "ndns-label.hpp"
namespace ndn {
namespace ndns {

Query::Query ()
  : m_queryType (QUERY_DNS)
  , m_interestLifetime (time::milliseconds (4000))
  , m_rrType (RR_NS)
{
}

Query::~Query ()
{
}

bool
Query::fromInterest (const Name &name, const Interest &interest)
{
  return fromInterest (interest);
}

bool
Query::fromInterest (const Interest& interest)
{
  const Name& interestName = interest.getName ();

  std::map<std::string, std::string> map;
  if (ndn::ndns::label::matchQueryName(interestName.toUri(), map)) {
    this->fromInterest(interest, map);
    return true;
  }
  else {
    std::cerr << "The name does not match the patter of NDNS Query: "
              << interestName.toUri() <<std::endl;
    return false;
  }
  return false;
}

bool
Query::fromInterest (const Interest& interest, std::map<std::string, std::string>& map)
{
  this->m_authorityZone = Name(map["zone"]);
  this->m_queryType = toQueryType(map["queryType"]);
  this->m_rrLabel = Name(map["rrLabel"]);
  this->m_rrType = toRRType(map["rrType"]);
  if (map["hint"] != "") {
    this->m_forwardingHint = Name(map["hint"]);
  }

  this->m_interestLifetime = interest.getInterestLifetime ();

  return true;
}

Interest
Query::toInterest () const
{
  Name name;
  if (m_forwardingHint.empty()) {
    //std::cout<<"do not add forwarding---------------------------:"<<std::endl;
  }
  else {
    name = m_forwardingHint;
    name.append (ndn::ndns::label::ForwardingHintLabel);

  }

  name.append (this->m_authorityZone);
  name.append (toString (this->m_queryType));
  name.append (this->m_rrLabel);
  name.append (ndn::ndns::toString (this->m_rrType));
  Selectors selector;

  Interest interest = Interest (name, selector, -1, this->m_interestLifetime);
  return interest;
}

} // namespace ndns
} // namespace ndn
