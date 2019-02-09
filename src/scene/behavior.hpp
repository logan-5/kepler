#ifndef BEHAVIOR_HPP
#define BEHAVIOR_HPP

#include "common/types.hpp"
#include "util/invoke_result.hpp"
#include "util/util.hpp"

#include <memory>
#include <vector>

NS_KEPLER_BEGIN

namespace util {
template <typename T,
          typename Func,
          typename U = invoke_result_t<Func&, const T&>>
std::vector<U> map(const std::vector<T>& vec, Func func) {
    std::vector<U> ret;
    ret.reserve(vec.size());
    std::transform(std::begin(vec), std::end(vec), std::back_inserter(ret),
                   func);
    return ret;
}
}  // namespace util

template <typename ActorType>
struct Behavior {
    virtual ~Behavior() = default;
    virtual void start(ActorType& actor) = 0;
    virtual void update(Seconds dt, ActorType& actor) = 0;
};

// a type-erased list that holds Behaviors that can act on an ActorType or any
// base of ActorType. this allows us to create e.g. a Behavior<Transformed> and
// store it in a BehaviorList<Object> or BehaviorList<Light> or
// BehaviorList<any_other_derived_class_of_Transformed>
template <typename ActorType>
class BehaviorList {
   public:
    struct BehaviorWrapper {
        virtual ~BehaviorWrapper() = default;
        virtual void start(ActorType& actor) = 0;
        virtual void update(Seconds dt, ActorType& actor) = 0;
        virtual std::unique_ptr<BehaviorWrapper> clone() const = 0;
    };

   private:
    template <typename B>
    struct BehaviorWrapper_impl final : BehaviorWrapper {
        void start(ActorType& actor) override { behavior.start(actor); }
        void update(Seconds dt, ActorType& actor) override {
            behavior.update(dt, actor);
        }
        std::unique_ptr<BehaviorWrapper> clone() const override {
            return std::make_unique<BehaviorWrapper_impl>(behavior);
        }
        B behavior;

        template <typename B_>
        BehaviorWrapper_impl(B_&& b) : behavior{std::forward<B_>(b)} {}
    };

    struct ActorTypeGetter {
       private:
        struct nat {};

       public:
        template <typename T>
        static auto test(const Behavior<T>&) -> T;
        static nat test(...);
    };

    template <typename BehaviorA,
              typename A =
                    decltype(ActorTypeGetter::test(std::declval<BehaviorA>()))>
    static std::enable_if_t<std::is_base_of<Behavior<A>, BehaviorA>::value &&
                                  std::is_base_of<A, ActorType>::value,
                            std::unique_ptr<BehaviorWrapper>>
    wrapBehavior(BehaviorA&& b) {
        return std::make_unique<BehaviorWrapper_impl<BehaviorA>>(
              std::forward<BehaviorA>(b));
    }

   public:
    using List = std::vector<std::unique_ptr<BehaviorWrapper>>;

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
    const List& get() const noexcept { return behaviors; }

    template <typename B>
    void push(B&& newBehavior) {
        auto behavior = wrapBehavior(std::forward<B>(newBehavior));
        behaviors.push_back(std::move(behavior));
    }

   private:
    List behaviors;

    static List deepCopy(const List& list) {
        return util::map(list,
                         [](const typename List::value_type& behaviorPtr) {
                             return behaviorPtr->clone();
                         });
    }
};

template <typename T>
struct Actor {
   public:
    using BehaviorList = BehaviorList<T>;

    Actor() : started{false} {}
    virtual ~Actor() = default;

    template <typename B>
    void addBehavior(B&& behavior) {
        behaviors.push(std::forward<B>(behavior));
        if (started) {
            getBehaviors().back()->start(getActor());
        }
    }

    util::container_view<typename BehaviorList::List> getBehaviors() noexcept {
        return behaviors.get();
    }
    const typename BehaviorList::List& getBehaviors() const noexcept {
        return behaviors.get();
    }

    void start() {
        started = true;
        T& actor = getActor();
        for (auto& behavior : getBehaviors())
            behavior->start(actor);
    }

    void update(Seconds dt) {
        T& actor = getActor();
        for (auto& behavior : getBehaviors()) {
            behavior->update(dt, actor);
        }
    }

    virtual T& getActor() = 0;

   protected:
    BehaviorList behaviors;
    bool started;
};

NS_KEPLER_END

#endif
