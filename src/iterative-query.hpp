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

#ifndef NDNS_ITERATIVE_QUERY_HPP
#define NDNS_ITERATIVE_QUERY_HPP

#include <ndn-cxx/data.hpp>
#include <ndn-cxx/interest.hpp>
#include <ndn-cxx/name.hpp>
#include <sys/_types/_ssize_t.h>
#include <cstdbool>
#include <iostream>

#include <boost/algorithm/string/predicate.hpp>
//#include <boost/lexical_cast.hpp>
#include "ndns-enum.hpp"
#include "query.hpp"
#include "response.hpp"
//#include "rr.hpp"

namespace ndn {
namespace ndns {

class IterativeQuery
{
public:
  IterativeQuery (const Query&);
  virtual
  ~IterativeQuery ();

  virtual void
  doData (Data& data);

  virtual bool
  doTimeout ();

  void
  abort ();

  virtual const Interest
  toLatestInterest ();

  bool
  hasFinished ()
  {
    return m_query.getRrLabel ().size () == m_finishedLabelNum;
  }

  void
  addTryNum ()
  {
    m_tryNum += 1;
  }

  ssize_t
  getFinishedLabelNum () const
  {
    return m_finishedLabelNum;
  }

  void
  setFinishedLabelNum (ssize_t finishedLabelNum)
  {
    m_finishedLabelNum = finishedLabelNum;
  }

  const Query&
  getQuery () const
  {
    return m_query;
  }

  const Response&
  getLastResponse () const
  {
    return m_lastResponse;
  }

  void
  setLastResponse (const Response& response)
  {
    m_lastResponse = response;
  }

  unsigned short
  getTryMax () const
  {
    return m_tryMax;
  }

  void
  setTryMax (unsigned short tryMax)
  {
    m_tryMax = tryMax;
  }

  unsigned short
  getTryNum () const
  {
    return m_tryNum;
  }

  void
  setTryNum (unsigned short tryNum)
  {
    m_tryNum = tryNum;
  }

  const Interest&
  getLastInterest () const
  {
    return m_lastInterest;
  }

  void
  setLastInterest (const Interest& lastInterest)
  {
    m_lastInterest = lastInterest;

  }

  QuerySteps
  getStep () const
  {
    return m_step;
  }

  ssize_t
  getRrLabelLen () const
  {
    return m_rrLabelLen;
  }

  void
  setRrLabelLen (ssize_t rrLabelLen)
  {
    m_rrLabelLen = rrLabelLen;
  }

  unsigned int
  getAuthZoneIndex () const
  {
    return m_authZoneIndex;
  }

  void
  setRrAsNextAuthorityZoneToTry (unsigned int authZoneIndex)
  {
    m_authZoneIndex = authZoneIndex;
  }


protected:
  QuerySteps m_step;

  unsigned short m_tryNum;
  unsigned short m_tryMax;
  //Name m_dstLabel;

  /*
   * the original query, not the intermediate query
   */
  const Query m_query;
  /*
   * the number of label that has been resolved successfully
   * This also define the next AuthZone prefix
   * AuthZone = m_query.getRrLabel().getPrefix(m_finishedLabelNum)
   */
  ssize_t m_finishedLabelNum;

  /*
   * used when query the KSK (key signing key), e.g., /net/ndnsim/ksk-1
   */
  ssize_t m_lastFinishedLabelNum;

  /*
   * the number used to generated the label
   * query.getRrLabel().getSubName(m_finishedLabelNum, m_labelLength);
   */
  ssize_t m_rrLabelLen;

  /*
   * m_lastResponse is changed once a new expecting Data is understood:
   * response.from(data)
   */
  Response m_lastResponse;

  /*
   * next auth zone should be m_lastResponse.getRrs()[m_authZoneIndex]
   */
  unsigned int m_authZoneIndex;

  /*
   * last interest that has sent
   */
  Interest m_lastInterest;

  //friend class IterativeQueryWithForwardingHint;
};

inline std::ostream&
operator<< (std::ostream& os, const IterativeQuery iq)
{
  os << "InterativeQuery: dstLabel=" << iq.getQuery ().getRrLabel ().toUri () << " currentStep="
     << toString (iq.getStep ()) << " finishedLabel=" << iq.getFinishedLabelNum () << " rrLabelen="
     << iq.getRrLabelLen () << " NextAuZoneIndex=" << iq.getAuthZoneIndex () << " [OriginalQuery: "
     << iq.getQuery () << "]" << " [LastReponse: " << iq.getLastResponse () << "]"
     << " [LastInterest: " << iq.getLastInterest () << "]";

  return os;
}

} /* namespace ndns */
} /* namespace ndn */

#endif
