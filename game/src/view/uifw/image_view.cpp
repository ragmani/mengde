#include "image_view.h"
#include "view/foundation/texture.h"
#include "view/foundation/drawer.h"
#include "view/foundation/texture_manager.h"

ImageView::ImageView(const Rect* frame, const string& path)
    : ImageView(*frame, path) {
}

ImageView::ImageView(const Rect& frame, const string& path)
    : View(frame), path_(path), texture_(nullptr) {
  bg_color(COLOR("transparent"));
  padding(0);
}

ImageView::~ImageView() {
}

void ImageView::SetPath(const string& path) {
  path_ = path;
  texture_ = nullptr;
}

void ImageView::SetSourceRect(const Rect& rect) {
  src_rect_ = rect;
}

void ImageView::Render(Drawer* drawer) {
  if (texture_ == nullptr) { // TODO better design?
    TextureManager* tm = drawer->GetTextureManager();
    texture_ = tm->FetchTexture(path_);
  }
  ASSERT(texture_ != nullptr);
  Rect frame = *GetFrame();

  if (src_rect_.GetSize() != Vec2D(0, 0))
    drawer->CopyTexture(texture_, &src_rect_, &frame);
  else
    drawer->CopyTexture(texture_, nullptr, &frame);
}

bool ImageView::OnMouseButtonEvent(const MouseButtonEvent e) {
  if (e.IsLeftButtonDown()) {
    LOG_DEBUG("LeftButtonDown on the a ImageView");
    return true;
  }
  return true;
}

bool ImageView::OnMouseMotionEvent(const MouseMotionEvent) {
  return false;
}

