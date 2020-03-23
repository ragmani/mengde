#include "attribute_modifier_list.h"

#include "attribute_modifier.h"
#include "util/common.h"
#include "util/std/util.h"

namespace mengde {
namespace core {

AttributeModifierList::AttributeModifierList() {}

AttributeModifierList::~AttributeModifierList() {
  for (auto e : elements_) {
    delete e;
  }
}

void AttributeModifierList::AddModifier(AttributeModifier* m) {
  for (int i = 0, sz = elements_.size(); i < sz; i++) {
    AttributeModifier* e = elements_[i];
    if (e->id() == m->id() && e->stat_id() == m->stat_id()) {
      // If both two have the same sign it will be replaced
      // or it will be erased (cancelling out)
      delete e;
      if (e->multiplier() * m->multiplier() >= 0) {
        elements_[i] = m;
      } else {
        elements_.erase(elements_.begin() + i);
        delete m;
      }
      return;
    }
  }
  elements_.push_back(m);
}

void AttributeModifierList::NextTurn() {
  util::std::VectorEraseIf(elements_, [](AttributeModifier* m) -> bool {
    bool remove = (m->turn().left() == 0);
    if (remove) delete m;
    return remove;
  });

  for (auto e : elements_) {
    e->NextTurn();
  }
}

Attribute AttributeModifierList::CalcAddends() const {
  Attribute calc_mods;
  for (auto e : elements_) {
    calc_mods[e->stat_id()] += e->addend();
  }
  return calc_mods;
}

Attribute AttributeModifierList::CalcMultipliers() const {
  Attribute calc_mods;
  for (auto e : elements_) {
    calc_mods[e->stat_id()] += e->multiplier();
  }
  return calc_mods;
}

void AttributeModifierList::iterate(const std::function<void(const AttributeModifier&)>& fn) const {
  for (auto e : elements_) {
    fn(*e);
  }
}

}  // namespace core
}  // namespace mengde
