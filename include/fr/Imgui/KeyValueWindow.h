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

  class KeyValueWindow : public NodeWindow {
    static const size_t keyLen = 200;
    char _keyText[keyLen];
    std::string _keyLabel;
    std::string _value;
    std::string _valueLabel;

    void setKeyValueText() {
      auto node = dynamic_pointer_cast<fr::RequirementsManager::KeyValue>(_node);
      if (node) {
        strncpy(_keyText, node->getKey().c_str(), keyLen);
        _value = node->getValue();
      }
    }

  public:

    using Type = KeyValueWindow;
    using PtrType = std::shared_ptr<Type>;
    using Parent = NodeWindow;
    using NodeType = fr::RequirementsManager::KeyValue;

    KeyValueWindow(const std::string &title = "KeyValue") : Parent(title) {
      memset(_keyText, '\0', keyLen);
      _keyLabel = getUniqueLabel("##Key");
      _valueLabel = getUniqueLabel("##Value");
    }

    virtual ~KeyValueWindow() {}

    void init() override {
      if (!_node) {
        _node = std::make_shared<NodeType>();
        _node->init();
      }
      setKeyValueText();
      Parent::init();
    }

    void begin() override {
      Parent::begin();

      auto node = dynamic_pointer_cast<NodeType>(_node);
      if (node) {
        auto inputTextFlags = ImGuiInputTextFlags_ReadOnly;
        // Do not allow any editing if node is committed
        if (_editable) {
          inputTextFlags = (ImGuiInputTextFlags_) 0;
        }

        ImGui::Text("Key: ");
        ImGui::SameLine();
        if (ImGui::InputText(_keyLabel.c_str(),
                             _keyText,
                             keyLen,
                             inputTextFlags)) {
          node->setKey(_keyText);
        }
        ImGui::Text("Value:");
        if (ImGui::InputTextMultiline(_valueLabel.c_str(),
                                      &_value,
                                      ImVec2(0,0),
                                      inputTextFlags)) {
          node->setValue(_value);
        }
      } else {
        ImGui::Text("If you're seeing this text, this window somehow doesn't have a node.");
        ImGui::Text("This should be impossible.");
      }
    }
    
  };

  namespace Registration {

    template <>
    struct Record<KeyValueWindow> {
      using Type = KeyValueWindow;
      using NodeType = KeyValueWindow::NodeType;

      static constexpr char name[] = "KeyValue";
      static constexpr char topMenuName[] = "Utility Nodes";

      static constexpr void init(std::shared_ptr<Type> window) {}

      static constexpr ImVec2 startingSize() { return ImVec2(300, 350); }
    };
    
  }
  
}
