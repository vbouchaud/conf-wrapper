/**
 * @file   Conf.cpp
 * @author Vianney Bouchaud <vianney@bouchaud.org>
 * @date   Thu Oct 13 10:53:52 2011
 * 
 * @brief  Implemented class for configuration file.
 * 
 */

#include <errno.h>
#include <string>
#include <list>

#include "Conf.hpp"

#include <boost/thread/mutex.hpp>

Conf::Conf() {
}

Conf::~Conf() {
}

void
Conf::initFromConf() {
  load(_values["fileconf"]);
}

bool
Conf::load(const std::string &fileName) {
  using boost::property_tree::ptree;

  try {
    read_ini(fileName, _pt);
  }
  catch (...) {
    return (false);
  }
  return (true);
}

void
Conf::save(const std::string &fileName) {
  boost::mutex::scoped_lock	scope(_mutex);

  using boost::property_tree::ptree;

  write_ini(fileName, _pt);
}

void
Conf::initCommandParser(int ac, char **av) {
  boost::mutex::scoped_lock	scope(_mutex);

#ifndef DISABLE_USAGE
  const std::string	usage = "usage: %prog [OPTION]... FOLDER [FILE]...";
#else
  const std::string	usage = SUPPRESS_USAGE;
#endif
  const std::string	version = "%prog 0.1\n"
    "This is free software: you are free to change and redistribute it.\n"
    "There is NO WARRANTY, to the extent permitted by law.";
  const std::string	desc = "Just another conf wrapper.";
  const std::string	epilog = "To get more informations contact me at vianney@bouchaud.org"
    "Fork me on Github https://github.com/vbouchaud/conf-wrapper";

  _parser.usage(usage)
    .version(version)
    .description(desc)
    .epilog(epilog);
#ifdef DISABLE_INTERSPERSED_ARGS
  .disable_interspersed_args()
#endif
     ;

  _parser.set_defaults(OptionKey::confFile,  "file.conf");
  _parser.add_option("-c",  "--conf")
    .metavar("FILE")
    .dest(OptionKey::confFile)
    .help("get configuration from FILE");

  _parser.set_defaults(OptionKey::generalVerbosity, "50");
  _parser.add_option("--verbose")
    .action("store_const")
    .set_const("100")
    .dest(OptionKey::generalVerbosity)
    .help("more verbose output");

  _parser.add_option("--silent")
    .action("store_const")
    .set_const("0")
    .dest(OptionKey::generalVerbosity)
    .help("silent mode");

  _parser.set_defaults(OptionKey::generalStdLog, "std.log");
  _parser.add_option("-l", "--log")
    .metavar("FILE")
    .dest(OptionKey::generalStdLog)
    .help("write sandard report to FILE");

  _parser.set_defaults(OptionKey::generalErrLog, "error.log");
  _parser.add_option("-e", "--err")
    .metavar("FILE")
    .dest(OptionKey::generalErrLog)
    .help("write error report to FILE");

  _values = _parser.parse_args(ac, av);
  _args = _parser.getArgs();
}

