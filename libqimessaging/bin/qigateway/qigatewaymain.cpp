/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**
** Copyright (C) 2012 Aldebaran Robotics
*/

#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include <qi/os.hpp>
#include <qimessaging/gateway.hpp>


namespace po = boost::program_options;

int main(int argc, char *argv[])
{
  // declare the program options
  po::options_description desc("Usage:\n  qi-service masterAddress [options]\nOptions");
  desc.add_options()
      ("help", "Print this help.")
      ("master-address",
       po::value<std::string>()->default_value(std::string("127.0.0.1:5555")),
       "The master address")
      ("gateway-address",
       po::value<std::string>()->default_value(std::string("127.0.0.1:12345")),
       "The gateway address");

  // allow master address to be specified as the first arg
  po::positional_options_description pos;
  pos.add("master-address", 1);

  // parse and store
  po::variables_map vm;
  try
  {
    po::store(po::command_line_parser(argc, argv).
              options(desc).positional(pos).run(), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
      std::cout << desc << "\n";
      return 0;
    }

    if (vm.count("master-address") == 1 &&
        vm.count("gateway-address") == 1 )
    {
      std::string gatewayAddress = vm["gateway-address"].as<std::string>();
      std::string masterAddress = vm["master-address"].as<std::string>();

      qi::Gateway gate;
//      gate.start(gatewayAddress);
      std::cout << "ready." << std::endl;

//      gate.registerGateway(masterAddress, gatewayAddress);

      while (1)
        qi::os::sleep(1);

//      gate.unregisterGateway(gatewayAddress);
    }
    else
    {
      std::cout << desc << "\n";
    }
  }
  catch (const boost::program_options::error&)
  {
    std::cout << desc << "\n";
  }

  return 0;
}
