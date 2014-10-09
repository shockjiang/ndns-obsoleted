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

#ifndef NDNS_NDNS_LABEL_HPP
#define NDNS_NDNS_LABEL_HPP

#include <ndn-cxx/name.hpp>
#include <vector>
#include <map>
#include <regex>

namespace ndn {
namespace ndns {
namespace label {
const std::string ForwardingHintLabel = "\xF0.";
const ndn::Name::Component ForwardingHintComp(ForwardingHintLabel);
const ndn::Name::Component QueryDNSComp("DNS");
const ndn::Name::Component QueryDNSRComp("DNS-R");

const std::regex DNS_QUERY_NAME_REGEX("((/.*)%F0\\.)?(/.*)/?(DNS)(/.*)/(FH|TXT|NS|ID-CERT)(.*)");
const std::regex QUERY_NAME_REGEX("((/.*)%F0\\.)?(/.*)/?(DNS-R|DNS)(/.*)/(FH|TXT|NS|ID-CERT)(.*)");
const std::regex UPDATE_NAME_REGEX("((/.*)%F0\\.)?(/.*)/?(DNS)(/.*)/(DYNDNSUPDATE)(.*)");

inline bool
matchQueryName(const std::string& str, std::map<std::string, std::string>& map)
{
  std::match_results<std::string::const_iterator> results;

  if (std::regex_match(str, results, QUERY_NAME_REGEX)) {
    map["hint"] = results[2];
    map["zone"] = results[3];
    map["queryType"] = results[4];
    map["rrLabel"] = results[5];
    map["rrType"] = results[6];
    //std::cout << "Query Match Successfully" << std::endl;
//    std::cout <<"hint="<<results[2]<<", zone="<<results[3];
//    std::cout <<", queryType="<<results[4] <<", rrLabel="<<results[5];
//    std::cout <<", rrType="<<results[6];
//    std::cout<< std::endl;
    return true;
  }
  return false;
}

/**
 * @brief this function does not match the query's name which contains the response
 * instead, it match the inner update (response)'s name
 */
inline bool
matchUpdateName(const std::string& str, std::map<std::string, std::string>& map)
{
  std::match_results<std::string::const_iterator> results;

  if (std::regex_match(str, results, UPDATE_NAME_REGEX)) {
    map["hint"] = results[2];
    map["zone"] = results[3];
    map["queryType"] = results[4];
    map["rrLabel"] = results[5];
    map["rrType"] = results[6];

    //std::cout << "Update Match Successfully" << std::endl;
    //std::cout <<"hint="<<results[2]<<", zone="<<results[3] << std::endl;
    return true;
  }
  return false;
}

}    // namespace label
} // namespace ndns
} // namespace ndn
#endif
