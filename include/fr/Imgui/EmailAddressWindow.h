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

  class EmailAddressWindow : public NodeWindow {
    static const size_t addressLen = 201;
    char _address[addressLen];
    std::string _addressLabel;
    
  public:
    using Type = EmailAddressWindow;
    using PtrType = std::shared_ptr<Type>;
    using Parent = NodeWindow;
    using NodeType = fr::RequirementsManager::EmailAddress;

    EmailAddressWindow(const std::string &title = "Email Address") : Parent(title) {
      memset(_address, '\0', addressLen);
      _addressLabel = getUniqueLabel("##Address");
    }

    virtual ~EmailAddressWindow() {}

    void init() override {
      if (!_node) {
        _node = std::make_shared<NodeType>();
        _node->init();
      }
      auto node = dynamic_pointer_cast<NodeType>(_node);
      if (node) {
        strncpy(_address, node->getAddress().c_str(), addressLen - 1);
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
        ImGui::Text("Email Address: ");
        ImGui::SameLine();
        if (ImGui::InputText(_addressLabel.c_str(),
                             _address,
                             addressLen - 1,
                             inputTextFlags)) {
          node->setAddress(_address);
        }
      } else {
        ImGui::Text("If you're seeing this text, this window somehow doesn't have a node.");
        ImGui::Text("This should be impossible.");
      }
    }    
  };

  namespace Registration {
    template <>
    struct Record<EmailAddressWindow> {
      using Type = EmailAddressWindow;
      using NodeType = EmailAddressWindow::NodeType;

      static constexpr char name[] =  "Email Address";
      static constexpr char topMenuName[] = "Utility Nodes";

      static constexpr void init(std::shared_ptr<Type> window) {}

      static constexpr ImVec2 startingSize() { return ImVec2(400,200); }
    };
  }
  
}
