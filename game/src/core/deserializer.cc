#include "deserializer.h"

#include "scenario_generated.h"
#include "util/common.h"

#include <fstream>

namespace mengde {
namespace core {

Deserializer::Deserializer(const Path& path) : path_{path} {}

unique_ptr<Scenario> Deserializer::Deserialize() {
  std::ifstream file(path_.ToString(), std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(size);
  file.read(buffer.data(), size);

  auto sce = save::GetScenario(buffer.data());

  auto assets = Build(*sce->resource_managers());

  (void)assets;

  return nullptr;
}

ResourceManagers Deserializer::Build(const save::ResourceManagers& rm) {
  ResourceManagers ret;
  ret.terrain_manager = Build(*rm.terrain_manager());
  ret.hero_class_manager = Build(*rm.hero_class_manager());
  ret.magic_manager = Build(*rm.magic_manager());
  ret.equipment_manager = Build(*rm.equipment_manager());
  ret.hero_tpl_manager = Build(*rm.hero_tpl_manager(), *ret.hero_class_manager);
  return ret;
}

TerrainManager* Deserializer::Build(const flatbuffers::Vector<flatbuffers::Offset<save::Terrain>>& tm) {
  auto ret = new TerrainManager;
  for (uint32_t i = 0; i < tm.Length(); ++i) {
    auto t = tm.Get(i);
    auto id = t->id()->str();
    vector<int32_t> move_costs;
    for (auto cost : *t->move_costs()) {
      move_costs.push_back(cost);
    }
    vector<int32_t> effects;
    for (auto cost : *t->class_effects()) {
      effects.push_back(cost);
    }

    auto obj = new Terrain{id, move_costs, effects};
    ret->Add(id, obj);
  }
  return ret;
}

HeroClassManager* Deserializer::Build(const flatbuffers::Vector<flatbuffers::Offset<save::HeroClass>>& hcm) {
  auto ret = new HeroClassManager;
  for (uint32_t i = 0; i < hcm.Length(); ++i) {
    auto hc = hcm.Get(i);
    auto id = hc->id()->str();
    auto stat_grade = BuildStruct<Attribute>(*hc->attr_grade());
    auto range = static_cast<Range::Type>(hc->attack_range());
    auto bi_hp = BuildStruct<BaseAndIncr>(*hc->bi_hp());
    auto bi_mp = BuildStruct<BaseAndIncr>(*hc->bi_mp());
    boost::optional<PromotionInfo> promotion_info;
    if (hc->promotion_info()) {
      promotion_info = PromotionInfo{hc->promotion_info()->hero_class_id()->str(), hc->promotion_info()->level()};
    }

    auto hc_obj = new HeroClass{id, i, stat_grade, range, hc->move(), bi_hp, bi_mp, promotion_info};
    ret->Add(id, hc_obj);
  }
  return ret;
}

MagicManager* Deserializer::Build(const flatbuffers::Vector<flatbuffers::Offset<save::Magic>>& mm) {
  auto ret = new MagicManager;
  for (auto magic : mm) {
    auto id = magic->id()->str();
    auto range = magic->range();
    auto learn_info_list = magic->learn_info_list();
    auto target_enemy = magic->target_enemy();
    auto mp_cost = magic->mp_cost();
    auto effects = magic->effects();
    auto obj = new Magic{id, static_cast<Range::Type>(range), target_enemy, mp_cost};
    for (auto li : *learn_info_list) {
      obj->AddLearnInfo(li->id(), li->lv());
    }
    for (auto effect : *effects) {
      obj->AddEffect(std::unique_ptr<MagicEffect>(Build(*effect)));
    }
    ret->Add(id, obj);
  }
  return ret;
}

EquipmentManager* Deserializer::Build(const flatbuffers::Vector<flatbuffers::Offset<save::Equipment>>& em) {
  auto ret = new EquipmentManager;
  for (auto eq : em) {
    auto id = eq->id()->str();
    auto type = static_cast<Equipment::Type>(eq->type());
    auto va = Build(*eq->volatile_attributes());
    auto obj = new Equipment{id, type, unique_ptr<VolatileAttribute>{va}};
    ret->Add(id, obj);
  }
  return ret;
}

VolatileAttribute* Deserializer::Build(const save::VolatileAttributes& va) {
  AttributeModifierList aml;
  for (auto e : *va.attribute_modifier_list()) {
    aml.AddModifier(new AttributeModifier{Build(*e)});  // NOTE this does redundant copy
  }

  EventEffectList eel;
  for (auto e : *va.event_effect_list()) {
    eel.Add(Build(*e));
  }

  return new VolatileAttribute{aml, eel};
}

EventEffectBase* Deserializer::Build(const save::EventEffect& ee) {
  EventEffectBase* ret = nullptr;

  if (ee.instance_type() == save::EventEffectImpl::GeneralEventEffect) {
    auto gee = ee.instance_as_GeneralEventEffect();
    auto event = static_cast<event::GeneralEvent>(gee->type());

    switch (gee->instance_type()) {
      case save::GeneralEventEffectImpl::NONE: {
        throw std::runtime_error{"Invalid value for GeneralEventEffectImpl"};
      }
      case save::GeneralEventEffectImpl::GEERestoreHp: {
        auto ree = gee->instance_as_GEERestoreHp();
        ret = new GEERestoreHp{event, BuildStruct<AttributeChange>(*ree->change())};
        break;
      }
      default:
        throw std::runtime_error{"Invalid value GeneralEventEffectImpl"};
    }
  } else {
    assert(ee.instance_type() == save::EventEffectImpl::OnCmdEventEffect);
    auto ocee = ee.instance_as_OnCmdEventEffect();
    auto event = static_cast<event::OnCmdEvent>(ocee->type());

    switch (ocee->instance_type()) {
      case save::OnCmdEventEffectImpl::NONE: {
        throw std::runtime_error{"Invalid value for OnCmdEventEffectImpl"};
        break;
      }
      case save::OnCmdEventEffectImpl::OCEEPreemptiveAttack: {
        ret = new OCEEPreemptiveAttack{event};
        break;
      }
      case save::OnCmdEventEffectImpl::OCEEEnhanceBasicAttack: {
        auto inst = ocee->instance_as_OCEEEnhanceBasicAttack();
        ret = new OCEEEnhanceBasicAttack{event, BuildStruct<AttributeChange>(*inst->change())};
        break;
      }
      default:
        throw std::runtime_error{"Invalid value for OnCmdEventEffectImpl"};
    }
  }

  return ret;
}

HeroTemplateManager* Deserializer::Build(const flatbuffers::Vector<flatbuffers::Offset<save::HeroTemplate>>& htm,
                                         const HeroClassManager& hcm) {
  auto ret = new HeroTemplateManager;
  for (auto ht : htm) {
    auto id = ht->id()->str();
    auto hero_class_id = ht->hero_class()->str();
    auto attr = BuildStruct<Attribute>(*ht->attr());
    auto obj = new HeroTemplate{id, hcm.Get(hero_class_id), attr};
    ret->Add(id, obj);
  }
  return ret;
}

MagicEffect* Deserializer::Build(const save::MagicEffect& me) {
  //  auto turn = me.type();
  switch (me.instance_type()) {
    case save::MagicEffectImpl::MagicEffectHp: {
      auto inst = me.instance_as_MagicEffectHp();
      return new MagicEffectHP{inst->power()};
    }
    case save::MagicEffectImpl::MagicEffectAttribute: {
      auto inst = me.instance_as_MagicEffectAttribute();
      auto attr_mod = Build(*inst->attribute_modifier());
      return new MagicEffectStat{attr_mod};
    }
    case save::MagicEffectImpl::MagicEffectCondition: {
      auto inst = me.instance_as_MagicEffectCondition();
      auto cond = static_cast<Condition>(inst->condition());
      auto turn = TurnBased{inst->turn()->turns()};
      return new MagicEffectCondition{cond, turn};
    }
    default:
      throw std::runtime_error{"Invalid MagicEffect Type"};
  }
}

AttributeModifier Deserializer::Build(const save::AttributeModifier& mod) {
  auto id = mod.id()->str();
  auto stat_idx = mod.stat_idx();
  auto turn = TurnBased{mod.turn()->turns()};
  auto change = BuildStruct<AttributeChange>(*mod.change());
  return AttributeModifier{id, stat_idx, change, turn};
}

}  // namespace core
}  // namespace mengde
