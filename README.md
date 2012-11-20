conf-wrapper
============

## Description
Yet another conf wrapper.
This conf wrapper uses a fork (vbouchaud/cpp-optparse) of cpp-optparse by Johannes Weißl (weisslj/cpp-optparse)

## Usage
You can use conf-wrapper editing Conf.hpp and Conf.cpp to add what option you want. Just follow the examples.
You might init conf-wrapper as following:
```c++
int     main(int ac, char **av) {
Conf::Instance().initCommandParser(ac, av);
Conf::Instance().initFromConf();
}
```
You might then use the following functions. Please, take a look at Conf.hpp in order to understand how it works. 
```c++
class Conf {

public:
template<typename T>
T  get(const std::string &key) const;

template<typename T>
void	put(const std::string &key, T value);

void	save(const std::string &s);
}
```
conf-wrapper is a singleton.

## Note
I'm aware that this code is NOT well-designed I didn't have so much time to work on, all improvement are very welcome.
conf-wrapper currently use boost::somethings. I hope I'll have enough time to change that.
