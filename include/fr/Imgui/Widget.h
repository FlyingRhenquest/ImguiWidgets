/**
 * Copyright 2025 Bruce Ide
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#pragma once

#include <boost/signals2.hpp>
#include <imgui.h>
#include <fr/Imgui/Window.h>

namespace fr::Imgui {

  class Window;
  /**
   * A widget is a non-window UI element that renders
   * some sort of data.
   */
  
  class Widget {
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

    virtual void setParent(std::shared_ptr<Window> p) {
      _parent = p;
    }

    std::string getLabel() {
      return _label;
    }

    virtual void begin() {
    }

    virtual void end() {
    }
    
  };
    
}
