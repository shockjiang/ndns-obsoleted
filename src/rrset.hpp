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

#ifndef NDNS_RRSET_HPP
#define NDNS_RRSET_HPP

#include "zone.hpp"
#include "ndns-enum.hpp"

namespace ndn {
namespace ndns {

class RRSet
{
public:
  RRSet (Zone & zone);
  RRSet ();

  virtual
  ~RRSet ();

  void
  setZone (const Name& zoneName)
  {
    m_zone.setAuthorizedName(zoneName);
  }

  const std::string&
  getClass () const
  {
    return m_class;
  }

  void
  setClass (const std::string& clazz)
  {
    m_class = clazz;
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

  const std::string&
  getNdndata () const
  {
    return m_ndndata;
  }

  void
  setNdndata (const std::string& ndndata)
  {
    m_ndndata = ndndata;
  }

  RRType
  getType () const
  {
    return m_type;
  }

  void
  setType (RRType type)
  {
    m_type = type;
  }

  const Zone&
  getZone () const
  {
    return m_zone;
  }

  /*
  RRSet&
  operator= (const RRSet& rrset)
  {
    return *this;
  }
  */

  const Name&
  getLabel () const
  {
    return m_label;
  }

  void
  setLabel (const Name& label)
  {
    m_label = label;
  }

  void
  setZone (const Zone& zone)
  {
    m_zone = zone;
  }

private:
  uint32_t m_id;
  Zone m_zone;
  Name m_label;
  std::string m_class;
  RRType m_type;
  std::string m_ndndata;

};

inline std::ostream&
operator<< (std::ostream& os, const RRSet& rrset)
{
  os << "RRSet: Id=" << rrset.getId () << " Zone="
     << rrset.getZone ().getAuthorizedName ().toUri ();
  os << " Label=" << rrset.getLabel ().toUri() << " Type=" << toString (rrset.getType ());
  os << " Class=" << rrset.getClass ();
  return os;
}

} /* namespace ndns */
} /* namespace ndn */

#endif
