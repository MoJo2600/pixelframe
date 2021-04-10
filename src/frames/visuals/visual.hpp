#ifndef VISUAL_HPP_INCLUDED
#define VISUAL_HPP_INCLUDED

#include <map>
#include <iostream>
#include "lib/utils.h"

class Visual {
  public:
    virtual void loop(void) = 0;
    virtual void enter(void) = 0;
    virtual ~Visual() = 0;
};

template<typename T> Visual * createT() { 
  std::cout << "Creating visual instance" << std::endl;
  return new T; 
}

struct VisualFactory {
  public:
    typedef std::map<std::string, Visual*(*)()> map_type;

    static Visual * getRandomVisual() {
      map_type::iterator it = getMap()->begin();
      std::advance(it, random_0_to_n(getMap()->size()));
      return it->second();
    }

    static Visual * createInstance(std::string const& s) {
        map_type::iterator it = getMap()->find(s);
        if(it == getMap()->end())
            return 0;
        return it->second();
    }

  protected:
    static map_type * getMap() {
        // never delete'ed. (exist until program termination)
        // because we can't guarantee correct destruction order 
        if(!map) { map = new map_type; } 
        return map; 
    }

  private:
    static map_type * map;
};

template<typename T>
struct RegisterVisual : VisualFactory { 
    RegisterVisual(std::string const& s) { 
        getMap()->insert(std::make_pair(s, &createT<T>));
    }
};
#endif // VISUAL_HPP_INCLUDED