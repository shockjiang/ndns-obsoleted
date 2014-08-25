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

#include "query-update.hpp"
#include "ndn-cxx/security/key-chain.hpp"
#include "boost-test.hpp"

namespace ndn {
namespace ndns {
namespace tests {

using namespace std;

BOOST_AUTO_TEST_SUITE(QueryUpdate)

BOOST_AUTO_TEST_CASE(UnderstandInterest)
{

  string label = "queryUpdate::UnderstandInterst";
  printbegin (label);

  KeyChain keyChain;

  ndns::QueryUpdate q;
  q.setFowardingHint (Name ("/ucla"));
  Name n1 ("/");
  Name n2 ("/www.baidu.com");
  q.setAuthorityZone (n1);
  q.setRrLabel (n2);
  q.setQueryType (QUERY_DNS);
  q.setRrType(ndn::ndns::RR_DYNDNS_UPDATE);




  Zone zone("/net");
  zone.setId(2);
  RRType rrType = RR_FH;
  Name rrLabel = Name("/skype");
  std::string rrData = "/att";

  Name name = zone.getAuthorizedName ();
  name.append (toString (QUERY_DNS));
  name.append (rrLabel);
  name.append (toString (rrType));
  name.appendVersion ();

  Response update;
  update.setQueryName (name);
  update.setQueryType (QUERY_DNS);

  update.setResponseType (RESPONSE_NDNS_Resp);

  RR rr;

  rr.setRrData (rrData);
  rr.setId (0);
  rr.setUpdateAction(UPDATE_ACTION_ADD);

  RRSet rrset;
  rrset.setType(rrType);
  rrset.setLabel(rrLabel);
  rrset.setZone(zone);
  std::cout << rrset << std::endl;
  rr.setRrset(rrset);
  std::cout << rr << std::endl;



  update.addRr (rr);
  q.setUpdate(update);

  std::cout << rr << std::endl;
  std::cout << q.getUpdate().getStringRRs()<< std::endl;

  Interest interest = q.toInterest (keyChain);
  Name name0 = interest.getName();

  cout << "InterestName=" << interest.getName () << endl;

  ndns::QueryUpdate q2;
  q2.fromInterest (interest);

  cout << "AuthZone=" << q2.getAuthorityZone () << " RRLable=" << q2.getRrLabel () << endl;
  BOOST_CHECK_EQUAL(q.getAuthorityZone (), q2.getAuthorityZone ());
  BOOST_CHECK_EQUAL(q.getUpdate().getStringRRs(), q2.getUpdate().getStringRRs());
  BOOST_CHECK_EQUAL(q.getQueryType (), q2.getQueryType ());
  BOOST_CHECK_EQUAL(q.getFowardingHint (), q2.getFowardingHint ());

  printend (label);
}

BOOST_AUTO_TEST_SUITE_END()

}// namespace tests
}// namespace ndns
} // namespace ndn
