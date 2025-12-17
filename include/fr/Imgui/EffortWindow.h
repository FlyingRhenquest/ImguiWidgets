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

  class EffortWindow : public NodeWindow {
  protected:
    std::string _text;
    std::string _textLabel;
    // We'll convert effort from hours
    int _effort;
    std::string _effortLabel;

  public:
    using Type = EffortWindow;
    using PtrType = std::shared_ptr<Type>;
    using Parent = NodeWindow;
    using NodeType = fr::RequirementsManager::Effort;

    EffortWindow(const std::string &title = "Effort") : Parent(title) {
      _textLabel = getUniqueLabel("##Text");
      _effortLabel = getUniqueLabel("##Effort");
      _effort = 0;
    }

    virtual ~EffortWindow() {}

    void init() override {
      if (!_node) {
        _node = std::make_shared<NodeType>();
        _node->init();
      }
      auto node = dynamic_pointer_cast<NodeType>(_node);
      if (node) {
        _text = node->getText();
        // 3600 seconds in an hour
        _effort = node->getEffort() / 3600;       
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
      
        ImGui::Text("Effort (hours):");
        ImGui::SameLine();
        if (ImGui::InputInt(_effortLabel.c_str(),
                            &_effort,
                            1,
                            5,
                            inputTextFlags)) {
          int effort = _effort * 3600;
          node->setEffort(effort);
        }
        
        ImGui::Text("Text:");
        if (ImGui::InputTextMultiline(_textLabel.c_str(),
                                      &_text,
                                      ImVec2(0,0),
                                      inputTextFlags)) {
          node->setText(_text);
        }
      } else {
        ImGui::Text("If you're seeing this, this window somehow doesn't have a node.");
        ImGui::Text("This should be impossible.");
      }
      
    }
    
  };

  namespace Registration {

    template <>
    struct Record<EffortWindow> {
      using Type = EffortWindow;
      using NodeType = EffortWindow::NodeType;

      static constexpr char name[] = "Effort";
      static constexpr char topMenuName[] = "Utility Nodes";

      static constexpr void init(std::shared_ptr<Type> window) {}

      static constexpr ImVec2 startingSize() { return ImVec2(350,320); }
      
    };
    
  }
  
}
