/*
 * Copyright (C) 2008 by Mark Pustjens <pustjens@dds.nl>
 * Copyright (C) 2010-2012 by CenterIM developers
 *
 * This file is part of CenterIM.
 *
 * CenterIM is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * CenterIM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * @file
 * ColorScheme class.
 *
 * @ingroup cppconsui
 */

#ifndef __COLORSCHEME_H__
#define __COLORSCHEME_H__

#include "ConsuiCurses.h"

#include <map>
#include <string>

#define COLORSCHEME (CppConsUI::ColorScheme::Instance())

namespace CppConsUI
{

class ColorScheme
{
public:
  struct Color
  {
    int foreground;
    int background;
    int attrs;

    Color(int f = Curses::Color::DEFAULT, int b = Curses::Color::DEFAULT,
        int a = Curses::Attr::NORMAL) : foreground(f), background(b)
                        , attrs(a) {}
  };
  static ColorScheme *Instance();

  typedef std::map<std::string, Color> Properties;
  typedef std::map<std::string, Properties> Widgets;
  typedef std::map<std::string, Widgets> Schemes;

  typedef Properties::const_iterator PropertiesIterator;
  typedef Widgets::const_iterator WidgetsIterator;
  typedef Schemes::const_iterator SchemesIterator;

  /**
   * Returns color pair and Curses attributes (that can be passed to
   * Curses::Window::attron()) for a given scheme, widget and property
   * combination.
   */
  int GetColorPair(const char *scheme, const char *widget,
      const char *property);
  int GetColorPair(Color& c);
  /**
   * Sets color pair and Curses attributes for a given scheme, widget,
   * property combination.
   */
  bool SetColorPair(const char *scheme, const char *widget,
      const char *property, int foreground, int background,
      int attrs = Curses::Attr::NORMAL, bool overwrite = false);
  void FreeScheme(const char *scheme);

  const Schemes& GetSchemes() { return schemes; }

protected:

private:
  typedef std::map<std::pair<int, int>, int> ColorPairs;

  Schemes schemes;
  ColorPairs pairs;

  ColorScheme() {}
  ColorScheme(const ColorScheme &);
  ColorScheme &operator=(ColorScheme &);
  ~ColorScheme() {}
};

} // namespace CppConsUI

#endif // __COLORSCHEME_H__

/* vim: set tabstop=2 shiftwidth=2 expandtab : */
