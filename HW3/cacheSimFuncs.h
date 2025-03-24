#ifndef CACHESIMFUNCS_H
#define CACHESIMFUNCS_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
class slot {
    unsigned int tag;
    bool valid, dirty;
    unsigned int load_ts, access_ts;
};

class set {
    std::vector<set> slots;
};

class cache {
    std::vector<set> sets;
};
#endif // CACHESIMFUNCS_H
