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

#include "response.hpp"
#include "rr.hpp"
#include "query.hpp"
#include "rr.hpp"

#include "boost-test.hpp"
#include <string>

namespace ndn {
namespace ndns {
namespace tests {

using namespace std;

BOOST_AUTO_TEST_SUITE(Response)

BOOST_AUTO_TEST_CASE(Encode)
{

  printbegin ("Response:Encode");



  ndns::Response re, re2;
  cout << "construct a Response Instance" << endl;
  vector<RR> vec;

  Zone zone ("/");
  Zone zone2 ("/net");

  RRSet rrset1 (zone);
  rrset1.setLabel(zone2.getAuthorizedName());
  RR rr (rrset1);

  std::string ex3 = "www3.ex.net";
  uint32_t id = 203;
  rr.setRrData (ex3);
  rr.setId (id);
  vec.push_back (rr);



  RRSet rrset2 (zone);
  rrset2.setLabel(Name("/net")); //RR in response must share the same RRLabel

  RR rr2 (rrset2);
  std::string ex4 = "www4.ex.com";
  id = 204;
  rr2.setRrData (ex4);
  rr2.setId (id);
  vec.push_back (rr2);

  ndns::Query q;
  Name n1 ("/");
  Name n2 ("/net");
  q.setAuthorityZone (n1);
  q.setRrLabel (n2);
  q.setQueryType (QUERY_DNS_R);
  Interest interest = q.toInterest ();
  Name n3 (q.toInterest ().getName ());
  n3.appendVersion();

  re.setQueryName (n3);
  re.setFreshness (time::milliseconds (4444));
  re.setRrs (vec);

  cout << "before encode" << endl;
  //ndn::Block block = re.wireEncode();
  //re.wireEncode();
  Data data = re.toData ();
  cout << re.toData () << endl;

  cout << "Encode finishes" << endl;

  re2.fromData (n2, data);
  BOOST_CHECK_EQUAL(re.getQueryName (), re2.getQueryName ());
  BOOST_CHECK_EQUAL(re.getQueryType (), re2.getQueryType ());
  BOOST_CHECK_EQUAL(re.getResponseType (), re2.getResponseType ());
  BOOST_CHECK_EQUAL(re.getRrs ().size (), re2.getRrs ().size ());
  BOOST_CHECK_EQUAL(re.getStringRRs(), re2.getStringRRs());

  printend ("response:Encode");
}

BOOST_AUTO_TEST_SUITE_END()

}// namespace tests
}// namespace ndns
} // namespace ndn
