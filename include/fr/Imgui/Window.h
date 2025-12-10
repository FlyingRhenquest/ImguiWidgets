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

#include <boost/signals2.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <format>
#include <fr/Imgui/WidgetApi.h>
#include <imgui.h>
#include <string>
#include <memory>
#include <unordered_map>

namespace fr::Imgui {

  /**
   * Just a basic, Generic window, that I'm going to build on
   */
  
  class Window : public std::enable_shared_from_this<Window> {
  public:
    using PtrType = std::shared_ptr<Window>;
    
  protected:
    const std::string _label;
    ImVec2 _lastMin;
    // Top left corner in screen coordinates
    ImVec2 _min;
    // Window starting size (width x height)
    ImVec2 _startingSize;
    // Current size (width x height)
    ImVec2 _currentSize;
    // Window color
    ImVec4 _backgroundColor;
    // A map of child windows to display
    std::unordered_map<std::string, Window::PtrType> _children;
    // A map of widgets to display
    std::unordered_map<std::string, std::shared_ptr<WidgetApi>> _widgets;
    // parent window if one exists
    Window::PtrType _parent;
    // Window's begin has been called the first time
    // This is use to set up initialization stuff for imgui
    bool _started;
    // Subscriptions to events
    std::vector<boost::signals2::connection> _subscriptions;

    void setParent(Window::PtrType p) {
      _parent = p;
    }

    // Imgui seems to need to be set with unqiue string lebels for
    // everything, so this will just tag a label with a hidden
    // uuid and return it for such use
    std::string getUniqueLabel(const std::string startingLabel) {
      boost::uuids::uuid id;
      boost::uuids::time_generator_v7 generator;
      id = generator();
      std::string ret = std::format("{}##{}", startingLabel, boost::uuids::to_string(id));
      return ret;
    }
    
  public:

    // Export events as boost::signals2 signals

    // Window resized events (if _currentSize != _startingSize)
    // Params are this window and the new size
    boost::signals2::signal<void(Window::PtrType, ImVec2)> resized;
    // Window moved event
    boost::signals2::signal<void(Window::PtrType, ImVec2)> moved;

    Window(const std::string& label) : _label(getUniqueLabel(label)),
                                       _lastMin(0,0),
                                       _min(0,0),
                                       _startingSize(0,0),
                                       _backgroundColor(0.0,0.0,0.0,1.0),
                                       _started(false) {
    }

    virtual ~Window() {
      unsubscribe();
    }

    // Unsubscribe from all subscriptions
    void unsubscribe() {
      for (auto subscription : _subscriptions) {
        subscription.disconnect();        
      }
      _subscriptions.clear();
    }

    // Add a child window. Key is just some string you can
    // use to retrieve the window later.
    virtual void add(const std::string& key, Window::PtrType child) {
      if (!_children.contains(key)) {
        _children[key] = child;
        child->setParent(shared_from_this());
      } else {
        std::string err = std::format("Window {} already exists in child list", key);
        throw std::logic_error("err");
      }
    }

    // Add a widget. Key can be used to retrieve the widget
    // later.
    virtual void addWidget(const std::string& key, std::shared_ptr<WidgetApi> w) {
      if (!_widgets.contains(key)) {
        _widgets[key] = w;
        w->setParent(shared_from_this());
      } else {
        std::string err = std::format("Widget {} is already owned by Window {}", key, _label);
        throw std::logic_error(err);
      }
    }

    // Retrieve a child window from the children list. Can return
    // a null shared ptr.
    Window::PtrType get(const std::string& key) {
      Window::PtrType ret;
      if (_children.contains(key)) {
        ret = _children[key];
      }
      return ret;
    }

    // Remove a child window from the children list
    void remove(const std::string& key) {
      if (_children.contains(key)) {
        _children.erase(key);
      }
    }

    // Returns true if the children list contains the key
    bool contains(std::string& key) {
      return _children.contains(key);
    }

    /**
     * Return screen coordinates from window coordinates
     */
    ImVec2 screenCoordinate(ImVec2 coordinate) {
      return ImVec2(coordinate.x + _min.x, coordinate.y + _min.y);
    }

    ImVec2 screenCoordinate(float x, float y) {
      return ImVec2(x + _min.x, y + _min.y);
    }

    void setStartingSize(int width, int height) {
      _startingSize.x = width;
      _startingSize.y = height;
    }

    void setBackgroundColor(float r, float g, float b, float alpha) {
      _backgroundColor.x = r;
      _backgroundColor.y = g;
      _backgroundColor.z = b;
      _backgroundColor.w = alpha;
    }

    virtual void renderChildren() {
      for (auto [key, child] : _children) {
        child->begin();
        child->end();
      }
      for (auto [key, widget] : _widgets) {
        widget->begin();
        widget->end();
      }
    }

    virtual void beginning() {
      _started = true;
      ImGui::SetNextWindowSize(_startingSize);
    }
    
    virtual void begin() {
      if (!_started) {
        beginning();
      }
      ImGui::PushStyleColor(ImGuiCol_WindowBg, _backgroundColor);
      ImGui::Begin(_label.c_str());
      _min = ImGui::GetWindowPos();
      if ((_min.x != _lastMin.x) || (_min.y != _lastMin.y)) {
        moved(shared_from_this(), _min);
      }
      _currentSize = ImGui::GetContentRegionAvail();
      if ((_currentSize.x != _startingSize.x) || (_currentSize.y != _startingSize.y)) {
        resized(shared_from_this(), _currentSize);
        _startingSize = _currentSize;
      }
      renderChildren();
    }

    virtual void end() {
      ImGui::PopStyleColor();
      ImGui::End();
    }
  };
  
}
