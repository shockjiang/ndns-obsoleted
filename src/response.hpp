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

#ifndef NDNS_RESPONSE_HPP
#define NDNS_RESPONSE_HPP

//#include <boost/asio.hpp>
#include <boost/bind.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/noncopyable.hpp>

#include <ndn-cxx/face.hpp> // /usr/local/include
#include <ndn-cxx/name.hpp>
#include <ndn-cxx/data.hpp>

#include <ndn-cxx/encoding/block-helpers.hpp>
#include <ndn-cxx/encoding/block.hpp>
<<<<<<< HEAD
=======
//#include <ndn-cxx/encoding/tlv-ndnd.hpp>
>>>>>>> 16f5f40... optimization

#include <vector>

#include "ndns-tlv.hpp"
#include "ndns-enum.hpp"
#include "ndns-label.hpp"
#include "rr.hpp"

namespace ndn {
namespace ndns {

class Response
{
public:

  Response ();
  virtual
  ~Response ();


  inline void
  addRr (RR rr)
  {
    this->m_rrs.push_back (rr);
  }

  const Block&
  wireEncode () const;

  void
  wireDecode (const Block& wire);

  template<bool T>
    size_t
    wireEncode (EncodingImpl<T> & block) const;

  void
  fromData (const Name& name, const Data& data);

  void
  fromData (const Data &data);

  Data
  toData () const;

  const std::string
  getStringRRs () const
  {
    std::stringstream str;
    str << "[";
    std::vector<RR>::const_iterator iter = m_rrs.begin ();
    while (iter != m_rrs.end ()) {
      str << " " << *iter;
      iter++;
    }
    str << "]";
    return str.str ();
  }

  QueryType
  getQueryType () const
  {
    return m_queryType;
  }

  void
  setQueryType (QueryType queryType)
  {
    m_queryType = queryType;
  }

  time::milliseconds
  getFreshness () const
  {
    return m_freshness;
  }

  void
  setFreshness (time::milliseconds freshness)
  {
    m_freshness = freshness;
  }

  const Name&
  getQueryName () const
  {
    return m_queryName;
  }

  void
  setQueryName (const Name& queryName)
  {
    m_queryName = queryName;
  }

  ResponseType
  getResponseType () const
  {
    return m_responseType;
  }

  void
  setResponseType (ResponseType responseType)
  {
    m_responseType = responseType;
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

  void
  setRrZones(const Zone& zone)
  {
    std::vector<RR>::iterator iter = m_rrs.begin();
    while (iter != m_rrs.end()) {
      RR& rr = *iter;
      rr.setZone(zone);
    }
  }
private:
  time::milliseconds m_freshness;
  /*
   * this is the whole name of the response.
   */
  Name m_queryName;

  QueryType m_queryType;
  Name m_authorityZone;
  Name m_forwardingHint;
  Name m_rrLabel;
  enum RRType m_rrType;

  ResponseType m_responseType;
  std::vector<RR> m_rrs;
  mutable Block m_wire;

};

inline std::ostream&
operator<< (std::ostream& os, const Response& response)
{
  os << "Response: queryName=" << response.getQueryName ().toUri () << " responseType="
     << toString (response.getResponseType ()) << " queryType="
     << toString (response.getQueryType ()) << " [";

  std::vector<RR>::const_iterator iter = response.getRrs ().begin ();
  while (iter != response.getRrs ().end ()) {
    os << " " << *iter;
    iter++;
  }

  os << "]";

  return os;
}

} // namespace ndns
} // namespace ndn

#endif
