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

#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/util/io.hpp>
#include <ndn-cxx/encoding/oid.hpp>

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>

using namespace ndn;
using namespace ndn::time;

using namespace std;
using namespace boost;

/*
 * The program will generate a KSK and DSK (and their certificate) according to
 * give zone name
 *
 * @input: zone name
 * @output:
 *  1) KeyChain System: hold the Identity with zone name
 *  2) KeyChain System: hold a KSK and its self-signed certificate
 *  3) KeyChain System: hold a DSK, as default key; and its self-KSK-signed certificate as the default
 *  4) File System: KSK self-signed certificate with suffix <KeyName>-SS.cert
 *  5) File System: DKS signed by KSk certificate with suffix <KeyName>-KS.cert
 */

int
main (int argc, char * argv[])
{
  std::string appName = boost::filesystem::basename (argv[0]);

  string zoneNameStr;

  string notBeforeStr;
  string notAfterStr;

  system_clock::TimePoint notBefore;
  system_clock::TimePoint notAfter;

  try {

    namespace po = boost::program_options;
    po::variables_map vm;

    po::options_description generic ("Generic Options");
    generic.add_options () ("help,h", "print help message") (
      "not-before,S", po::value<std::string> (&notBeforeStr),
      "certificate starting date, YYYYMMDDhhmmss") ("not-after,E",
                                                    po::value<std::string> (&notAfterStr),
                                                    "certificate ending date, YYYYMMDDhhmmss");

    po::options_description config ("Configuration");
    config.add_options () ("zone,z", po::value<std::string> (&zoneNameStr), "set the identity");

    po::options_description hidden ("Hidden Options");
    hidden.add_options ();

    po::positional_options_description postion;
    postion.add ("zone", 1);

    if (vm.count ("not-before") == 0) {
      notBefore = system_clock::now ();
    }
    else {
      notBefore = fromIsoString (notBeforeStr.substr (0, 8) + "T" + notBeforeStr.substr (8, 6));
    }

    if (vm.count ("not-after") == 0) {
      notAfter = notBefore + days (365);
    }
    else {
      notAfter = fromIsoString (notAfterStr.substr (0, 8) + "T" + notAfterStr.substr (8, 6));

      if (notAfter < notBefore) {
        std::cerr << "not-before is later than not-after" << std::endl;
        return 1;
      }
    }

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

    if (vm.count ("help")) {
      cout << "E.g: zone-build /net/ndnsim" << endl;
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

  KeyChain keyChain;
  Name identity (zoneNameStr);
  Name KSKName;
  Name DSKName;
  Name identityCertName;

  if (keyChain.doesIdentityExist (identity)) {
    cout << "Identity with name=" << zoneNameStr << " already exists in KeyChain System" << endl;
    return 0;
  }

  KSKName = keyChain.generateRsaKeyPair (identity, true); //KSK
  DSKName = keyChain.generateRsaKeyPairAsDefault (identity, false); //DSK

  boost::shared_ptr<IdentityCertificate> cert = keyChain.selfSign (KSKName);
  keyChain.addCertificate (*cert);

  string temp, fout;
  temp = KSKName.toUri ();
  replace_all (temp, "/", "-");
  fout = temp.substr (1) + "-SS.cert";
  ndn::io::save (*cert, fout);
  cout << *cert << endl;
  cout << "save cert to file: " << fout << endl;

  vector<CertificateSubjectDescription> desc;
  desc.push_back (
    CertificateSubjectDescription (ndn::oid::ATTRIBUTE_NAME, "Signer Key: " + KSKName.toUri ())); // push any subject description into the list.

  boost::shared_ptr<IdentityCertificate> cert2 = keyChain.prepareUnsignedIdentityCertificate (
    DSKName, identity, notBefore, notAfter, desc);

  keyChain.sign (*cert2, cert->getName ());
  keyChain.addCertificateAsKeyDefault (*cert2);

  temp = DSKName.toUri ();
  replace_all (temp, "/", "-");
  fout = temp.substr (1) + "-SKS.cert"; //self-KSK-signed
  ndn::io::save (*cert2, fout);
  cout << *cert2 << endl;
  cout << "save cert to file: " << fout << endl;

} //main

