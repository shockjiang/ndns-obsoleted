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

#ifndef NDNS_APP_NAME_CACHING_RESOLVER_HPP
#define NDNS_APP_NAME_CACHING_RESOLVER_HPP

//#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/name.hpp>
#include <ndn-cxx/security/key-chain.hpp>

#include "zone.hpp"
#include "db/zone-mgr.hpp"

#include "query.hpp"
#include "response.hpp"
#include "rr.hpp"
#include "iterative-query.hpp"
#include "iterative-query-with-forwarding-hint.hpp"
#include "ndn-app.hpp"

using namespace std;

namespace ndn {
namespace ndns {

template<class IQStrategy>
//IQ stands for iterative Query Strategy
class NameCachingResolver : public NDNApp
{

public:

  NameCachingResolver (const char *programName, const char *prefix);

  void
  resolve (IQStrategy& iq);

  using NDNApp::onData;
  void
  onData (const Interest& interest, Data &data, IQStrategy& iq);

  void
  onInterest (const Name &name, const Interest &interest, const bool keepResolver);

  inline void
  onInterest (const Name &name, const Interest &interest)
  {
    onInterest(name, interest, true);
  }



  using NDNApp::onTimeout;
  void
  onTimeout (const Interest& interest, IQStrategy& iq);

  void
  run ();

  const Name&
  getRootZoneFowardingHint () const
  {
    return m_rootZoneFowardingHint;
  }

  void
  setRootZoneFowardingHint (const Name& rootZoneFowardingHint)
  {
    m_rootZoneFowardingHint = rootZoneFowardingHint;
  }

private:
  /**
   * 0 means disable forwarding hint,
   * 1 means enable forwarding hint
   * todo: 2, retrieve name and hint of the name server as a optimization way
   */

