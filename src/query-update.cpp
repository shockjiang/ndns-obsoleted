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

namespace ndn {
namespace ndns {

QueryUpdate::QueryUpdate ()
{
}

QueryUpdate::~QueryUpdate ()
{
}

Interest
QueryUpdate::toInterest (KeyChain& keyChain)
{

  Data data = m_update.toData ();
  keyChain.sign (data);
  m_wiredUpdate = data.wireEncode();


  /*
  Block block = data.wireEncode ();

  Name rrLabel = Name(block);

  this->m_rrLabel=rrLabel;




  std::string str = this->m_rrLabel.toUri();
  std::cout << "the str=" << str << std::endl;
  Name name(str);

  Block b2 = name.wireEncode();

  Block b3 = rrLabel.wireEncode();
  b3.resetWire();
  Response re2;

  if (rrLabel == name) {
    std::cout << rrLabel.at(0).toUri() << " = " << name.at(0).toUri()<<std::endl;
    std::cout << name.hasWire() << " = " << rrLabel.hasWire();
  }
  else {
    std::cout<<b2.size()<< " != "<< b3.size() <<std::endl;

  }



  Data d2;
  d2.wireDecode(b3);
  std::cout<<"=======";

  re2.fromData(d2);
  std::cout << "re2=" << re2 << std::endl;
  */


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
  name.append (m_wiredUpdate);
  name.append (ndn::ndns::toString (this->m_rrType));
  Selectors selector;

  Interest interest = Interest (name, selector, -1, this->m_interestLifetime);
  return interest;
}

bool
QueryUpdate::fromInterest (const Name& name, const Interest& interest)
{
  return this->fromInterest(interest);
}

bool
QueryUpdate::fromInterest (const Interest& interest)
{
  Name interestName;
  interestName = interest.getName ();

  std::map<std::string, std::string> map;
  if (ndn::ndns::label::matchUpdateName(interestName.toUri(), map)) {
    this->m_authorityZone = Name(map["zone"]);
    this->m_queryType = toQueryType(map["queryType"]);
    this->m_rrLabel = Name(map["rrLabel"]);

    this->m_rrType = toRRType(map["rrType"]);
    if (map["hint"] != "") {
      this->m_forwardingHint = Name(map["hint"]);
    }


    ssize_t temp = 0;
    // hint cannot be "/"
    if (map["hint"] == "") {
      temp += 0;
    }
    else if (map["hint"] == "/") {
      std::cout << "ERROR: forwording hint is /" << std::cout;
      temp += 1;
    }
    else {
      temp += this->m_forwardingHint.size() + 1;
    }

    if (map["zone"] == "/") {
      temp += 0;
    }
    else {
      temp += this->m_authorityZone.size();
    }

    temp += 1;
    std::cout << " wiredUpdate at index=" << temp << std::endl;
    this->m_wiredUpdate = interestName.get(temp).blockFromValue();

    this->m_interestLifetime = interest.getInterestLifetime ();
  }
  else {
    std::cerr << "The name does not match the patter of NDNS Update: "
              << interestName.toUri() <<std::endl;
  }



  std::cout << "to init data" << std::endl;
  Data data (m_wiredUpdate);
  std::cout << data << std::endl;
  m_update.fromData (data);

  return true;
}

} // namespace ndns
} // namespace ndn