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

        auto it = underlying_.upper_bound(end);
        auto const end_val = at_upper_bound(it);

        bool insert_begin = !(val == init_);

        while (it != underlying_.begin()) {
            it = std::prev(it);
            auto begin_found = it->first < begin;
            if (begin_found) {
                insert_begin = !(val == it->second);
                break;
            }
            if (it != underlying_.end())
                // Account for up-coming `std::prev` with `++`
                underlying_.erase(it++);
        }

        if (insert_begin)
            it = underlying_.insert(it, {begin, val});
        // Get the proper upper-bound for `end`
        it = (it == underlying_.end() || end < it->first) ? it : std::next(it);
        if (!(at_upper_bound(it) == end_val))
            underlying_.insert(it, {end, end_val});
    }

    V const& operator[](K const& key) const {
        return at_upper_bound(underlying_.upper_bound(key));
    }

    // Used in testing
    friend class ::IntervalMapTest;

private:
    V const& at_upper_bound(std::map<K, V>::const_iterator it) const {
        if (it == underlying_.begin())
            return init_;
        return std::prev(it)->second;
    }

    V init_;
    std::map<K, V> underlying_{};
};

} // namespace amby
