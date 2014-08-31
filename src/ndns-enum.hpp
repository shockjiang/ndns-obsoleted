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

#ifndef NDNS_NDNS_ENUM_HPP
#define NDNS_NDNS_ENUM_HPP

namespace ndn {
namespace ndns {



enum RRType
{
  RR_NS = 1,
  RR_TXT,
  RR_FH,
  RR_ID_CERT,
  RR_DYNDNS_UPDATE,
  RR_A,
  RR_AAAA,
  RR_UNKNOWN
};
inline std::string
toString (const RRType& type)
{
  std::string str;

  switch (type) {
  case RR_NS:
    str = "NS";
    break;
  case RR_TXT:
    str = "TXT";
    break;
  case RR_FH:
    str = "FH";
    break;
  case RR_A:
    str = "A";
    break;
  case RR_AAAA:
    str = "AAAA";
    break;
  case RR_ID_CERT:
    str = "ID-CERT";
    break;
  case RR_DYNDNS_UPDATE:
    str = "DYNDNSUPDATE";
    break;
  default:
    str = "UNKNOWN";
    break;
  }
  return str;
}

inline RRType
toRRType (const std::string& str)
{
  RRType atype;
  if (str == "NS") {
    atype = RR_NS;
  }
  else if (str == "TXT") {
    atype = RR_TXT;
  }
  else if (str == "FH") {
    atype = RR_FH;
  }
  else if (str == "A") {
    atype = RR_A;
  }
  else if (str == "AAAA") {
    atype = RR_AAAA;
  }
  else if (str == "ID-CERT") {
    atype = RR_ID_CERT;
  }
  else if (str == "DYNDNSUPDATE") {
    atype = RR_DYNDNS_UPDATE;
  }

  else {
    atype = RR_UNKNOWN;
  }
  return atype;
}

enum ResponseType
{
  RESPONSE_NDNS_Resp = 1,
  RESPONSE_NDNS_Nack,
  RESPONSE_NDNS_Auth,
  RESPONSE_DyNDNS_OK,
  RESPONSE_DyNDNS_FAIL,
  RESPONSE_UNKNOWN
};

inline std::string
toString (ResponseType responseType)
{
  std::string label;
  switch (responseType) {
  case RESPONSE_NDNS_Resp:
    label = "NDNS-Resp";
    break;
  case RESPONSE_NDNS_Nack:
    label = "NDNS-Nack";
    break;
  case RESPONSE_NDNS_Auth:
    label = "NDNS-Auth";
    break;
  case RESPONSE_DyNDNS_OK:
    label = "DyNDNS-OK";
    break;
  case RESPONSE_DyNDNS_FAIL:
    label = "DyNDNS-Fail";
    break;
  default:
    label = "UNKNOWN";
    break;
  }
  return label;
}

inline ResponseType
toResponseType (const std::string& str)
{
  ResponseType atype;
  if (str == "NDNS-Resp") {
    atype = RESPONSE_NDNS_Resp;
  }
  else if (str == "NDNS-Nack") {
    atype = RESPONSE_NDNS_Nack;
  }
  else if (str == "NDNS-Auth") {
    atype = RESPONSE_NDNS_Auth;
  }
  else if (str == "DyNDNS-OK")
    atype = RESPONSE_DyNDNS_OK;
  else if (str == "DyNDNS-Fail")
    atype = RESPONSE_DyNDNS_FAIL;
  else {
    atype = RESPONSE_UNKNOWN;
  }
  return atype;
}

enum QueryType
{
  QUERY_DNS = 1,
  QUERY_DNS_R,
  QUERY_KEY,
  QUERY_UNKNOWN
};

inline std::string
toString (const QueryType& qType)
{
  std::string label;
  switch (qType) {
  case QUERY_DNS:
    label = "DNS";
    break;
  case QUERY_DNS_R:
    label = "DNS-R";
    break;
  case QUERY_KEY:
    label = "KEY";
    break;
  default:
    label = "UNKNOWN";
    break;
  }
  return label;

}

inline const QueryType
toQueryType (const std::string& str)
{
  QueryType atype;
  if (str == "DNS") {
    atype = QUERY_DNS;
  }
  else if (str == "DNS-R") {
    atype = QUERY_DNS_R;
  }
  else if (str == "KEY") {
    atype = QUERY_KEY;
  }
  else {
    atype = QUERY_UNKNOWN;
  }
  return atype;
}

enum QuerySteps
{
  QUERY_STEP_NSQuery = 1,
  QUERY_STEP_NackNS,
  QUERY_STEP_RRQuery,
  QUERY_STEP_NackRR,
  QUERY_STEP_FHQuery,
  QUERY_STEP_AnswerStub,
  QUERY_STEP_Abort,
  QUERY_STEP_FinishedSuccessfully,
  QUERY_STEP_UNKNOWN
};

inline std::string
toString (const QuerySteps& step)
{
  std::string label;
  switch (step) {
  case QUERY_STEP_NSQuery:
    label = "NSQuery";
    break;
  case QUERY_STEP_NackNS:
    label = "NackNS";
    break;
  case QUERY_STEP_RRQuery:
    label = "RRQuery";
    break;
  case QUERY_STEP_NackRR:
    label = "NackRR";
    break;
  case QUERY_STEP_FHQuery:
    label = "FHQuery";
    break;
  case QUERY_STEP_AnswerStub:
    label = "AnswerStub";
    break;
  case QUERY_STEP_Abort:
    label = "Abort";
    break;
  case QUERY_STEP_FinishedSuccessfully:
    label = "FinishedSuccessfully";
    break;
  default:
    label = "UNKNOW";
  }

  return label;
}

inline QuerySteps
toQuerySteps (std::string str)
{
  QuerySteps step = QUERY_STEP_UNKNOWN;
  if (str == "NSQuery") {
    step = QUERY_STEP_NSQuery;
  }
  else if (str == "NackNS") {
    step = QUERY_STEP_NackNS;
  }
  else if (str == "RRQuery") {
    step = QUERY_STEP_RRQuery;
  }
  else if (str == "NackRR") {
    step = QUERY_STEP_NackRR;
  }
  else if (str == "FHQuery")
    step = QUERY_STEP_FHQuery;
  else if (str == "AnswerStub")
    step = QUERY_STEP_AnswerStub;
  else if (str == "Abort")
    step = QUERY_STEP_Abort;
  else if (str == "FinishedSuccessfully")
    step = QUERY_STEP_FinishedSuccessfully;
  else
    step = QUERY_STEP_UNKNOWN;

  return step;
}


enum UpdateAction
{
  UPDATE_ACTION_NONE = 1, /*this is the value when RR is not used to update (QueryUpdate) */
  UPDATE_ACTION_ADD,
  UPDATE_ACTION_REMOVE,
  UPDATE_ACTION_REPLACE_ALL,
  UPDATE_ACTION_REPLACE, /*not support yet, since only RR data is specified in a RR*/
  UPDATE_ACTION_UNKNOWN
};

inline const UpdateAction
toUpdateAction(const std::string& str)
{
  UpdateAction action = UPDATE_ACTION_UNKNOWN;
  if (str == "NONE")
    action = UPDATE_ACTION_NONE;
  else if (str == "ADD")
    action = UPDATE_ACTION_ADD;
  else if (str == "REMOVE")
    action = UPDATE_ACTION_REMOVE;
  else if (str == "REPLACE-ALL")
    action = UPDATE_ACTION_REPLACE_ALL;
  else if (str == "REPLACE")
    action = UPDATE_ACTION_REPLACE;
  else if (str == "UNKNOWN")
    action = UPDATE_ACTION_UNKNOWN;
  else
    action = UPDATE_ACTION_UNKNOWN;

  return action;
}

inline std::string
toString(UpdateAction action)
{
  std::string str = "UNKNOWN";
  switch (action) {
  case UPDATE_ACTION_NONE:
    str = "NONE";
    break;
  case UPDATE_ACTION_ADD:
    str = "ADD";
    break;
  case UPDATE_ACTION_REMOVE:
    str = "REMOVE";
    break;
  case UPDATE_ACTION_REPLACE:
    str = "REPLACE";
    break;
  case UPDATE_ACTION_REPLACE_ALL:
    str = "REPLACE-ALL";
    break;
  default:
    str = "UNKNOWN";
  }

  return str;
}


} //namespace ndns;
} //namespace ndn

#endif
