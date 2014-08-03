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

#ifndef NDNS_ITERATIVE_QUERY_WITH_FORWARDING_HINT_HPP
#define NDNS_ITERATIVE_QUERY_WITH_FORWARDING_HINT_HPP

#include "iterative-query.hpp"

namespace ndn {
namespace ndns {

class IterativeQueryWithForwardingHint : public IterativeQuery
{

public:
  IterativeQueryWithForwardingHint (const Query& query);
  virtual
  ~IterativeQueryWithForwardingHint ();

  const Interest
  toLatestInterest ();

  void
  doData (Data& data);

  bool
  doTimeout ();

  void
  abort ();

  unsigned int
  getForwardingHintIndex () const
  {
    return m_forwardingHintIndex;
  }

  void
  setForwardingHintIndex (unsigned int forwardingHintIndex)
  {
    m_forwardingHintIndex = forwardingHintIndex;
  }

  const Name&
  getForwardingHint () const
  {
    return m_forwardingHint;
  }

  void
  setForwardingHint (const Name& forwardingHint)
  {
    m_forwardingHint = forwardingHint;
  }

  const Name&
  getLastForwardingHint () const
  {
    return m_lastForwardingHint;
  }

  void
  setLastForwardingHint (const Name& lastForwardingHint)
  {
    m_lastForwardingHint = lastForwardingHint;
  }

private:

  Name m_forwardingHint;

  /*
   * work with m_lastFinishedLabelNum
   */
  Name m_lastForwardingHint;
  unsigned int m_forwardingHintIndex;

};

inline std::ostream&
operator<< (std::ostream& os, const IterativeQueryWithForwardingHint iq)
{
  os << "IterativeQueryWithFowardingHint: dstLabel=" << iq.getQuery ().getRrLabel ().toUri ()
     << " currentStep=" << toString (iq.getStep ()) << " finishedLabel="
     << iq.getFinishedLabelNum () << " FHint=" << iq.getForwardingHint ().toUri () << " rrLabelen="
     << iq.getRrLabelLen () << " NextAuZoneIndex=" << iq.getAuthZoneIndex ()

     << " [OriginalQuery: "
     << iq.getQuery () << "]" << " [LastReponse: " << iq.getLastResponse () << "]"
     << " [LastInterest: " << iq.getLastInterest () << "]";

  return os;
}

} /* namespace ndns */
} /* namespace ndn */

#endif
