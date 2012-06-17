#ifndef _CACHE
#define _CACHE
#include <vector>
#include <string>
#include <algorithm>

#include <string.h>

#include <libmemcached/memcached.hpp>
//http://posulliv.github.com/2009/09/19/using-memcached-with-c.html
class MyCache
{
public:

  static const uint32_t num_of_clients= 10;

  static MyCache &singleton()
  {
    static MyCache instance;
    return instance;
  }

  void set(const std::string &key,
           const std::vector<char> &value)
  {
    time_t expiry= 0;
    uint32_t flags= 0;
    getCache()->set(key, value, expiry, flags);
  }

  std::vector<char> get(const std::string &key)
  {
    std::vector<char> ret_value;
    getCache()->get(key, ret_value);
    return ret_value;
  }

  void remove(const std::string &key)
  {
    getCache()->remove(key);
  }

  memcache::Memcache *getCache()
  {
    /* 
     * pick a random element from the vector of clients. 
     * Obviously, this is not very random but suffices as
     * an example!
     */
    uint32_t index= rand() % num_of_clients;
    return clients[index];
  }

private:

  /*
   * A vector of clients.
   */
  std::vector<memcache::Memcache *> clients;

  MyCache()
    :
      clients()
  {
    /* create clients and add them to the vector */
    for (uint32_t i= 0; i < num_of_clients; i++)
    {
      memcache::Memcache *client= 
        new memcache::Memcache("127.0.0.1:11211");
      clients.push_back(client);
    }
  }

  ~MyCache()
  {
      //FIXME IMPLEMENT DeletePtrs
      //for_each(clients.begin(), clients.end(), DeletePtrs());
    clients.clear();
  }

  MyCache(const MyCache&);
};
#endif
