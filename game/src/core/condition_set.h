#ifndef MENGDE_CORE_CONDITION_SET_H_
#define MENGDE_CORE_CONDITION_SET_H_

#include <unordered_map>

#include "condition.h"
#include "turn_based.h"

namespace mengde {
namespace core {

class ConditionSet {
 public:
  ConditionSet() = default;

 public:
  bool Has(Condition condition) const;
  TurnBased Get(Condition condition) const;
  void Set(Condition condition, const TurnBased& turn_based);
  void NextTurn();
  void Iterate(const std::function<void(Condition, TurnBased)>& fn) const;

 private:
  std::unordered_map<Condition, TurnBased> set_;
};

}  // namespace core
}  // namespace mengde

#endif  // MENGDE_CORE_CONDITION_SET_H_