  Name m_rootZoneFowardingHint;

};

template<typename IQStrategy>
  NameCachingResolver<IQStrategy>::NameCachingResolver (const char *programName, const char *prefix)
    : NDNApp (programName, prefix)
  {
    this->setInterestLifetime (time::milliseconds (2000));
  }

template<class IQStrategy>
  void
  NameCachingResolver<IQStrategy>::run ()
  {
    boost::asio::signal_set signalSet (m_ioService, SIGINT, SIGTERM);
    signalSet.async_wait (boost::bind (&NDNApp::signalHandler, this));
    // boost::bind(&NdnTlvPingServer::signalHandler, this)

    Name name (m_prefix);
    name.append (toString (QUERY_DNS_R));

    m_face.setInterestFilter (name, bind (&NameCachingResolver::onInterest, this, _1, _2),
                              bind (&NDNApp::onRegisterFailed, this, _1, _2));

    std::cout << "\n=== NDNS Resolver " << m_programName << " with routeble prefix "
              << name.toUri () << " starts";

    if (this->m_enableForwardingHint > 0) {
      std::cout << " & Root Zone ForwardingHint " << this->m_rootZoneFowardingHint.toUri ();
    }
    std::cout << "===" << std::endl;

    try {
      m_face.processEvents ();
    }
    catch (std::exception& e) {
      std::cerr << "ERROR: " << e.what () << std::endl;
      m_hasError = true;
      m_ioService.stop ();
    }
  }
template<class IQStrategy>
  void
  NameCachingResolver<IQStrategy>::onData (const Interest& interest, Data &data, IQStrategy& iq)
  {

    iq.doData (data);

    if (iq.getStep () == QUERY_STEP_AnswerStub) {
      Data data = iq.getLastResponse ().toData ();
      Name name = iq.getQuery ().getAuthorityZone ();
      name.append (toString (iq.getQuery ().getQueryType ()));
      name.append (iq.getQuery ().getRrLabel ());
      name.append (toString (iq.getQuery ().getRrType ()));
      name.appendVersion ();
      data.setName (name);
      data.setFreshnessPeriod (iq.getLastResponse ().getFreshness ());

      m_keyChain.sign (data);
      m_face.put (data);
      std::cout << "[* <- *] answer Response ("
        << toString (iq.getLastResponse ().getResponseType ()) << ") to stub:" << std::endl;
      std::cout << iq.getLastResponse () << std::endl;
      for (int i = 0; i < 15; i++) {
        std::cout << "----";
      }
      std::cout << std::endl << std::endl;

      //iq.setStep(FinishedSuccessfully);

    }
    else if (iq.getStep () == QUERY_STEP_NSQuery) {
      resolve (iq);
    }
    else if (iq.getStep () == QUERY_STEP_RRQuery) {
      resolve (iq);
    }
    else if (iq.getStep () == QUERY_STEP_Abort) {
      return;
    }
    else if (iq.getStep () == QUERY_STEP_FHQuery) {
      resolve (iq);
    }
    else {
      std::cout << "let me see the current step=" << toString (iq.getStep ()) << std::endl;
      std::cout << iq;
      std::cout << std::endl;
    }

  }

template<class IQStrategy>
  void
  NameCachingResolver<IQStrategy>::onInterest (const Name &name, const Interest &interest, bool keepResolver)
  {
    Query query;
    query.fromInterest (interest);
    //std::cout << "[* -> *] receive Interest: " << interest.getName ().toUri () << std::endl;
    if (query.getQueryType () == QUERY_DNS) {
      cout << m_programName << " is not in charge of Query_DNS" << endl;
    }
    //std::cout << "LINE=" <<__LINE__<<std::endl;
    IQStrategy iq (query);

    //std::cout << "LINE=" <<__LINE__<<std::endl;
    if (this->m_enableForwardingHint == 0) {
      //std::cout<<"--------------------Disable Forwarding Hint"<<std::endl;
    }
    else if (this->m_enableForwardingHint > 0) {
      //std::cout<<"--------------------create a Forwarding Hint"<<std::endl;
      IterativeQueryWithForwardingHint& iqfh = dynamic_cast<IterativeQueryWithForwardingHint&> (iq);
      iqfh.setForwardingHint (this->m_rootZoneFowardingHint);
      iqfh.setLastForwardingHint (this->m_rootZoneFowardingHint);
    }
    if (keepResolver) {
      resolve (iq);
    }
    //resolve (iq);
    //std::cout << "LINE=" <<__LINE__<<std::endl;
  }

template<class IQStrategy>
  void
  NameCachingResolver<IQStrategy>::onTimeout (const Interest& interest, IQStrategy& iq)
  {
    std::cout << "[* !! *] timeout Interest " << interest.getName ().toUri () << " timeouts"
              << std::endl;

    iq.doTimeout ();
  }

template<class IQStrategy>
  void
  NameCachingResolver<IQStrategy>::resolve (IQStrategy& iq)
  {

    Interest interest = iq.toLatestInterest ();

    //must be set before express interest,since the call will back call iq
    //if set after, then the iq in call of onData will return nothing
    //be very careful here,  as a new guy to c++

    interest.setInterestLifetime (this->getInterestLifetime ());
    iq.setLastInterest (interest);

    std::cout << "lastInterest=" << iq.getLastInterest () << std::endl;

    try {
      m_face.expressInterest (interest,
                              boost::bind (&NameCachingResolver::onData, this, _1, _2, iq),
                              boost::bind (&NameCachingResolver::onTimeout, this, _1, iq));
      //boost::bind(&NameCachingResolver::onData, this, _1, _2, ndn::ref(iq)),
      //boost::bind(&NameCachingResolver::onTimeout, this, _1, iq));
    }
    catch (std::exception& e) {
      std::cerr << "ERROR: " << e.what () << std::endl;
    }

    std::cout << "[* <- *] send Interest: " << interest.getName ().toUri () << std::endl;
    std::cout << iq << std::endl;
  }
} /* namespace ndns */
} /* namespace ndn */

#endif
