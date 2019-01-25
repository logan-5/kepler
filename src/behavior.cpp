#include "behavior.hpp"

#include "object.hpp"

auto BehaviorList::deepCopy(const List& list) -> List {
    List copy;
    copy.reserve(list.size());
    std::transform(std::begin(list), std::end(list), std::back_inserter(copy),
                   [](const List::value_type& behaviorPtr) {
                       return behaviorPtr->clone();
                   });
    return copy;
}

auto RotateForeverBehavior::clone() const -> CloneResult {
    return std::make_unique<RotateForeverBehavior>(*this);
}
void RotateForeverBehavior::update(Seconds dt, Object& object) {
    object.transform().angle.rep() += this->rotationPerSecond.rep() * dt.rep();
}
