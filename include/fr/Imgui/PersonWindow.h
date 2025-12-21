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

  class PersonWindow : public NodeWindow {
    // Both name lens
    static const size_t nameLen = 201;
    std::string _firstNameLabel;
    char _firstName[nameLen];
    std::string _lastNameLabel;
    char _lastName[nameLen];

  public:
    using Type = PersonWindow;
    using PtrType = std::shared_ptr<PtrType>;
    using Parent = NodeWindow;
    using NodeType = fr::RequirementsManager::Person;

    PersonWindow(const std::string &title = "Person") : Parent(title) {
      memset(_firstName, '\0', nameLen);
      memset(_lastName, '\0', nameLen);
      _firstNameLabel = getUniqueLabel("##FirstName");
      _lastNameLabel = getUniqueLabel("##LastName");
    }

    virtual ~PersonWindow() {}

    void init() override {
      if (!_node) {
        _node = std::make_shared<NodeType>();
        _node->init();
      }

      auto node = dynamic_pointer_cast<NodeType>(_node);
      if (node) {
        strncpy(_firstName, node->getFirstName().c_str(), nameLen - 1);
        strncpy(_lastName, node->getLastName().c_str(), nameLen - 1);
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
        ImGui::Text("First Name: ");
        ImGui::SameLine();
        if (ImGui::InputText(_firstNameLabel.c_str(),
                             _firstName,
                             nameLen - 1,
                             inputTextFlags)) {
          node->setFirstName(_firstName);
        }
        ImGui::Text("Last Name: ");
        ImGui::SameLine();
        if (ImGui::InputText(_lastNameLabel.c_str(),
                             _lastName,
                             nameLen - 1,
                             inputTextFlags)) {
          node->setLastName(_lastName);
        }
      } else {
        ImGui::Text("If you're seeing this text, this window somehow doesn't have a node.");
        ImGui::Text("This should be impossible.");
      }
    }
  };

  namespace Registration {

    template <>
    struct Record<PersonWindow> {
      using Type = PersonWindow;
      using NodeType = PersonWindow::NodeType;

      static constexpr char name[] = "Person";
      static constexpr char topMenuName[] = "Utility Nodes";

      static constexpr void init(std::shared_ptr<Type> window) {}
      static constexpr ImVec2 startingSize() { return ImVec2(400,200); }
    };
    
  }
  
}
