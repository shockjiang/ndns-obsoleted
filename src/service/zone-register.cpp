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

#include "zone-register.hpp"

namespace ndn {
namespace ndns {

ndn::ndns::ZoneRegister::ZoneRegister (Zone& zone, Zone& subzone,
  boost::shared_ptr<IdentityCertificate>& cert)
  : m_zone (zone)
  , m_subzone (subzone)
  , m_rrType (RR_NS)
  , m_subZoneKSKSignedByDSKCert (*cert)
{
  //std::cout<<m_subZoneKSKSignedByDSKCert<<std::endl;
}

ndn::ndns::ZoneRegister::ZoneRegister (Zone& zone, Zone& subzone, IdentityCertificate& cert)
  : m_zone (zone)
  , m_subzone (subzone)
  , m_rrType (RR_NS)
  , m_subZoneKSKSignedByDSKCert (cert)
{
  //std::cout<<m_subZoneKSKSignedByDSKCert<<std::endl;
}

ZoneRegister::~ZoneRegister ()
{
}

bool
ZoneRegister::tryRegister ()
{
  if (m_zone.getAuthorizedName () == m_subzone.getAuthorizedName ()) {
    return doRegisterTop ();
  }
  else {
    return doRegisterZone ();
  }
}

bool
ZoneRegister::doRegisterTop ()
{
  if (m_rrType != RR_NS) {
    std::cout <<"Register Top: RRType ust be RR_NS" << std::endl;
    return  false;
  }

  ZoneMgr zoneMgr (m_zone);
  zoneMgr.setResultNum (0);
  zoneMgr.lookupId ();
  if (zoneMgr.getResultNum () > 0) {
    std::cout << "The Zone: " << m_zone << " already exist in the database" << std::endl;
    return false;
  }

  std::cout << "to insert zone: " << m_zone << std::endl;
  zoneMgr.insert ();
  std::cout << "after insert zone: " << m_zone << std::endl;

  RRSet rrset (m_zone);
  Name name = m_subZoneKSKSignedByDSKCert.getName (); //.getSubName(m_zone.getAuthorizedName().size() + 1);
  name = name.getSubName(m_zone.getAuthorizedName().size(), name.size() - m_zone.getAuthorizedName().size());

  rrset.setLabel (name.toUri ());
  rrset.setClass ("NULL");
  rrset.setType (RR_ID_CERT);
  rrset.setNdndata ("NULL");

  RRSetMgr rrsetMgr;
  rrsetMgr.addRRSet (rrset);

  std::cout << "to insert rrset: " << rrset << std::endl;
  rrsetMgr.insert ();
  std::cout << "after insert rrset: " << rrset << std::endl;

  std::ostringstream os;
  ndn::io::save (m_subZoneKSKSignedByDSKCert, os);

  RR rr (rrset);
  rr.setRrData (os.str ());
  RRMgr rrMgr;
  rrMgr.addRR (rr);

  rrMgr.insert ();
  m_rrs.push_back (rr);

  return true;
}

bool
ZoneRegister::doRegisterZone ()
{
  ZoneMgr zoneMgr (m_zone);
  zoneMgr.setResultNum (0);
  zoneMgr.lookupId ();
  if (zoneMgr.getResultNum () == 0) {
    std::cout << "The zone: " << m_zone << " does not exist in the database" << std::endl;
    return false;
  }

  RRSetMgr rrsetMgr;
  RRMgr rrMgr;

  RRSet rrset (m_zone);
  Name name = m_subZoneKSKSignedByDSKCert.getName (); //.getSubName(m_zone.getAuthorizedName().size() + 1);
  name = name.getSubName(m_zone.getAuthorizedName().size(), name.size() - m_zone.getAuthorizedName().size());

  rrset.setLabel (name.toUri ());
  rrset.setClass ("NULL");
  rrset.setType (RR_ID_CERT);
  rrset.setNdndata ("NULL");
  rrsetMgr.addRRSet (rrset);

  RRSet rrset2 (m_zone);
  name = m_subzone.getAuthorizedName ().getSubName (m_zone.getAuthorizedName ().size ());
  rrset2.setLabel (name.toUri ());
  rrset2.setClass ("NULL");
  // could be NS, TXT
  rrset2.setType (m_rrType);

  rrset2.setNdndata ("NULL");

  rrsetMgr.addRRSet (rrset2);
  rrsetMgr.insert ();
  std::cout << "after insert: " << rrset << std::endl;
  std::cout << "after insert: " << rrset2 << std::endl;

  /*
   * work only after inserting rrset first
   */

  std::ostringstream os;
  ndn::io::save (m_subZoneKSKSignedByDSKCert, os);
  RR rr (rrset);
  rr.setRrData (os.str ());
  rrMgr.addRR (rr);

  std::vector<std::string>::const_iterator iter = m_nsDatas.begin ();
  RR* temp;
  while (iter != m_nsDatas.end ()) {
    temp = new RR (rrset2);
    temp->setRrData ((std::string) *iter);
    rrMgr.addRR (temp);
    iter++;
  }

  rrMgr.insert ();

  std::vector<RR*>::const_iterator iter2 = rrMgr.getRrs ().begin ();
  while (iter2 != rrMgr.getRrs ().end ()) {
    RR& temp = **iter2;
    m_rrs.push_back (temp);
    std::cout << "add RR: " << temp;
    std::cout << std::endl;
    iter2++;
  }

  return true;
}

} /* namespace ndns */
} /* namespace ndn */
