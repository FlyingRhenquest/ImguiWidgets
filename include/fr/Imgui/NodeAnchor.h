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

#include <imgui.h>
#include <fr/Imgui/Window.h>
#include <memory>
#include <unordered_map>

namespace fr::Imgui {

  /**
   * NodeAnchor needs to know what type of anchor it is
   */
  enum class AnchorType {
    UNKNOWN,
    Up, // up list type
    Down, // Down list type
    // Used in CommitableNode
    Left, // left list type
    Right // right list type
  };
  
  /**
   * NodeAnchor displays an anchor that can be used to connect
   * nodes.
   *
   * NodeAnchor uses a draw list and its frame of reference is
   * screen coordinates, so use screenCoordinate when setting
   * up the center value from the parent window.
   */
  
  class NodeAnchor : public Widget {

    // Color the anchor usually is
    ImU32 _color;
    // Color when you hover the mouse over the anchor
    ImU32 _hoverColor;
    // Anchor Type
    AnchorType _type;
    // Center of Anchor
    ImVec2 _center;
    // Anchor radius
    float _radius;
    // Top left of the circle bounding box
    ImVec2 _min;
    // Bounding Box Size
    ImVec2 _bbsize;
    // Anchor hovered flag
    bool _hovered;
    // Parent node
    fr::RequirementsManager::Node::PtrType _node;
    
  public:

    using Type = Widget;
    using PtrType = std::shared_ptr<NodeAnchor>;
    using Parent = Widget;
    
    NodeAnchor(const std::string& label, ImVec2 center, float radius, ImU32 color, ImU32 hoverColor, AnchorType t) :
      Parent(label),
      _color(color),
      _hoverColor(hoverColor),
      _type(t),
      _center(center),
      _radius(radius),
      _min(0,0),
      _bbsize(0,0),
      hovered(false) {
    }

    virtual ~NodeAnchor() {}

    // Try to retrieve node now if possible.
    void setParent(std::shared_ptr<Window> p) override {
      Parent::setParent(p);
      _node = p->getNode();
    }

    void begin() override {
      if (!_node) {
        _node = _parent->getNode();
      }
      // Color to render this time around
      ImU32 color;
      // Set an invisible button to detect hover state
      ImGui::SetCursorScreenPos(_min);
      ImGui::InvisibleButton(_label.c_str(), _bbsize);

      if (ImGui::IsItemHovered()) {
        color = _hoverColor;
        _hovered = true;
      } else {
        color = _color;
        _hovered = false;
      }
      
      ImDrawList *drawList = ImGui::GetWindowDrawList();
      drawList->AddCircleFilled(_center, _radius, color);
    }

    void end() override {
    }

    // (Re)set center coordinate
    void setCenter(ImVec2 center) {
      _center = center;
      _min.x = center.x - _radius;
      _min.y = center.y - _radius;
      _bbsize.x = _radius * 2;
      _bbsize.y = _radius * 2;
    }
    
  };
  
}
