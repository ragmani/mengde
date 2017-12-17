#include "text_view.h"
#include "../drawer.h"

TextView::TextView(const Rect* frame, const std::string& text)
    : TextView(frame, text, COLOR("white"), 14, LayoutHelper::kAlignLftTop) {
}

TextView::TextView(const Rect* frame, const std::string& text, Color color, int size, LayoutHelper::Align align)
    : View(frame), text_(text), color_(color), size_(size), align_(align) {
  SetBgColor(COLOR("transparent"));
  SetPadding(0);
}

void TextView::SetText(const std::string& text) {
  text_ = text;
}

void TextView::SetAlign(LayoutHelper::Align align) {
  align_ = align;
}

void TextView::SetColor(Color c) {
  color_ = c;
}

void TextView::Render(Drawer* drawer) {
  Rect frame(GetFrameCoords(), GetActualFrameSize());
  drawer->DrawText(text_, size_, color_, &frame, align_);
}

bool TextView::OnMouseButtonEvent(const MouseButtonEvent e) {
  if (e.IsLeftButtonDown()) {
    LOG_DEBUG("LeftButtonDown on the TextView '%s'", text_.c_str());
    return true;
  }
  return true;
}

bool TextView::OnMouseMotionEvent(const MouseMotionEvent e) {
  return false;
}

