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

#include "service/dyndns-mgr.hpp"
#include "../boost-test.hpp"
#include "app/dynamic-dns-update.hpp"

namespace ndn {
namespace ndns {
namespace tests {

using namespace std;

BOOST_AUTO_TEST_SUITE(DbDyndnsMgr)

BOOST_AUTO_TEST_CASE(update)
{
  KeyChain keyChain;
  string label = "DbDyndnsMgr::update";
  printbegin (label);

  Zone zone (Name ("/net"));

  zone.setId (1UL);

  //DynamicDNSUpdate update("update", "/com/skype", zone, Name("/shock"), RR::FH, "/t-mobile2", ndn::ndns::label::UpdateAdd);
  //DynamicDNSUpdate update("update", "/com/skype", zone, Name("/shock"), RR::FH, "/t-mobile", "/t-mobile2");
  DynamicDNSUpdate update ("update", "/net/skype", zone, Name ("/shock"), RR_FH, "/t-mobile2",
                           ndn::ndns::UPDATE_ACTION_ADD);

  Interest interest = update.toInterest ();

  QueryUpdate queryUpdate;

  queryUpdate.fromInterest (interest);

  Response re; //this is the update information
  DyndnsMgr mgr (zone, re, queryUpdate);

  mgr.update ();
  const std::vector<RR>& rrs = queryUpdate.getUpdate().getRrs();
  const RR& rr = rrs[0];
  std::cout << rr <<std::endl;
  BOOST_CHECK_GT(rr.getId(), 0);

  printend (label);
}

BOOST_AUTO_TEST_SUITE_END()

}
}
}
