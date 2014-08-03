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

#ifndef NDNS_RR_HPP
#define NDNS_RR_HPP

#include "ndns-tlv.hpp"
#include "ndns-label.hpp"
#include "ndns-enum.hpp"

#include <ndn-cxx/encoding/block.hpp>
#include <ndn-cxx/interest.hpp>

#include <ndn-cxx/encoding/encoding-buffer.hpp>

#include "rrset.hpp"

namespace ndn {
namespace ndns {

class RR
{
public:
  RR (RRSet& rrset);

  RR ();
  virtual
  ~RR ();

public:

  void
  setType (RRType type)
  {
    m_rrset.setType(type);
  }

  void
  setLabel (const Name& label)
  {
    m_rrset.setLabel(label);
  }

  void
  setZone(const Zone& zone)
  {
    m_rrset.setZone(zone);
  }
  void
  setZone (const Name& zone)
  {
    m_rrset.setZone(zone);
  }

  uint32_t
  getId () const
  {
    return m_id;
  }

  void
  setId (uint32_t id)
  {
    m_id = id;
  }

  const Block&
  getWire () const
  {
    return m_wire;
  }

  void
  setWire (const Block& wire)
  {
    m_wire = wire;
  }

  inline bool
  operator== (const RR& rr) const
  {
    if (this->getRrData () == rr.getRrData ())
      return true;

    return false;
  }

  template<bool T>
    inline size_t
    wireEncode (EncodingImpl<T> & block) const
    {
      size_t totalLength = 0;

      std::string msg = toString(m_updateAction);
      totalLength += prependByteArrayBlock (block, ndn::ndns::tlv::RRUpdateAction,
                                            reinterpret_cast<const uint8_t*> (msg.c_str ()),
                                            msg.size ());

       msg = this->getRrData ();
      totalLength += prependByteArrayBlock (block, ndn::ndns::tlv::RRDataRecord,
                                            reinterpret_cast<const uint8_t*> (msg.c_str ()),
                                            msg.size ());

      totalLength += prependNonNegativeIntegerBlock (block, ndn::ndns::tlv::RRDataSub1,
                                                     this->getId ());

      totalLength += block.prependVarNumber (totalLength);
      totalLength += block.prependVarNumber (ndn::ndns::tlv::RRData);
      //std::cout<<"call rr.h wireEncode"<<std::endl;
      return totalLength;
    }

  const Block&
  wireEncode () const;

  void
  wireDecode (const Block& wire);

  //////////////////////////////////////////////////////////////////

  uint32_t
  getTtl () const
  {
    return m_ttl;
  }

  void
  setTtl (uint32_t ttl)
  {
    m_ttl = ttl;
  }

  const std::string&
  getRrData () const
  {
    return m_rrData;
  }

  void
  setRrData (const std::string& rrData)
  {
    m_rrData = rrData;
  }

  const RRSet&
  getRrset () const
  {
    return m_rrset;
  }

/*
  RR&
  operator= (const RR& rr)
  {
    return *this;
  }
*/
  UpdateAction
  getUpdateAction () const
  {
    return m_updateAction;
  }

  void
  setUpdateAction (UpdateAction updateAction)
  {
    m_updateAction = updateAction;
  }

  void
  setRrset (const RRSet& rrset)
  {
    m_rrset = rrset;
    //std::cout << "this.rrset="<< m_rrset  <<std::endl;
    //std::cout << "othe.rrset="<< rrset  <<std::endl;
  }



private:
  uint32_t m_id;
  std::string m_rrData;
  uint32_t m_ttl;

  /**
   * the old the label when update. If Any, removes all and add this one.
   *
   */
  UpdateAction m_updateAction;

  RRSet m_rrset;
  mutable Block m_wire;
};
//class RR

inline std::ostream&
operator<< (std::ostream& os, const RR& rr)
{
  os << "RR: Id=" << rr.getId () << " Data=";
  int maxsize = 10;
  if (rr.getRrData().size() > maxsize) {
   os  << rr.getRrData ().substr (0, 10) + "...";
  }
  else {
    os << rr.getRrData();
  }
  os << " [RRSet=" << rr.getRrset () << "]";
  os << " UpdateAction=" << toString(rr.getUpdateAction());
  return os;
}

} // namespace ndns
} // namespace ndn

#endif
