#pragma once

#include <map>

// Testing class forward declaration
class IntervalMapTest;

namespace amby {

template <typename K, typename V> class interval_map {
public:
    interval_map(V const& init) : init_(init) {}

    void assign(K const& begin, K const& end, V const& val) {
        if (!(begin < end))
            return;

        auto const end_val = (*this)[end];
        underlying_.erase(underlying_.lower_bound(begin),
                          underlying_.upper_bound(end));
        if (!((*this)[begin] == val))
            underlying_.insert({begin, val});
        if (!((*this)[end] == end_val))
            underlying_.insert({end, end_val});
    }

    V const& operator[](K const& key) const {
        auto it = underlying_.upper_bound(key);
        if (it == underlying_.begin())
            return init_;
        return std::prev(it)->second;
    }

    // Used in testing
    friend class ::IntervalMapTest;

private:
    V init_;
    std::map<K, V> underlying_{};
};

} // namespace amby
