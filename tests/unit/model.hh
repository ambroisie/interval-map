#pragma once

#include <vector>

template <typename K, typename V> struct Range {
    K begin;
    K end;
    V val;
};

// Same behaviour as interval_map, but implementation is trivally correct
template <typename K, typename V> class Model {
public:
    Model(V const& init) : init_(init) {}

    void assign(K const& begin, K const& end, V const& val) {
        if (!(begin < end))
            return;
        ranges_.emplace_back(begin, end, val);
    }

    V const& operator[](K const& key) const {
        for (auto it = ranges_.rbegin(); it != ranges_.rend(); ++it) {
            if (key < it->begin)
                continue;
            if (it->end <= key)
                continue;
            return it->val;
        }
        return init_;
    }

    V init_;
    std::vector<Range<K, V>> ranges_{};
};
