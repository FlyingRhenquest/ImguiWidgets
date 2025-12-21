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
#include <fr/RequirementsManager/UtilityNodes.h>

namespace fr::Imgui {

  class RoleWindow : public NodeWindow {
  protected:
    static const size_t whoLen = 201;
    char _who[whoLen];
    std::string _whoLabel;

  public:
    using Type = RoleWindow;
    using PtrType = std::shared_ptr<Type>;
    using Parent = NodeWindow;
    using NodeType = fr::RequirementsManager::Role;

    RoleWindow(const std::string &title = "Role") : Parent(title) {
      memset(_who, '\0', whoLen);
      _whoLabel = getUniqueLabel("##Who");
    }

    virtual ~RoleWindow() {}

    void init() override {
      if (!_node) {
        _node = std::make_shared<NodeType>();
        _node->init();
      }
      auto node = dynamic_pointer_cast<NodeType>(_node);
      if (node) {
        strncpy(_who, node->getWho().c_str(), whoLen - 1);
      }
      Parent::init();
    }

    void begin() override {
      Parent::begin();
      auto node = dynamic_pointer_cast<NodeType>(_node);
      if (node) {
        auto inputTextFlags = ImGuiInputTextFlags_ReadOnly;
        if (_editable) {
          inputTextFlags = (ImGuiInputTextFlags_) 0;
        }
        ImGui::Text("Who:");
        ImGui::SameLine();
        if (ImGui::InputText(_whoLabel.c_str(),
                             _who,
                             whoLen - 1,
                             inputTextFlags)) {
          node->setWho(_who);
        }
      } else {
        ImGui::Text("If you're seeing this, this window somehow doesn't have a node");
        ImGui::Text("This should be impossible.");
      }
    }
  };

  namespace Registration {
    template <>
    struct Record<RoleWindow> {
      using Type = RoleWindow;
      using NodeType = RoleWindow::NodeType;

      static constexpr char name[] = "Role";
      static constexpr char topMenuName[] = "Process Nodes";

      static constexpr void init(std::shared_ptr<Type> window) {}
      static constexpr ImVec2 startingSize() { return ImVec2(300, 300); }
      
    };
  }
  
}
