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

#include <fr/Imgui/NodeWindow.h>
#include <fr/RequirementsManager/GraphNode.h>

namespace fr::Imgui {

  class GraphNodeWindow : public NodeWindow {
    static const size_t titleTextLen = 200;
    char _titleText[titleTextLen];
    std::string _titleTextLabel;
    
    void setTitleText() {
      auto node = dynamic_pointer_cast<fr::RequirementsManager::GraphNode>(_node);
      if (node) {
        strncpy(_titleText, node->getTitle().c_str(), titleTextLen);
      }
    }

  public:

    using Type = GraphNodeWindow;
    using Parent = NodeWindow;
    using PtrType = std::shared_ptr<Type>;

    GraphNodeWindow(std::string title = "GraphNode")
      : Parent(title) {
      memset(_titleText, '\0', titleTextLen);
      _titleTextLabel = getUniqueLabel("##Title");
    }

    virtual ~GraphNodeWindow() {}

    void init() override {
      if (!_node) {
        _node = std::make_shared<fr::RequirementsManager::GraphNode>();
      }
      setTitleText();
      Parent::init();
    }

    void begin() override {
      Parent::begin();
      auto inputTextFlags = ImGuiInputTextFlags_ReadOnly;
      if (_editable) {
        inputTextFlags = (ImGuiInputTextFlags_) 0;
      }

      ImGui::Text("Title: ");
      ImGui::SameLine();
      if (ImGui::InputText(_titleTextLabel.c_str(), _titleText, titleTextLen, inputTextFlags)) {
        auto node = dynamic_pointer_cast<fr::RequirementsManager::GraphNode>(_node);
        if (node) {
          node->setTitle(_titleText);
        }
      }
    }
    
  };
  
}
