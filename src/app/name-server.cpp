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

#include "name-server.hpp"

namespace ndn {
namespace ndns {
NameServer::NameServer(const char *programName, const char *prefix, const char *nameZone,
  const string dbfile)
  : NDNApp(programName, prefix),
    m_zone(Name(nameZone)),
    m_zoneMgr(m_zone)
{
  m_zoneMgr.setDbfile(dbfile);
  //m_zoneMgr.lookupId();
} //NameServer Construction

void
NameServer::onInterest(const Name &name, const Interest &interest)
{

  cout << "[* -> *] receive Interest: " << toNameDigest(interest.getName()) << std::endl;

  std::string nameStr = interest.getName().toUri();

  std::map<std::string, std::string> map;
  //if (std::regex_match(nameStr, results, ndn::ndns::label::UPDATE_NAME_REGEX)) {
  if (ndn::ndns::label::matchUpdateName(nameStr, map)) {
    //Here is Update Message

    QueryUpdate queryUpdate;
    queryUpdate.fromInterest(interest); //, map);
    ResponseUpdate re;
    queryUpdate.setUpdateZone(m_zone);

    DyndnsMgr mgr(m_zone, re, queryUpdate);

    re.setQueryName(interest.getName());
    re.setResponseType(RESPONSE_DyNDNS_OK);
    re.setFreshness(this->m_contentFreshness);

    std::string msg = "OK";
    try {
      mgr.update();
    }
    catch (std::runtime_error& e) {
      //basically, it handles the runtime_error shrowed by executeOnly (in db-mgr.cpp)
      std::cout << e.what() << std::endl;
      re.setResponseType(RESPONSE_DyNDNS_FAIL);
      msg = "Failed: ";
      msg += e.what();
    }
    catch (...) {
      re.setResponseType(RESPONSE_DyNDNS_FAIL);
      msg = "Failed: Unknown Internal Error";
      std::cout << msg << std::endl;
    }

//    const std::vector<RR>& rrs = queryUpdate.getUpdate().getRrs();
//    const RR& rr = rrs[0];
//    if (rr.getId() == 0) {
//      re.setResponseType(RESPONSE_DyNDNS_FAIL);
//      msg = "Failed: Unknown Reason";
//      std::cout << "update failed due to " << msg << std::endl;
//      //error when duplicated ADD/REMOVE operations
//    }
//
    re.setResult(msg);

    std::cout << re << std::endl;
    ;

    Data data = re.toData();
    data.setFreshnessPeriod(re.getFreshness());
    m_keyChain.sign(data);
    m_face.put(data);
    cout << "[* <- *] send response: " << re << endl;

    /*
     * check the update agains? NDNs security policy
     */

    /*
     * check the seq
     */

  }
  //else if (std::regex_match(nameStr, results, ndn::ndns::label::DNS_QUERY_NAME_REGEX)) {
  else if (ndn::ndns::label::matchQueryName(nameStr, map)) {
    //Here is Query Message

    Query query;
    query.fromInterest(interest);

    Response response;

    Name name2 = interest.getName();
    name2.appendVersion();
    response.setQueryName(name2);

    response.setFreshness(this->m_contentFreshness);

    QueryMgr mgr(m_zone, query, response);

    mgr.lookup();

    if (response.getRrs().size() > 0) {
      response.setResponseType(RESPONSE_NDNS_Resp);
    }
    else {

      if (query.getRrType() == RR_NS) {
        int count = mgr.hasAuth();
        if (count < 0) {
          cout << "[* !! *] lookup error, then exit: " << mgr.getErr() << endl;
          return;
        }
        else if (count > 0) {
          response.setResponseType(RESPONSE_NDNS_Auth);
        }
        else {
          response.setResponseType(RESPONSE_NDNS_Nack);
        }
      }
      else {
        response.setResponseType(RESPONSE_NDNS_Nack);
      }
    }

    Data data = response.toData();
    data.setFreshnessPeriod(response.getFreshness());

    m_keyChain.sign(data);
    m_face.put(data);
    cout << "[* <- *] send response: " << response << ": " << data << endl;
  }
  else {
    // illegal RR Type, Query Type are here
    cerr << "get Illegal Interest: " << interest << endl;
  }

} //onInterest

void
NameServer::run()
{
  m_zoneMgr.lookupId();
  if (m_zoneMgr.getZone().getId() == 0) {
    m_hasError = true;
    m_error = "cannot get Zone.id from database for name=" + m_zone.getAuthorizedName().toUri();
    stop();
  }

  boost::asio::signal_set signalSet(m_ioService, SIGINT, SIGTERM);
  signalSet.async_wait(boost::bind(&NDNApp::signalHandler, this));
  // boost::bind(&NdnTlvPingServer::signalHandler, this)
  Name name;
  name.set(m_prefix);
  name.append(toString(QUERY_DNS));

  std::cout << "========= NDNS Name Server for Zone "
            << m_zoneMgr.getZone().getAuthorizedName().toUri() << " Starts with Prefix "
            << m_prefix;
  if (m_enableForwardingHint > 0) {
    std::cout << " & ForwardingHint " << m_forwardingHint.toUri();
  }
  std::cout << "=============" << std::endl;

  m_face.setInterestFilter(name, bind(&NameServer::onInterest, this, _1, _2),
                           bind(&NDNApp::onRegisterFailed, this, _1, _2));
  std::cout << "Name Server Register Name Prefix: " << name << std::endl;

  if (m_enableForwardingHint > 0) {
    Name name2 = Name(m_forwardingHint);
    name2.append(ndn::ndns::label::ForwardingHintLabel);
    name2.append(name);
    m_face.setInterestFilter(name2, bind(&NameServer::onInterest, this, _1, _2),
                             bind(&NDNApp::onRegisterFailed, this, _1, _2));
    std::cout << "Name Server Register Name Prefix: " << name2 << std::endl;
  }

  try {
    m_face.processEvents();
  }
  catch (std::exception& e) {
    m_hasError = true;
    m_error = "ERROR: ";
    m_error += e.what();
    stop();
  }

} //run

} //namespace ndns
} /* namespace ndn */
