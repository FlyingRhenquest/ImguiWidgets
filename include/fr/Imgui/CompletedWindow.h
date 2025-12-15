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
#include <imgui_stdlib.h>

namespace fr::Imgui {

  class CompletedWindow : public NodeWindow {
  protected:
    std::string _description;
    std::string _descriptionLabel;

  public:
    using Type = CompletedWindow;
    using PtrType = std::shared_ptr<Type>;
    using Parent = NodeWindow;
    using NodeType = fr::RequirementsManager::Completed;

    CompletedWindow(const std::string &title = "Completed") : Parent(title) {
      _descriptionLabel = getUniqueLabel("##Description");
    }

    virtual ~CompletedWindow() {}

    void init() override {
      if (!_node) {
        _node = std::make_shared<NodeType>();
        _node->init();
      }

      auto node = dynamic_pointer_cast<NodeType>(_node);
      if (node) {
        _description = node->getDescription();
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
        ImGui::Text("Description:");
        if (ImGui::InputTextMultiline(_descriptionLabel.c_str(),
                                      &_description,
                                      ImVec2(0,0),
                                      inputTextFlags)) {
          node->setDescription(_description);
        }
      } else {
                ImGui::Text("If you're seeing this, this window somehow doesn't have a node");
        ImGui::Text("This should be impossible.");
      }
    }
    
  };

  namespace Registration {

    template <>
    struct Record<CompletedWindow> {

      using Type = CompletedWindow;
      using NodeType = CompletedWindow::NodeType;

      static constexpr char name[] = "Completed";
      static constexpr char topMenuName[] = "Utility Nodes";

      static constexpr void init(std::shared_ptr<Type> window) {}
      static constexpr ImVec2 startingSize() { return ImVec2(300,300); }
      
    };

  }
  
}
