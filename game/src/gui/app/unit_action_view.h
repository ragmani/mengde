#ifndef MENGDE_GUI_APP_UNIT_ACTION_VIEW_H_
#define MENGDE_GUI_APP_UNIT_ACTION_VIEW_H_

#include <boost/optional.hpp>

#include "core/id.h"
#include "gui/uifw/vertical_list_view.h"

#include "common.h"

namespace mengde {
namespace core {
class Stage;
class UserInterface;
class Unit;
}  // namespace core
}  // namespace mengde

namespace mengde {
namespace gui {
namespace app {

class GameView;

class UnitActionView : public VerticalListView {
 public:
  UnitActionView(const Rect& frame, core::UserInterface*, GameView* gv);
  void SetUnitAndMoveKey(const core::UnitKey& ukey, const core::MoveKey& mkey);
  virtual bool OnMouseButtonEvent(const foundation::MouseButtonEvent&) override;

 private:
  core::UserInterface* gi_;
  GameView* gv_;
  ButtonView* btn_attack_;
  ButtonView* btn_magic_;
  ButtonView* btn_stay_;
};

}  // namespace app
}  // namespace gui
}  // namespace mengde

#endif  // MENGDE_GUI_APP_UNIT_ACTION_VIEW_H_
