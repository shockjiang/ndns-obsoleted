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

#ifndef NDNS_SERVICE_ZONE_REGISTER_HPP
#define NDNS_SERVICE_ZONE_REGISTER_HPP

#include "rrset.hpp"
#include "rr.hpp"
#include "zone.hpp"

#include "db/rrset-mgr.hpp"
#include "db/zone-mgr.hpp"
#include "db/rr-mgr.hpp"

#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/util/io.hpp>

namespace ndn {
namespace ndns {

class ZoneRegister
{
public:
  ZoneRegister (Zone& zone, Zone& subzone, boost::shared_ptr<IdentityCertificate>& cert);

  ZoneRegister (Zone& zone, Zone& subzone, IdentityCertificate& cert);

  virtual
  ~ZoneRegister ();

  bool
  tryRegister ();

  bool
  doRegisterRoot ();

  bool
  doRegisterZone ();

  const Zone&
  getZone () const
  {
    return m_zone;
  }

  void
  setZone (const Zone& zone)
  {
    m_zone = zone;
  }

  const Zone&
  getSubZone () const
  {
    return m_subzone;
  }

  const std::vector<RR>&
  getRrs () const
  {
    return m_rrs;
  }

  void
  setRrs (const std::vector<RR>& rrs)
  {
    m_rrs = rrs;
  }

  const std::vector<std::string>&
  getNsDatas () const
  {
    return m_nsDatas;
  }

  void
  setNsDatas (const std::vector<std::string>& nsDatas)
  {
    m_nsDatas = nsDatas;
  }

  void
  addNsData (const std::string& nsdata)
  {
    m_nsDatas.push_back (nsdata);
  }

private:
  Zone& m_zone;
  Zone& m_subzone;

  std::vector<RR> m_rrs;

  /*
   * when create NS records, the RRData of the rr
   * does not work when register root zone
   */
  std::vector<std::string> m_nsDatas;

  /*
   * when root zone, the certificate is the roots DSK certificate signed by its KSK;
   *
   * otherwise, the certificate is the subzone's KSK signed by parent zone's DSK
   */

  IdentityCertificate& m_subZoneKSKSignedByDSKCert;

  KeyChain m_keyChain;

};

inline std::ostream&
operator<< (std::ostream& os, const ZoneRegister& zg)
{
  os << "Zone=[" << zg.getZone () << "] ";
  os << "SubZone=[" << zg.getSubZone () << "] ";
  os << "[";
  std::vector<RR>::const_iterator iter = zg.getRrs ().begin ();
  while (iter != zg.getRrs ().end ()) {
    os << " " << *iter;
    iter++;
  }
  os << "]";
  return os;
}

} /* namespace ndns */
} /* namespace ndn */

#endif
