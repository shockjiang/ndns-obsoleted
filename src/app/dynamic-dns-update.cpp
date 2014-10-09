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

#include "dynamic-dns-update.hpp"

namespace ndn {
namespace ndns {

DynamicDNSUpdate::DynamicDNSUpdate(const char* programName, const char* prefix, Zone zone,
  Name rrlabel, RRType rrtype, std::string rrdata, UpdateAction action)
  : NDNApp(programName, prefix),
    m_zone(zone),
    m_rrLabel(rrlabel),
    m_rrType(rrtype),
    m_rrData(rrdata),
    m_action(action)
{
}

DynamicDNSUpdate::~DynamicDNSUpdate()
{
}

void
DynamicDNSUpdate::onData(const ndn::Interest& interest, Data& data)
{
  m_response.fromData(data);
  std::cout << "[* -> *] get response: " << m_response << std::endl;
  this->stop();
}

void
DynamicDNSUpdate::send(const Interest& interest)
{
  try {
    m_face.expressInterest(interest, boost::bind(&DynamicDNSUpdate::onData, this, _1, _2),
                           boost::bind(&DynamicDNSUpdate::onTimeout, this, _1));
    std::cout << "[* <- *] send Interest: " << toNameDigest(interest.getName()) << std::endl;
  }
  catch (std::exception& e) {
    m_hasError = true;
    m_error = e.what();
    std::cout << "Fail to send Interest: " << m_error << std::endl;
  }
  m_interestTriedNum += 1;
}

void
DynamicDNSUpdate::send()
{
  Interest interest = this->toInterest();
  interest.setInterestLifetime(this->m_interestLifetime);
  this->send(interest);
}

void
DynamicDNSUpdate::onTimeout(const Interest& interest)
{
  std::cout << "[* !! *] timeout Interest: " << toNameDigest(interest.getName()) << std::endl;

  if (m_interestTriedNum >= m_interestTriedMax) {
    m_error = "All Interests timeout";
    m_hasError = true;
    this->stop();
  }
  else {
    this->send(interest);
  }

}

void
DynamicDNSUpdate::run()
{
  this->send();

  try {
    m_face.processEvents();
  }
  catch (std::exception& e) {
    m_error = e.what();
    m_hasError = true;
    this->stop();
  }

}

Interest
DynamicDNSUpdate::toInterest()
{

  Name name = m_zone.getAuthorizedName();
  name.append(toString(QUERY_DNS));
  name.append(m_rrLabel);
  name.append(toString(m_rrType));
  name.appendVersion();

  m_update.setQueryName(name);
  m_update.setQueryType(QUERY_DNS);

  m_update.setResponseType(RESPONSE_NDNS_Resp);

  RR rr;
  rr.setRrData(m_rrData);
  rr.setId(0);
  rr.setUpdateAction(m_action);

  RRSet rrset;
  rrset.setType(m_rrType);
  rrset.setLabel(m_rrLabel);
  rrset.setZone(m_zone);
  rr.setRrset(rrset);

  m_update.addRr(rr);

  m_queryUpdate.setUpdate(m_update);

  m_queryUpdate.setInterestLifetime(this->m_interestLifetime);

  m_queryUpdate.setAuthorityZone(m_zone.getAuthorizedName());
  m_queryUpdate.setQueryType(QUERY_DNS);
  m_queryUpdate.setRrType(RR_DYNDNS_UPDATE);

  /*
   Data data = m_update.toData ();
   m_keyChain.sign (data);
   const Block& block = data.wireEncode();
   m_queryUpdate.setRrLabel(Name(block));
   return m_queryUpdate.toInterest();
   */
  return m_queryUpdate.toInterest(m_keyChain);

}

} // namespace ndns
} // namespace ndn
