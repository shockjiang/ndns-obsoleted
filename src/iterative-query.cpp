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

#include "iterative-query.hpp"

namespace ndn {
namespace ndns {

IterativeQuery::IterativeQuery(const Query& query)
  : m_step(QUERY_STEP_NSQuery),
    m_tryNum(0),
    m_tryMax(2),
    m_query(query),
    m_finishedLabelNum(0),
    m_lastFinishedLabelNum(0),
    m_rrLabelLen(1),
    m_authZoneIndex(0)
{
}

IterativeQuery::~IterativeQuery()
{
}

bool
IterativeQuery::doTimeout()
{
  abort();
  return false;
}

void
IterativeQuery::abort()
{
  std::cout << "QUERY_STEP_Abort the Resolving" << std::endl;
  std::cout << (*this);
  std::cout << std::endl;
}

void
IterativeQuery::doData(Data& data)
{
  std::cout << "[* -> *] resolve Data: " << data.getName().toUri() << std::endl;
  Response re;
  re.fromData(data);
  std::cout << re << std::endl;

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
//        Name dstLabel = m_query.getRrLabel ();
      //        //here working for KSK and ID-CERT when get a Nack
      //        //e.g., /net/ndnsim/ksk-1, ksk-1 returns nack, but it should query /net
      //
//        Name label = dstLabel.getSubName (m_finishedLabelNum, m_rrLabelLen);
//        if (boost::starts_with (label.toUri (), "/ksk")
//          || boost::starts_with (label.toUri (), "/KSK")) {
//          m_finishedLabelNum = m_lastFinishedLabelNum;
//        }
//
//      }
      else {
        m_step = QUERY_STEP_RRQuery;
      }
    }      //Nack  NSQuery
    else if (m_step == QUERY_STEP_RRQuery) {
      m_step = QUERY_STEP_AnswerStub;
    }      //Nack RRQuery

    m_lastResponse = re;
  }      //fie (NDNS_NACK), after getting NACK
  else if (re.getResponseType() == RESPONSE_NDNS_Auth) { // need more specific info
    m_rrLabelLen += 1;
  } //fie(NDNS_Auth), after getting Auth
  else if (re.getResponseType() == RESPONSE_NDNS_Resp) { // get the intermediate answer
    if (m_step == QUERY_STEP_NSQuery) {
      //do nothing, step QUERY_STEP_NSQuery
      m_lastFinishedLabelNum = m_finishedLabelNum;
      m_finishedLabelNum += m_rrLabelLen;
      m_rrLabelLen = 1;
      m_authZoneIndex = 0;
      m_lastResponse = re;

      if (m_query.getRrType() == RR_NS && m_finishedLabelNum == m_query.getRrLabel().size()) {
        m_step = QUERY_STEP_AnswerStub;
      }

    }
    else if (m_step == QUERY_STEP_RRQuery) { // final resolver gets result back
      m_step = QUERY_STEP_AnswerStub;
      m_lastResponse = re;
    }

    std::cout << "get RRs: " << m_lastResponse.getStringRRs() << std::endl;
  } //fie(NDNS_Resp), after getting Resp

}

const Interest
IterativeQuery::toLatestInterest()
{
  Query query = Query();
  Name dstLabel = m_query.getRrLabel();

  Name authZone = dstLabel.getPrefix(m_finishedLabelNum);

  Name label;
  if (m_step == QUERY_STEP_RRQuery) {
    label = dstLabel.getSubName(m_finishedLabelNum);
  }
  else {
    label = dstLabel.getSubName(m_finishedLabelNum, m_rrLabelLen);
  }
  query.setAuthorityZone(authZone);
  query.setRrLabel(label);

  if (m_step == QUERY_STEP_NSQuery) {
    query.setRrType(RR_NS);
    query.setQueryType(QUERY_DNS);
  }
  else if (m_step == QUERY_STEP_RRQuery) {
    query.setRrType(m_query.getRrType());
    if (m_query.getRrType() == RR_ID_CERT) {
      query.setQueryType(QUERY_KEY);
      query.setQueryType(QUERY_DNS);
    }
    else {
      query.setQueryType(QUERY_DNS);
    }

  }
  else if (m_step == QUERY_STEP_AnswerStub) {
    query.setRrType(m_query.getRrType());
    query.setQueryType(QUERY_DNS);
  }
  Interest interest = query.toInterest();
  //m_lastInterest = interest;

  return interest;
}

} /* namespace ndns */
} /* namespace ndn */
