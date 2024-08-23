#pragma once

#include <map>

namespace amby {

template <typename K, typename V> class interval_map {
public:
    interval_map(V const& init) : init_(init) {}

    void assign(K const& begin, K const& end, V const& val) {
        if (!(begin < end))
            return;
        // TODO: implement
    }

    V const& operator[](K const& key) const {
        auto it = underlying_.upper_bound(key);
        if (it == underlying_.begin())
            return init_;
        return std::prev(it)->second;
    }

private:
    V init_;
    std::map<K, V> underlying_{};
};

} // namespace amby
