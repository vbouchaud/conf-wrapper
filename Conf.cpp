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
#include "tools.hpp"

#include <boost/thread/mutex.hpp>

Conf::Conf(): _maxReplicata(5) {
}

Conf::~Conf() {
}

void
Conf::initFromConf() {
  load(_values["fileconf"]);
}

short
Conf::getMaxReplicata() const {
  return _maxReplicata;
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
Conf::initCommandParser(int ac, char **av, const std::string &usage) {
  boost::mutex::scoped_lock	scope(_mutex);

#ifndef DISABLE_USAGE
  const std::string	usage = "usage: %prog [OPTION]... FOLDER [FILE]...";
#else
  const std::string	usage = SUPPRESS_USAGE;
#endif
  const std::string	version = "%prog 0.1\n"
    "This is free software: you are free to change and redistribute it.\n"
    "There is NO WARRANTY, to the extent permitted by law.";
  const std::string	desc = "This daemon logs you into the Bibimbap network";
  const std::string	epilog = "To get more informations contact us via our Github : https://github.com/bibimbap\n"
    "Fork me on Github https://github.com/bibimbap/Bibimbap";

  _parser.usage(usage)
    .version(version)
    .description(desc)
    .epilog(epilog);
#ifdef DISABLE_INTERSPERSED_ARGS
  .disable_interspersed_args()
#endif
     ;

  _parser.set_defaults(OptionKey::confFile, "bibimbap.conf");
  _parser.add_option("-c", "--conf")
    .metavar("FILE")
    .dest(OptionKey::confFile)
    .help("get configuration from FILE");

  _parser.set_defaults(OptionKey::confFolder, "./bibimbap/");
  _parser.add_option("--conf-folder")
    .metavar("FOLDER")
    .action("store")
    .dest(OptionKey::accountPvKey)
    .help("Path to your configuration folder.");

  _parser.set_defaults(OptionKey::dhtNode, "node.dht");
  _parser.add_option("--dht-file")
    .metavar("NAME")
    .action("store")
    .dest(OptionKey::accountPvKey)
    .help("Just the name (not the PATH) of your dht file.");

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

  _parser.set_defaults(OptionKey::generalStdLog, "bibimbap.log");
  _parser.add_option("-l", "--log")
    .metavar("FILE")
    .dest(OptionKey::generalStdLog)
    .help("write sandard report to FILE");

  _parser.set_defaults(OptionKey::generalErrLog, "error.log");
  _parser.add_option("-e", "--err")
    .metavar("FILE")
    .dest(OptionKey::generalErrLog)
    .help("write error report to FILE");

  _parser.set_defaults(OptionKey::networkIp, "localhost");
  _parser.add_option("-i", "--ip")
    .metavar("IP")
    .action("store")
    .dest(OptionKey::networkIp)
    .help("First ip to connect for bootstrap");

  _parser.set_defaults(OptionKey::networkPort, "4242");
  _parser.add_option("-p", "--port")
    .metavar("PORT")
    .action("store")
    .dest(OptionKey::networkPort)
    .help("First port to connect in adress for bootstrap");

  _parser.set_defaults(OptionKey::networkMaxPort, "4249");
  _parser.add_option("--max-listen-port")
    .metavar("PORT")
    .action("store")
    .dest(OptionKey::networkMaxPort)
    .help("Highest port to listen.");

  _parser.set_defaults(OptionKey::networkMinPort, "4242");
  _parser.add_option("--min-listen-port")
    .metavar("PORT")
    .action("store")
    .dest(OptionKey::networkMinPort)
    .help("Lowest port to listen.");

  _parser.set_defaults(OptionKey::networkMaxDl, "999");
  _parser.add_option("--max-download")
    .metavar("SPEED")
    .action("store")
    .dest(OptionKey::networkMaxDl)
    .help("Max speed limit for download.");

  _parser.set_defaults(OptionKey::networkMaxUp, "999");
  _parser.add_option("--max-upload")
    .metavar("SPEED")
    .action("store")
    .dest(OptionKey::networkMaxUp)
    .help("Max speed limit for upload.");

  _parser.set_defaults(OptionKey::folderTheir, "their/");
  _parser.add_option("-t", "--their-dir")
    .metavar("FOLDER")
    .action("store")
    .dest(OptionKey::folderTheir)
    .help("Directory where to save files from Bibimbap network.");

  _parser.set_defaults(OptionKey::folderMine, "mine/");
  _parser.add_option("-m", "--mine-dir")
    .metavar("FOLDER")
    .action("store")
    .dest(OptionKey::folderMine)
    .help("Directory where are your files to send on bibimbap network.");

  _parser.set_defaults(OptionKey::folderTheirSize, "2");
  _parser.add_option("-s", "--their-dir-size")
    .metavar("SIZE")
    .action("store")
    .dest(OptionKey::folderTheirSize)
    .help("Max limit for 'their' directory's size.");

  _parser.set_defaults(OptionKey::accountPbKey, "");
  _parser.add_option("--public-key")
    .metavar("FILE")
    .action("store")
    .dest(OptionKey::accountPbKey)
    .help("Path to your public key.");

  _parser.set_defaults(OptionKey::accountPvKey, "");
  _parser.add_option("--private-key")
    .metavar("FILE")
    .action("store")
    .dest(OptionKey::accountPvKey)
    .help("Path to your private key.");

  _values = _parser.parse_args(ac, av);
  _args = _parser.getArgs();
}

void
Conf::initWorkspace() {
  boost::mutex::scoped_lock	scope(_mutex);

  const std::string	paths[] = {
    this->get<std::string>(OptionKey::confFolder),
    this->get<std::string>(OptionKey::folderTheir),
    this->get<std::string>(OptionKey::folderMine)
  };

  for (int i = 0; i < 3; i++) {
    try {
      tools::io_utils::mkdirIfNotExists(paths[i]);
    }
    catch (const tools::io_utils::filesystemException & e) {
      Logger::Instance().writeError("Conf", e.what());
      // FIXME throw exception
    }
  }
}
