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

#include <fstream>

//#include <cryptopp/files.h>
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/util/io.hpp>
#include <ndn-cxx/encoding/oid.hpp>

//#include <boost/ptr_container/ptr_container.hpp>
#include <boost/program_options.hpp>
//#include "boost/filesystem.hpp"
#include <boost/algorithm/string/replace.hpp>

#include "service/zone-register.hpp"

using namespace ndn;
using namespace ndn::ndns;
using namespace std;
using namespace ndn::time;
using namespace boost;

/*
 * This program register a sub zone (cannot be root zone)
 *  into the NDNS under current working zone
 * @attention:
 *    1) the name of the zone must be also an identity in the keyChain system
 *    2) the zone cannot be the root zone
 *    3) the program will work only if the database is used in NDNS
 *
 * @input:
 *    1) subzone name
 *    2) file and its name: KSK's self-signed certificate of sub zone
 *    3) parent zone's name, the name must be also an identity in the keyChain system
 *    3) the DSK of the current zone (parent zone) used to generate the certificate for the sub zone
 *
 * @output:
 *    1) NDNS system: zones table add a new entry
 *    2) NDN system: rrsets table add two entries (NS, ID-CERT)
 *    3) File system: the subzone's KSK certificated by parent's DKS, (-PKS.cert)
 *
 *
 *
 */

int
main (int argc, char * argv[])
{
  string certPath;
  string zoneStr;
  string subzoneStr;
  string key;
  string rrTypeStr = "NS";
  std::vector<string> rrdatas;

  string notBeforeStr;
  string notAfterStr;
  system_clock::TimePoint notBefore;
  system_clock::TimePoint notAfter;

  bool store = false;

  try {
    namespace po = boost::program_options;
    po::variables_map vm;

    po::options_description generic ("Generic Options");
    generic.add_options () ("help,h", "print help message")
      ("not-before,S", po::value<std::string> (&notBeforeStr),
      "certificate starting date, YYYYMMDDhhmmss")
      ("not-after,E", po::value<std::string> (&notAfterStr),
         "certificate ending date, YYYYMMDDhhmmss")
      ("store,s",
          "to store the PSK (parent signed key)'s \
          Certificate Name and filename to delegation-info.txt. Default is not")
            ;

    po::options_description config ("Configuration");
    config.add_options ();

    po::options_description hidden ("Hidden Options");
    hidden.add_options ()
      ("zone,z", po::value<string> (&zoneStr), "The Zone which is working on (parent zone)")
      ("subzone", po::value<string> (&subzoneStr), "The Zone to be added (subzone)")
      ("key,k", po::value<string> (&key),
                    "parent's DSK cert to sign the KSK of subzone")
      ("cert,c", po::value<string> (&certPath),
                    "The Certificate which is DSK signed by self (root) KSK")
      ("rrtype,t", po::value<string>(&rrTypeStr),"The RRType used to register")
      ("rrdata,d", po::value<vector<string> > (&rrdatas),
      "The rrdata os RR record, which is name server. if not set, default is <Zonename>/ns1");

    po::positional_options_description postion;
    postion.add ("zone", 1);
    postion.add ("subzone", 1);
    postion.add ("key", 1);
    postion.add ("cert", 1);
    postion.add ("rrdata", -1);

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
      cout
        << "E.g: "<< argv[0] <<" CurrentWorkingZone Subzone|Site   CertificateNameOfWorkingZone<prefer dsk-IDCERT> CeritificateFileNameOfSubZone<prefer ksk-*-SS.cert>"
        << endl;
      cout << visible << endl;
      return 0;
    }

    if (vm.count("store")) {
      store = true;
    }

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
        return 0;
      }
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

  RRType rrType = toRRType(rrTypeStr);
  if (rrType == RR_UNKNOWN) {
    cout << "unknow RR type: " << rrTypeStr << endl;
    return 0;
  }

  Name identity (zoneStr);
  KeyChain keyChain;
  if (!keyChain.doesIdentityExist (identity)) {
    cout << identity.toUri () << " is not an identity in the KeyChain System" << endl;
    return -1;
  }

  if (!keyChain.doesCertificateExist (key)) {
    cout << key << " does not exist in the KeyChain System" << endl;
    return -1;
  }

  //boost::shared_ptr<IdentityCertificate> pcert = keyChain.getCertificate(key);

  Name zoneName(identity);
  Zone zone (zoneName);

  Name subzoneName (subzoneStr);
  Zone subzone (subzoneName);
  boost::shared_ptr<IdentityCertificate> scert = ndn::io::load<ndn::IdentityCertificate> (certPath);

  vector<CertificateSubjectDescription> desc;
  desc.push_back (
    CertificateSubjectDescription (ndn::oid::ATTRIBUTE_NAME,
                                   "Owner: " + scert->getName ().toUri ())); // push any subject description into the list.
  desc.push_back (CertificateSubjectDescription (ndn::oid::ATTRIBUTE_NAME, "Signer Key: " + key)); // push any subject description into the list.

  IdentityCertificate certificate;
  Name n2 = zoneName;
	n2.append("KEY");
	n2.append(subzoneName.getSubName(zoneName.size()));
	n2.append(scert->getName().getSubName(subzoneName.size()+1));//remove the zone prefix and KEY
  certificate.setName (n2);
  certificate.setNotBefore (notBefore);
  certificate.setNotAfter (notAfter);
  certificate.setPublicKeyInfo (scert->getPublicKeyInfo ());

  vector<CertificateSubjectDescription>::const_iterator iter = desc.begin ();
  while (iter != desc.end ()) {
    certificate.addSubjectDescription ((CertificateSubjectDescription) *iter);
    iter++;
  }

  certificate.encode ();

  keyChain.sign (certificate, Name (key));

  string temp;
  temp = certificate.getName ().toUri ();
  replace_all (temp, "/", "-");
  string fout = temp.substr (1) + "-PKS.cert";
  ndn::io::save (certificate, fout);
  cout << certificate << endl;
  cout << "save cert to file: " << fout << endl;

  ZoneRegister zoneRegister (zone, subzone, certificate);

  if (rrdatas.size () == 0) {
    if (rrType == RR_NS)
      zoneRegister.addNsData (subzoneStr + "/ns1");
    else
      zoneRegister.addNsData(subzoneStr + "/host1");
  }
  else {
    zoneRegister.setNsDatas (rrdatas);
  }

  zoneRegister.setRrType(rrType);
  zoneRegister.tryRegister ();

  cout << zoneRegister << endl;

  cout << "finish register subzone [" << subzone << "] at zone [" << zone << "]" << endl;


  if (store) {
    ofstream f("delegation-details.txt", ios::app);
    f << subzoneStr << " " << zoneStr << " "
      << certificate.getName().toUri() << " " << fout << endl;
    f.close();
  }

  return 1;

}

