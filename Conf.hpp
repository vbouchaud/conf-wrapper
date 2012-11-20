#ifndef		__CONF_HH__
# define	__CONF_HH__

/**
 * @file   Conf.hpp
 * @author Vianney Bouchaud <vianney@bouchaud.org>
 * @date   Thu Oct 13 10:53:38 2011
 * 
 * @brief  Class header for configuration file.
 * 
 */

#include <string>
#include <list>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/lexical_cast.hpp>

#include "tSingleton.hpp"
#include "OptionParser.hpp"
#include "Logger.hpp"

#include <boost/thread/mutex.hpp>

//! List of all available key for Conf::put() and Conf::get() function.
namespace OptionKey {
  static const std::string	confFile		= "fileconf";
  static const std::string	generalVerbosity	= "general.verbosity";
  static const std::string	generalStdLog		= "general.standard-log";
  static const std::string	generalErrLog		= "general.error-log";
}

/**
 * @class Conf Conf.hpp Conf.cpp
 */
class	Conf: public utility::singlt<Conf> {
private:
  boost::mutex			_mutex;
  optparse::Values		_values;	/**< values getted by _parser */
  optparse::OptionParser	_parser;	/**< parser wrote with lib boost */
  std::list<std::string>	_args;		/**< args (string) getted by binary */
  boost::property_tree::ptree	_pt;		/**< ptree from lib boost */

  const short			_maxReplicata;

  /** 
   * This function will load a ptree from the file given in parameter.
   * 
   * @param s file to load path
   * 
   * @return false if there is an error else true
   */
  bool	load(const std::string &s);

protected:
  friend class	singlt<Conf>;
  Conf();
  ~Conf();

public:
  /** 
   * get max_replica allowed per file
   * 
   * 
   * @return max_replica allowed
   */
  short	getMaxReplicata() const;

  /** 
   * initCommandParser Will get binary's args and convert it into options. (overwriting options in file.conf)
   * 
   * @param ac nbr of arg getted by binary
   * @param av args getted by binary
   */
  void	initCommandParser(int ac, char **av);

  /** 
   * This function call load().
   * ThreadSafe
   * @return false if there is an error else true
   */
  void	initFromConf();

  /** 
   * Conf::get() return option asked.
   * 
   * @param key a string formated as "section.key". (list of available token is at the top of this file.)
   * 
   * @return the value in args else in ptree if it's not in args or default value where "section.key"
   */
  template<typename T>
  T  get(const std::string &key) const {
    using boost::lexical_cast;
    using boost::bad_lexical_cast;

    try {
      if (find(_args.begin(), _args.end(), key) != _args.end())
	return (boost::lexical_cast<T>(_values[key]));
      else if (_pt.find(key) != _pt.not_found())
	return (_pt.get<T>(key));
      return (boost::lexical_cast<T>(_values[key]));
    }
    catch(bad_lexical_cast &) {
      Logger::Instance().writeError("Conf", "Bad conversion with " + key + ".");
      return (boost::lexical_cast<T>("")); //FIXME : what to do in case of error?
    }
  }

  /** 
   * Conf::put() allow to edit ptree. Only the configuration file can be overwrited. If a key was previously in args, it'll be erased and getted from ptree for each Conf::get(key).
   * threadSafe
   *
   * @param key Key to put in ptree.
   * @param value Value vor key to put in ptree.
   */
  template<typename T>
  void	put(const std::string &key, T value) {
    boost::mutex::scoped_lock	scope(_mutex);

    _args.remove(key);
    _pt.put(key, value);
  }

  /** 
   * This function will save _pt in the file given in parameter.
   * ThreadSafe
   *
   * @param s file to write path
   */
  void	save(const std::string &s);
};

#endif /* !__CONF_HH__ */
