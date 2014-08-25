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
#include <boost/program_options.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include "app/dynamic-dns-update.hpp"

using namespace ndn;
using namespace ndn::ndns;
using namespace std;
using namespace boost;

/*
 * This program try to update the records in its parent zone.
 * This program runs on a zone (but not root zone),
 *   or a node whose label is delegated. The node should has its own KSK and DSK
 *
 * @attention:
 *    1) the root zone must be also an identity in the keyChain system
 *    2) the program will work only if the database is used in NDNS
 *
 * @input:
 *    1) node's DSK
 *
 * @output:
 *
 *
 *
 */

int
main (int argc, char * argv[])
{
//  DynamicDNSUpdate (const char* programName, const char* prefix, Zone zone, Name rrlabel,
//  RRType rrtype, std::string rrdata, UpdateAction action=ndn::ndns::UPDATE_ACTION_REPLACE_ALL);

  string keyNameStr;
  string zoneStr;
  string rrLabelStr;
  string rrTypeStr;
  string rrData;
  string actionStr;

  try {
    namespace po = boost::program_options;
    po::variables_map vm;

    po::options_description generic ("Generic Options");
    generic.add_options () ("help,h", "print help message");

    po::options_description config ("Configuration");
    config.add_options ();

    po::options_description hidden ("Hidden Options");

    hidden.add_options () ("zone,z", po::value<string> (&keyNameStr),
                               "Name of delegated zone's name")
                          ("label,l", po::value<string> (&rrLabelStr),
                               "The RR Label to be updated")
                          ("type,t", po::value<string> (&rrTypeStr),
                                   "The RR Type to be updated: FH|NS|ID-CERT|TXT")
                          ("data,d", po::value<string> (&rrData),
                                   "The RR Data that the record should be changed to")
                          ("action,a", po::value<string> (&actionStr),
                                   "The update action: ADD|REMOVE|REPLACE_ALL")
                          ("key,k", po::value<string> (&keyNameStr),
                           "The Certificate which is DSK signed by self (root) KSK");




    po::positional_options_description postion;

    postion.add("zone", 1);
    postion.add("label", 1);
    postion.add("type", 1);
    postion.add("data", 1);
    postion.add("action", 1);
    postion.add("key", 1);

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
      cout << "E.g: zone-register-root /ksk-1407883612811-SKS.cert" << endl;
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


  RRType rrType = toRRType(rrTypeStr);
  if (rrType == RR_UNKNOWN) {
    cout << "Unkown RRType=" << rrTypeStr << endl;
    return 0;
  }
  UpdateAction action = toUpdateAction(actionStr);
  if (action == UPDATE_ACTION_UNKNOWN) {
    cout << "Unkown UpdateAction=" << actionStr << endl;
    return 0;
  }

  if (action == UPDATE_ACTION_REPLACE || action == UPDATE_ACTION_NONE) {
    cout << "NOT support UpdateAction=" << actionStr << endl;
    return 0;
  }



  return 0;

}


