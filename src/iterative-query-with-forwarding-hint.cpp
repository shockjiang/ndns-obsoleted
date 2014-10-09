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

#include "iterative-query-with-forwarding-hint.hpp"

namespace ndn {
namespace ndns {

IterativeQueryWithForwardingHint::IterativeQueryWithForwardingHint(const Query& query)
  : IterativeQuery(query),
    m_forwardingHintIndex(0)
{

}

IterativeQueryWithForwardingHint::~IterativeQueryWithForwardingHint()
{
}

void
IterativeQueryWithForwardingHint::abort()
{
  std::cout << "QUERY_STEP_Abort the Resolving" << std::endl;
  std::cout << (*this);
  std::cout << std::endl;
}

bool
IterativeQueryWithForwardingHint::doTimeout()
{
  abort();
  return false;
}

void
IterativeQueryWithForwardingHint::doData(Data& data)
{
  std::cout << "[* -> *] resolve Data: " << data.getName().toUri() << std::endl;
  Response re;
  re.fromData(data);
  std::cout << re << std::endl;
  //std::cout<<"step 1"<<std::endl;
  if (re.getResponseType() == RESPONSE_UNKNOWN) {
    std::cout << "[* !! *] unknown content type and exit";
    m_step = QUERY_STEP_Abort;
    abort();
    return;
  }
  else if (re.getResponseType() == RESPONSE_NDNS_Nack) {
    if (m_step == QUERY_STEP_NSQuery) {
      //In fact, there are two different situations
      //1st: /net/ndnsim/DNS/www/NS is nacked
      //2st: /net/DNS/ndnsim/www/NS is nacked
      if (m_query.getRrType() == RR_NS) {
        m_step = QUERY_STEP_AnswerStub;
      }
//      else if (m_query.getRrType () == RR_ID_CERT && m_rrLabelLen == 1) {
//        //here working for KSK and ID-CERT when get a Nack
//        //e.g., /net/ndnsim/ksk-1, ksk-1 returns nack, but it should query /net
//
//        Name dstLabel = m_query.getRrLabel ();
//        Name label = dstLabel.getSubName (m_finishedLabelNum, m_rrLabelLen);
//        if (boost::starts_with (label.toUri (), "/ksk-")
//          || boost::starts_with (label.toUri (), "/KSK-")) {
//          m_finishedLabelNum = m_lastFinishedLabelNum;
//          m_forwardingHint = m_lastForwardingHint;
//        }
//      }
      else {
        m_step = QUERY_STEP_RRQuery;
      }
    }      // fi(NSQuery after geting Nack)
    else if (m_step == QUERY_STEP_RRQuery) {
      m_step = QUERY_STEP_AnswerStub;
    }
    else if (m_step == QUERY_STEP_FHQuery) {
      m_step = QUERY_STEP_Abort;
      abort();
    }
    m_lastResponse = re;
  }
  else if (re.getResponseType() == RESPONSE_NDNS_Auth) { // need more specific info
    m_rrLabelLen += 1;
  }
  else if (re.getResponseType() == RESPONSE_NDNS_Resp) { // get the intermediate answer
    //std::cout<<"step 2"<<std::endl;
    if (m_step == QUERY_STEP_NSQuery) {
      m_step = QUERY_STEP_FHQuery;
      //std::cout<<"step 3"<<std::endl;
      m_lastResponse = re;
      //std::cout<<"step 4"<<std::endl;

      if (m_query.getRrType() == RR_NS && m_finishedLabelNum == m_query.getRrLabel().size()) {
        m_step = QUERY_STEP_AnswerStub;
      }
    }
    else if (m_step == QUERY_STEP_FHQuery) {
      m_step = QUERY_STEP_NSQuery;
      //m_lastResponse = re;

      m_lastFinishedLabelNum = m_finishedLabelNum;
      m_lastForwardingHint = m_forwardingHint;
      m_finishedLabelNum += m_rrLabelLen;
      m_rrLabelLen = 1;
      m_authZoneIndex = 0;
      RR rr = re.getRrs()[m_forwardingHintIndex];
      m_forwardingHint = Name(rr.getRrData());

    }
    else if (m_step == QUERY_STEP_RRQuery) { // final resolver gets result back
      m_step = QUERY_STEP_AnswerStub;
      m_lastResponse = re;
    }
    //std::cout<<"step 5"<<std::endl;
    //std::cout << "get RRs: " << re.getStringRRs() << std::endl;
  }    //NDNS_Resp Response

}

/**
 * @brief call only when m_step belongs to
 *      [QUERY_STEP_RRQuery, QUERY_STEP_NSQuery, QUERY_STEP_FHQuery]
 */
const Interest
IterativeQueryWithForwardingHint::toLatestInterest()
{

  Query query = Query();
  Name dstLabel = m_query.getRrLabel();

  Name authZone = dstLabel.getPrefix(m_finishedLabelNum);

  Name label;
  if (m_step == QUERY_STEP_RRQuery) {
    label = dstLabel.getSubName(m_finishedLabelNum);
    query.setRrType(m_query.getRrType());
    if (m_query.getRrType() == RR_ID_CERT) {
      query.setQueryType(QUERY_KEY);
      query.setQueryType(QUERY_DNS);
    }
    else {
      query.setQueryType(QUERY_DNS);
    }
  }
  else if (m_step == QUERY_STEP_NSQuery) {
    label = dstLabel.getSubName(m_finishedLabelNum, m_rrLabelLen);
    query.setRrType(RR_NS);
    query.setQueryType(QUERY_DNS);

  }
  else if (m_step == QUERY_STEP_FHQuery) {
    RR rr = m_lastResponse.getRrs()[m_authZoneIndex];
    label = Name(rr.getRrData());
    query.setRrType(RR_FH);
    query.setQueryType(QUERY_DNS);
  }
  else {
    std::cout << "[* !!!! *] should not be called. toLatestInterest: " << *this << std::endl;
    return m_lastInterest;
  }

  query.setRrLabel(label);
  query.setAuthorityZone(authZone);
  query.setFowardingHint(m_forwardingHint);
  //std::cout<<"--------------lastFH="<<m_lastForwardingHint<<std::endl;
  Interest interest = query.toInterest();

  return interest;
}

} /* namespace ndns */
} /* namespace ndn */
