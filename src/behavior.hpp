#ifndef BEHAVIOR_HPP
#define BEHAVIOR_HPP

#include "types.hpp"
#include "util.hpp"

#include <memory>
#include <vector>

class Object;

struct Behavior {
    virtual ~Behavior() = default;
    virtual void update(Seconds dt, Object& object) = 0;
    using CloneResult = std::unique_ptr<Behavior>;
    virtual CloneResult clone() const = 0;
};

struct BehaviorList {
    using List = std::vector<std::unique_ptr<Behavior>>;

    BehaviorList() noexcept = default;
    BehaviorList(List list) : behaviors{std::move(list)} {}

    friend void swap(BehaviorList& first, BehaviorList& second) noexcept {
        using std::swap;
        swap(first.behaviors, second.behaviors);
    }

    BehaviorList(const BehaviorList& other)
        : BehaviorList{deepCopy(other.behaviors)} {}

    BehaviorList& operator=(const BehaviorList& other) {
        BehaviorList temp{other};
        swap(*this, temp);
        return *this;
    }

    BehaviorList(BehaviorList&&) noexcept = default;
    BehaviorList& operator=(BehaviorList&&) noexcept = default;

    util::container_view<List> get() noexcept { return {behaviors}; }

    void push(List::value_type newBehavior) {
        behaviors.push_back(std::move(newBehavior));
    }

   private:
    List behaviors;

    static List deepCopy(const List& list);
};

//

struct DoNothingBehavior : Behavior {
    CloneResult clone() const override {
        return std::make_unique<DoNothingBehavior>();
    }
    void update(Seconds, Object&) override {}
};

struct RotateForeverBehavior : Behavior {
    RotateForeverBehavior(const Euler& r) : rotationPerSecond{r} {}
    Euler rotationPerSecond;
    CloneResult clone() const override;
    void update(Seconds dt, Object& object) override;
};

#endif
