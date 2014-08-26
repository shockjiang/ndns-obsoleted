#ifndef NDNS_TESTS_BOOST_TEST_HPP
#define NDNS_TESTS_BOOST_TEST_HPP

// suppress warnings from Boost.Test
#pragma GCC system_header
#pragma clang system_header

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/concept_check.hpp>
#include <boost/test/output_test_stream.hpp>

#include <ndn-cxx/name.hpp>
#include <ndn-cxx/data.hpp>
#include <ndn-cxx/interest.hpp>

namespace ndn {
namespace ndns {
namespace tests {

void inline
printbegin (std::string label)
{
  std::cout << std::endl << "-->>>>>>>>>>>>>>" << label << " begins:" << std::endl;
}

void inline
printend (std::string label)
{
  std::cout << "<<<<<<<<<<<<<<--" << label << " ends." << std::endl;
}
} //tests
} //ndns
} //ndn
#endif // NDN_TESTS_BOOST_TEST_HPP
