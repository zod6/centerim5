// Copyright (C) 2010-2015 Petr Pavlu <setup@dagobah.cz>
//
// This file is part of CenterIM.
//
// CenterIM is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// CenterIM is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/// @file
/// Window class.
///
/// @ingroup cppconsui

#ifndef WINDOW_H
#define WINDOW_H

#include "Container.h"
#include "Panel.h"

namespace CppConsUI {

class Window : public Container {
public:
  enum Type {
    TYPE_NON_FOCUSABLE,
    TYPE_NORMAL,
    TYPE_TOP,
  };

  Window(
    int x, int y, int w, int h, Type t = TYPE_NORMAL, bool decorated_ = true);
  Window(int x, int y, int w, int h, const char *title, Type t = TYPE_NORMAL,
    bool decorated_ = true);
  virtual ~Window();

  // Widget
  virtual void moveResizeRect(const Rect &rect)
  {
    moveResize(rect.x, rect.y, rect.width, rect.height);
  }
  virtual int draw(Curses::ViewPort area, Error &error);
  virtual void setVisibility(bool visible);
  virtual bool isVisibleRecursive() const { return isVisible(); }
  virtual Point getAbsolutePosition();

  // Container
  virtual bool isWidgetVisible(const Widget &widget) const;
  virtual bool setFocusChild(Widget &child);
  virtual Point getAbsolutePosition(const Widget &child) const;

  virtual void show();
  virtual void hide();
  virtual void close();

  // virtual void SetType(Type t) { type_ = t; }
  virtual Type getType() { return type_; }

  // virtual void setDecorated(bool new_decorated);
  virtual bool isDecorated() const { return decorated_; }

  virtual void setClosable(bool new_closable);
  virtual bool isClosable() const { return closable_; }

  virtual void setTitle(const char *text) { panel_->setTitle(text); }
  virtual const char *getTitle() const { return panel_->getTitle(); }

  /// This function is called when the screen is resized.
  virtual void onScreenResized() {}

  sigc::signal<void, Window &> signal_close;
  sigc::signal<void, Window &> signal_show;
  sigc::signal<void, Window &> signal_hide;

protected:
  Type type_;
  bool decorated_;

  /// Flag indicating if it is allowed to close the window by the close-window
  /// action, this usually means if it is possible to close the window by
  /// pressing the Esc key.
  bool closable_;

  Panel *panel_;

  // Widget
  virtual void signalMoveResize(const Rect &oldsize, const Rect &newsize);
  virtual void signalWishSizeChange(const Size &oldsize, const Size &newsize);
  virtual void updateArea();
  virtual void redraw();

  virtual void initWindow(int x, int y, const char *title);

private:
  CONSUI_DISABLE_COPY(Window);

  // Widget
  // Windows cannot have any parent.
  using Container::setParent;

  void actionClose();

  void declareBindables();
};

} // namespace CppConsUI

#endif // WINDOW_H

// vim: set tabstop=2 shiftwidth=2 textwidth=80 expandtab:
