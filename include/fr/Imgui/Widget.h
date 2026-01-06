/**
* Copyright 2025 Bruce Ide
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.

 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <imgui.h>
#include <fr/Imgui/WidgetApi.h>
#include <fr/Imgui/Window.h>

namespace fr::Imgui {

  /**
   * A widget is a non-window UI element that renders
   * some sort of data.
   */
  
  class Widget : public WidgetApi {
  public:
    using Type = Widget;
    using PtrType = std::shared_ptr<Widget>;

  protected:
    // Parent window if one exists
    std::shared_ptr<Window> _parent;
    // Label for this widget. This will potentially
    // be handy if I want to surface it in ImGui.
    const std::string _label;
    
    std::string getUniqueLabel(const std::string startingLabel) {
      boost::uuids::uuid id;
      boost::uuids::time_generator_v7 generator;
      id = generator();
      std::string ret = std::format("{}##{}", startingLabel, boost::uuids::to_string(id));
      return ret;
    }

  public:

    Widget(const std::string label = "") : _label(getUniqueLabel(label)) {
    }
    
    virtual ~Widget() {}

    void setParent(std::shared_ptr<Window> p) override {
      _parent = p;
    }

    std::string getLabel() override {
      return _label;
    }

    virtual void begin() override {
    }

    virtual void end() override {
    }
    
  };
    
}
