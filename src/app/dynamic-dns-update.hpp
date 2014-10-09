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

#ifndef NDNS_APP_DYNAMIC_DNS_UPDATE_HPP
#define NDNS_APP_DYNAMIC_DNS_UPDATE_HPP

#include "ndn-app.hpp"

#include "rr.hpp"
#include "query.hpp"
#include "response.hpp"
#include "zone.hpp"
#include "ndns-enum.hpp"
#include "query-update.hpp"
#include "response-update.hpp"

namespace ndn {
namespace ndns {

class DynamicDNSUpdate : public NDNApp
{
public:
  DynamicDNSUpdate(const char* programName, const char* prefix, Zone zone, Name rrlabel,
    RRType rrtype, std::string rrdata, UpdateAction action = ndn::ndns::UPDATE_ACTION_REPLACE_ALL);

  virtual
  ~DynamicDNSUpdate();

  /**
   * @brief construct a query (interest) which contains the update information
   */
  Interest
  toInterest();

  void
  onData(const ndn::Interest& interest, Data& data);

  void
  onTimeout(const ndn::Interest& interest);

  void
  run();

  void
  send(const Interest& interest);

  void
  send();

  const ResponseUpdate&
  getResponse() const
  {
    return m_response;
  }

  void
  setResponse(const ResponseUpdate& response)
  {
    m_response = response;
  }

  const std::string&
  getRrData() const
  {
    return m_rrData;
  }

  void
  setRrData(const std::string& rrData)
  {
    m_rrData = rrData;
  }

  const Name&
  getRrLabel() const
  {
    return m_rrLabel;
  }

  void
  setRrLabel(const Name& rrLabel)
  {
    m_rrLabel = rrLabel;
  }

  RRType
  getRrType() const
  {
    return m_rrType;
  }

  void
  setRrType(RRType rrType)
  {
    m_rrType = rrType;
  }

  const Response&
  getUpdate() const
  {
    return m_update;
  }

  void
  setUpdate(const Response& update)
  {
    m_update = update;
  }

  UpdateAction
  getAction() const
  {
    return m_action;
  }

  void
  setAction(UpdateAction action)
  {
    m_action = action;
  }

  const QueryUpdate&
  getQueryUpdate() const
  {
    return m_queryUpdate;
  }

  void
  setQueryUpdate(const QueryUpdate& queryUpdate)
  {
    m_queryUpdate = queryUpdate;
  }

  const Zone&
  getZone() const
  {
    return m_zone;
  }

  void
  setZone(const Zone& zone)
  {
    m_zone = zone;
  }

private:
  Zone m_zone;

  /**
   * the data encapsulated in the name(rrLabel postion)
   */
  Response m_update;

  QueryUpdate m_queryUpdate;

  /**
   *
   */
  ResponseUpdate m_response;

  Name m_rrLabel;
  RRType m_rrType;
  std::string m_rrData;
  UpdateAction m_action;
};

} /* namespace ndns */
} /* namespace ndn */

#endif
