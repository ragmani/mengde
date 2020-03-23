#ifndef MENGDE_GUI_APP_MAGIC_LIST_VIEW_H_
#define MENGDE_GUI_APP_MAGIC_LIST_VIEW_H_

#include <boost/optional.hpp>
#include "core/id.h"
#include "gui/uifw/composite_view.h"
#include "util/common.h"

#include "common.h"

namespace mengde {
namespace core {

class Stage;
class Unit;
class UserInterface;
class MagicList;

}  // namespace core
}  // namespace mengde

namespace mengde {
namespace gui {
namespace app {

class GameView;

class MagicListView : public CompositeView {
 public:
  static const int kTitleHeight = 24;

 public:
  MagicListView(const Rect&, core::Stage*, core::UserInterface*, GameView*);
  ~MagicListView();

  void SetData(const core::UnitKey& ukey, const core::MoveKey& mkey, shared_ptr<core::MagicList>);

  virtual bool OnMouseButtonEvent(const foundation::MouseButtonEvent&) override;

 private:
  void Cleanup();

 private:
  core::Stage* stage_;
  core::UserInterface* gi_;
  GameView* gv_;

 private:
  int item_height_;

 private:
  TextView* tv_title_;
  VerticalListView* lv_magics_;
  View* lv_magics_wrap_;
};

}  // namespace app
}  // namespace gui
}  // namespace mengde

#endif  // MENGDE_GUI_APP_MAGIC_LIST_VIEW_H_
