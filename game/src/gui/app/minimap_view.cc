#include "minimap_view.h"

#include "core/stage.h"
#include "gui/uifw/drawer.h"
#include "gui/uifw/image_view.h"
#include "gui/uifw/layout_helper.h"
#include "gui/uifw/rect_view.h"
#include "gui/uifw/view.h"
#include "resource_path.h"

namespace mengde {
namespace gui {
namespace app {

MinimapView::MinimapView(const Rect& frame, core::Stage* game, Vec2D* camera_coords_ptr, Vec2D camera_size,
                         Vec2D map_size)
    : CompositeView(frame), camera_coords_ptr_(camera_coords_ptr), camera_size_(camera_size), map_size_(map_size) {
  padding(0);

  Rect bg_frame = GetActualFrame();
  bg_image_view_ = new ImageView(bg_frame, rcpath::MapPath(game->GetMapId()).ToString());
  AddChild(bg_image_view_);

  mini_units_view_ = new MinimapUnitsView(bg_frame, game, map_size);
  AddChild(mini_units_view_);

  Vec2D frame_size = GetFrameSize();
  Vec2D camera_area_size = frame_size * camera_size / map_size;
  Rect rect_frame(MagnifyForMinimap(*camera_coords_ptr), camera_area_size);
  camera_rect_view_ = new RectView(rect_frame, COLOR("transparent"));
  camera_rect_view_->SetBorder(2, COLOR("white"));
  AddChild(camera_rect_view_);
}

void MinimapView::Update() { camera_rect_view_->SetCoords(MagnifyForMinimap(*camera_coords_ptr_)); }

Vec2D MinimapView::MagnifyForMinimap(Vec2D v) { return v * GetFrameSize() / map_size_; }

MinimapUnitsView::MinimapUnitsView(const Rect& frame, core::Stage* game, Vec2D map_size) : View(frame), stage_(game) {
  unit_size_ = {48, 48};
  unit_size_ = unit_size_ * GetFrameSize() / map_size;
}

void MinimapUnitsView::Render(Drawer* drawer) {
  stage_->ForEachUnit([=](core::Unit* u) {
    switch (u->force()) {
      case core::Force::kOwn:
        drawer->SetDrawColor({192, 0, 0, 255});
        break;
      case core::Force::kAlly:
        drawer->SetDrawColor(COLOR("orange"));
        break;
      case core::Force::kEnemy:
        drawer->SetDrawColor(COLOR("blue"));
        break;
      default:
        LOG_FATAL("Unknown force for a unit.");
        drawer->SetDrawColor(COLOR("gray"));
        break;
    }

    Rect rect(u->position(), {1, 1});
    rect.Magnify(unit_size_);
    rect -= 1;
    drawer->FillRect(rect);
  });
}

}  // namespace app
}  // namespace gui
}  // namespace mengde
