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

#include <ndn-cxx/util/io.hpp>
//#include <boost/ptr_container/ptr_container.hpp>
#include <boost/program_options.hpp>
//#include "boost/filesystem.hpp"
#include <ndn-cxx/security/key-chain.hpp>
//#include <typeinfo>
#include "service/zone-register.hpp"

using namespace ndn;
using namespace ndn::ndns;
using namespace std;
using namespace boost;

/*
 * This program register the zone into the NDNS
 * @attention:
 *    1) the zone must be also an identity in the keyChain system
 *    2) the program will work only if the database is used in NDNS
 *
 * @input:
 *    1) file and its name: DKS with its KSK-signed certificate (-SKK.cert)
 *    2) optionally, it also can register a site (not a zone) to its delegated zone
 *        if rrType is set not to be "NS"
 *
 * @output:
 *    1) NDNS system: zones table add a new entry
 *    2) NDN system: rrsets table add one entry (ID-CERT) its DSK-*-.SKK.cert
 *
 *
 *
 */

int
main (int argc, char * argv[])
{

  string certPath;
  std::string zoneStr;
  std::string rrTypeStr = "NS";
  try {
    namespace po = boost::program_options;
    po::variables_map vm;

    po::options_description generic ("Generic Options");
    generic.add_options () ("help,h", "print help message");

    po::options_description config ("Configuration");
    config.add_options ();

    po::options_description hidden ("Hidden Options");
    hidden.add_options () ("zone,z", po::value<string>(&zoneStr),
                            "The Zone's name")
                            ("cert,c", po::value<string> (&certPath),
                           "The Certificate which is DSK signed by self (root) KSK")

                           ;

    po::positional_options_description postion;
    postion.add("zone", 1);
    postion.add ("cert", 1);

    po::options_description cmdline_options;
    cmdline_options.add (generic).add (config).add (hidden);

    po::options_description config_file_options;
    config_file_options.add (config).add (hidden);

    po::options_description visible ("Allowed options");
    visible.add (generic).add (config);

    po::parsed_options parsed =
      po::command_line_parser (argc, argv).options (cmdline_options).positional (postion).run ();

    po::store (parsed, vm);
    po::notify (vm);

    if (vm.count ("help") || !vm.count("zone") || !vm.count("cert")) {
      cout << "E.g: " << argv[0] << " ZoneName CertificateOfZone<prefer dsk*-SS.cert>" << endl;
      cout << visible << endl;
      return 0;
    }

  }
  catch (const std::exception& ex) {
    cout << "Parameter Error: " << ex.what () << endl;
    return 0;
  }
  catch (...) {
    cout << "Parameter Unknown error" << endl;
    return 0;
  }

  Name zoneName (zoneStr);

  KeyChain keyChain;
  if (!keyChain.doesIdentityExist (zoneName)) {
    cout << zoneName.toUri () << " is not an identity in the KeyChain System" << endl;
    return -1;
  }

  Zone zone (zoneName);
  Zone& subzone (zone);
  boost::shared_ptr<IdentityCertificate> cert = ndn::io::load<ndn::IdentityCertificate> (certPath);


  ZoneRegister zoneRegister (zone, subzone, cert);
  zoneRegister.tryRegister ();
  cout << " finish to register the root zone" << endl;

  cout << zoneRegister << endl;

  return 1;

}

