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

#ifndef RESPONSE_UPDATE_HPP_
#define RESPONSE_UPDATE_HPP_

#include "response.hpp"

namespace ndn {
namespace ndns {

/*
 *
 */
class ResponseUpdate : public Response
{
public:
  ResponseUpdate ();
  virtual
  ~ResponseUpdate ();

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

  const std::string&
  getResult () const
  {
    return m_result;
  }

  void
  setResult (const std::string& result)
  {
    m_result = result;
  }

private:
  std::string m_result;
};


inline std::ostream&
operator<< (std::ostream& os, const ResponseUpdate& ru)
{
  os << "ResponseUpdate: queryName=" << toNameDigest(ru.getQueryName ())
     << " responseType=" << toString (ru.getResponseType ())
     << " queryType=" << toString (ru.getQueryType ())
     << " result=" << ru.getResult();

  return os;
}
} // namespace ndns
} // namespace ndn

#endif /* RESPONSE_UPDATE_HPP_ */
