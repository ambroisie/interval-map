#include <gtest/gtest.h>

#include <interval-map/interval-map.hh>

#include <sstream>
#include <type_traits>

#include "model.hh"

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

class IntervalMapTest : public testing::Test {
protected:
    using key_type = char;
    using value_type = int;
    using map_type = amby::interval_map<key_type, value_type>;
    using model_type = Model<key_type, value_type>;

    map_type map{0};
    model_type model{0};

    void SetUp() override {
        map = map_type{0};
        model = model_type{0};
    }

    void TearDown() override {
        check();
    }

    void assign(key_type const& begin, key_type const& end,
                value_type const& val) {
        map.assign(begin, end, val);
        model.assign(begin, end, val);
    }

    void check() const {
        SCOPED_TRACE(stringify_map());
        SCOPED_TRACE(stringify_operations());
        check_ranges();
        check_canonicity();
    }

    std::string stringify_map() const {
        std::ostringstream out;
        out << "map: ";
        for (const auto& [key, val] : map.underlying_)
            out << "[" << +key << ": " << +val << "]";
        return out.str();
    }

    std::string stringify_operations() const {
        std::ostringstream out;
        out << "ops: ";
        for (const auto& [start, end, val] : model.ranges_)
            out << "[" << +start << ":" << +end << " => " << +val << "]";
        return out.str();
    }

    // Compare against the fake 'Model' implementation
    void check_ranges() const {
        auto i = std::numeric_limits<key_type>::min();
        for (; i < std::numeric_limits<key_type>::max(); ++i) {
            ASSERT_EQ(map[i], model[i]) << "(i: " << +i << ")";
        }
        ASSERT_EQ(map[i], model[i]) << "(i: " << +i << ")";
    };

    void check_canonicity() const {
        // Consecutive map entries must not contain the same value
        for (auto it = map.underlying_.begin(); it != map.underlying_.end();
             ++it) {
            const auto next = std::next(it, 1);
            if (next == map.underlying_.end())
                break;
            EXPECT_NE(it->second, next->second);
        }

        // The first entry must not contain the initial value
        if (const auto it = map.underlying_.begin();
            it != map.underlying_.end()) {
            EXPECT_NE(it->second, map.init_);
        }
    }
};

TEST_F(IntervalMapTest, minimal_interface) {
    using Key = KeyInterface<char>;
    using Value = ValueInterface<int>;

    auto map = amby::interval_map<Key, Value>{Value(0)};
    ASSERT_EQ(map[Key(0)], Value(0));
    map.assign(Key(0), Key(1), Value(1));
}

TEST_F(IntervalMapTest, no_insertion) {}

TEST_F(IntervalMapTest, insert_begin_equal_end) {
    assign(0, 0, 1);
}

TEST_F(IntervalMapTest, insert_begin_bigger_than_end) {
    assign(1, 0, 1);
}

TEST_F(IntervalMapTest, insert_one_range) {
    assign(std::numeric_limits<key_type>::min(), 0, 1);
}

TEST_F(IntervalMapTest, insert_non_overlapping_ranges) {
    assign(std::numeric_limits<key_type>::min(), 0, 1);
    assign(10, std::numeric_limits<key_type>::max(), 2);
}

TEST_F(IntervalMapTest, insert_up_to_max) {
    assign(std::numeric_limits<key_type>::min(),
           std::numeric_limits<key_type>::max(), 1);
}

TEST_F(IntervalMapTest, insert_range_right_after) {
    assign(0, 10, 1);
    assign(10, 20, 1);
}

TEST_F(IntervalMapTest, insert_range_right_before) {
    assign(10, 20, 1);
    assign(0, 10, 1);
}

TEST_F(IntervalMapTest, insert_range_middle) {
    assign(0, 10, 1);
    assign(20, 30, 1);
    assign(10, 20, 1);
}

TEST_F(IntervalMapTest, insert_range_inside_another) {
    assign(0, 20, 1);
    assign(5, 15, 2);
}

TEST_F(IntervalMapTest, insert_range_around_another) {
    assign(5, 15, 2);
    assign(0, 20, 1);
}

TEST_F(IntervalMapTest, insert_range_overlaps_many) {
    assign(0, 10, 1);
    assign(10, 20, 2);
    assign(20, 30, 3);
    assign(30, 40, 4);
    assign(40, 50, 5);
    assign(0, 50, -1);
}

TEST_F(IntervalMapTest, insert_range_overlaps_many_init_value) {
    assign(0, 10, 1);
    assign(10, 20, 2);
    assign(20, 30, 3);
    assign(30, 40, 4);
    assign(40, 50, 5);
    assign(0, 50, 0);
}

TEST_F(IntervalMapTest, insert_range_overlaps_many_oversize) {
    assign(0, 10, 1);
    assign(10, 20, 2);
    assign(20, 30, 3);
    assign(30, 40, 4);
    assign(40, 50, 5);
    assign(-10, 60, -1);
}
