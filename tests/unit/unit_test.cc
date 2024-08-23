#include <gtest/gtest.h>

#include <interval-map/interval-map.hh>

#include <type_traits>

template <typename T> class KeyInterface {
public:
    explicit KeyInterface(T val) : underlying_(val) {}

    KeyInterface(KeyInterface const&) = default;
    KeyInterface& operator=(KeyInterface const&) = default;

    bool operator<(KeyInterface const& other) const {
        return underlying_ < other.underlying_;
    }

private:
    T underlying_;
};

template <typename T> struct std::numeric_limits<KeyInterface<T>> {
    static KeyInterface<T> lowest() {
        return KeyInterface(std::numeric_limits<T>::lowest());
    }
};

static_assert(std::is_copy_constructible_v<KeyInterface<int>>);
static_assert(std::is_copy_assignable_v<KeyInterface<int>>);
static_assert(
    std::is_same_v<KeyInterface<int>,
                   decltype(std::numeric_limits<KeyInterface<int>>::lowest())>);

template <typename T> class ValueInterface {
public:
    explicit ValueInterface(T val) : underlying_(val) {}

    ValueInterface(ValueInterface const&) = default;
    ValueInterface& operator=(ValueInterface const&) = default;

    bool operator==(ValueInterface const& other) const {
        return underlying_ == other.underlying_;
    }

private:
    T underlying_;
};

template <typename T> struct std::numeric_limits<ValueInterface<T>> {
    static ValueInterface<T> lowest() {
        return ValueInterface(std::numeric_limits<T>::lowest());
    }
};

static_assert(std::is_copy_constructible_v<ValueInterface<int>>);
static_assert(std::is_copy_assignable_v<ValueInterface<int>>);
static_assert(std::is_same_v<
              ValueInterface<int>,
              decltype(std::numeric_limits<ValueInterface<int>>::lowest())>);

TEST(interval_map, minimal_interface) {
    using Key = KeyInterface<char>;
    using Value = ValueInterface<int>;

    auto map = amby::interval_map<Key, Value>{Value(0)};
    ASSERT_EQ(map[Key(0)], Value(0));
    map.assign(Key(0), Key(1), Value(1));
}

TEST(interval_map, no_insertion) {
    auto map = amby::interval_map<char, int>{0};
    for (int i = std::numeric_limits<char>::min();
         i <= std::numeric_limits<char>::max(); ++i) {
        ASSERT_EQ(map[i], 0);
    }
}

TEST(interval_map, insert_begin_equal_end) {
    auto map = amby::interval_map<char, int>{0};
    map.assign(0, 0, 1);
    for (int i = std::numeric_limits<char>::min();
         i <= std::numeric_limits<char>::max(); ++i) {
        ASSERT_EQ(map[i], 0);
    }
}

TEST(interval_map, insert_begin_bigger_than_end) {
    auto map = amby::interval_map<char, int>{0};
    map.assign(1, 0, 1);
    for (int i = std::numeric_limits<char>::min();
         i <= std::numeric_limits<char>::max(); ++i) {
        ASSERT_EQ(map[i], 0);
    }
}
