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

#ifndef NDNS_NDNS_TLV_HPP
#define NDNS_NDNS_TLV_HPP
namespace ndn {
namespace ndns {
namespace tlv {

enum
{
  ResponseType = 130,
  ResponseFressness = 131,
  ResponseContentBlob = 132,
  ResponseNumberOfRRData = 133,
  RRData = 134,

  RRDataSub1 = 141,
  RRDataRecord = 142,
  RRUpdateAction = 143,
  UpdateResult = 144,
  RRZone = 145,

  NDNSRecusive = 151,
  NDNSInteative = 152

};

} // namespace tlv
} // namespace ndns
} // namespace ndn

#endif
