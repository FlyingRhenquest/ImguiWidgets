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

  class TextWindow : public NodeWindow {
  protected:
    std::string _text;
    std::string _textLabel;
    
  public:
    using Type = TextWindow;
    using PtrType = std::shared_ptr<TextWindow>;
    using Parent = NodeWindow;
    using NodeType = fr::RequirementsManager::Text;

    TextWindow(const std::string &title = "Text") : Parent(title) {
      _textLabel = getUniqueLabel("Text");
    }

    virtual ~TextWindow() {}

    void init() override {
      if (!_node) {
        _node = std::make_shared<fr::RequirementsManager::Text>();
        _node->init();
      }
      auto node = dynamic_pointer_cast<fr::RequirementsManager::Text>(_node);
      if (node) {
        _text = node->getText();
      }

      Parent::init();
    }

    void begin() override {
      Parent::begin();

      auto node = dynamic_pointer_cast<fr::RequirementsManager::Text>(_node);
      if (node) {
        auto inputTextFlags = ImGuiInputTextFlags_ReadOnly;
        if (_editable) {
          inputTextFlags = (ImGuiInputTextFlags_) 0;
        }
        ImGui::Text("Text:");
        if (ImGui::InputTextMultiline(_textLabel.c_str(),
                                      &_text,
                                      ImVec2(0,0),
                                      inputTextFlags)) {
          node->setText(_text);
        }
      } else {
        ImGui::Text("If you're seeing this, this node somehow doesn't have a node");
        ImGui::Text("This should be impossible.");
      }
    }
    
  };

  namespace Registration {

    template <>
    struct Record<TextWindow> {
      using Type = TextWindow;
      using NodeType = TextWindow::NodeType;

      static constexpr char name[] = "Text";
      static constexpr char topMenuName[] = "Utility Nodes";

      static constexpr void init(std::shared_ptr<Type> window) {}

      static constexpr ImVec2 startingSize() { return ImVec2(300,300); }
      
    };
    
  }
  
}
