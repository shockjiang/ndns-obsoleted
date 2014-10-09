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

#ifndef NDNS_UTIL_CERT_IO_HPP
#define NDNS_UTIL_CERT_IO_HPP

#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/util/io.hpp>
#include <cryptopp/files.h>
#include "boost/smart_ptr/shared_ptr.hpp"
#include "boost/program_options.hpp"

#include "boost/filesystem.hpp"

using namespace ndn;
using namespace ndn::time;

using namespace std;

class HttpException : public std::runtime_error
{
public:
  explicit
  HttpException(const std::string& what)
    : std::runtime_error(what)
  {
  }
};

ndn::shared_ptr<ndn::IdentityCertificate>
getCertificateHttp(const std::string& host, const std::string& port, const std::string& path)
{
  using namespace boost::asio::ip;
  tcp::iostream requestStream;

  requestStream.expires_from_now(boost::posix_time::milliseconds(3000));

  requestStream.connect(host, port);
  if (!static_cast<bool>(requestStream)) {
    throw HttpException("HTTP connection error");
  }
  requestStream << "GET " << path << " HTTP/1.0\r\n";
  requestStream << "Host: " << host << "\r\n";
  requestStream << "Accept: */*\r\n";
  requestStream << "Cache-Control: no-cache\r\n";
  requestStream << "Connection: close\r\n\r\n";
  requestStream.flush();

  std::string statusLine;
  std::getline(requestStream, statusLine);
  if (!static_cast<bool>(requestStream)) {
    throw HttpException("HTTP communication error");
  }

  std::stringstream responseStream(statusLine);
  std::string httpVersion;
  responseStream >> httpVersion;
  unsigned int statusCode;
  responseStream >> statusCode;
  std::string statusMessage;

  std::getline(responseStream, statusMessage);
  if (!static_cast<bool>(requestStream) || httpVersion.substr(0, 5) != "HTTP/") {
    throw HttpException("HTTP communication error");
  }
  if (statusCode != 200) {
    throw HttpException("HTTP server error");
  }
  std::string header;
  while (std::getline(requestStream, header) && header != "\r")
    ;

  ndn::OBufferStream os;
  {
    using namespace CryptoPP;
    FileSource ss2(requestStream, true, new Base64Decoder(new FileSink(os)));
  }

  ndn::shared_ptr<ndn::IdentityCertificate> identityCertificate = ndn::make_shared<
    ndn::IdentityCertificate>();
  identityCertificate->wireDecode(ndn::Block(os.buf()));

  return identityCertificate;
}

ndn::shared_ptr<ndn::IdentityCertificate>
getIdentityCertificate(const std::string& fileName)
{

  if (fileName == "-")
    return ndn::io::load<ndn::IdentityCertificate>(std::cin);
  else {
    cout << "get the certificate from the file: " << fileName << endl;
    return ndn::io::load<ndn::IdentityCertificate>(fileName);
  }
}

#endif
