#ifndef ENTRY_HPP
#define ENTRY_HPP

#include "../../math/distribution.hpp"

namespace csapex {
using DataIndex = std::array<int, 3>;

struct Entry {
    int                      cluster;
    DataIndex                index;
    std::vector<int>         indices;

    Entry() :
        cluster(-1)
    {
    }
};

struct EntryStatistical  : public Entry
{
    math::Distribution<3> distribution;
    EntryStatistical() :
        Entry()
    {
    }
};
}

#endif // ENTRY_HPP
