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
#include <fr/RequirementsManager/Organization.h>

namespace fr::Imgui {

  class OrganizationWindow : public NodeWindow {
    static const size_t nameLen = 200;
    char _nameText[nameLen];
    std::string _nameTextLabel;

    void setName() {
      auto node = dynamic_pointer_cast<fr::RequirementsManager::Organization>(_node);
      if (node) {
        strncpy(_nameText, node->getName().c_str(), nameLen);
      }
    }

  public:
    using Type = OrganizationWindow;
    using Parent = NodeWindow;
    using PtrType = std::shared_ptr<Type>;

    OrganizationWindow(std::string title = "Organization")
      : Parent(title) {
      memset(_nameText, '\0', nameLen);
      _nameTextLabel = getUniqueLabel("##Name");
    }

    virtual ~OrganizationWindow() {}

    void init() override {
      if (!_node) {
        _node = std::make_shared<fr::RequirementsManager::Organization>();
      }
      setName();
      Parent::init();
    }

    void begin() override {
      Parent::begin();
      auto node = dynamic_pointer_cast<fr::RequirementsManager::Organization>(_node);
      auto inputTextFlags = ImGuiInputTextFlags_ReadOnly;
      if (node) {
        // Organization can be locked to prevent editing. Have this state
        // track with _editable.
        if (_editable) {
          node->unlock();
          inputTextFlags = (ImGuiInputTextFlags_) 0;
        } else {
          node->lock();          
        }
        // Also force displayEditable to be true for Organizations
        _displayEditable = true;

        ImGui::Text("Name: ");
        ImGui::SameLine();
        if (ImGui::InputText(_nameTextLabel.c_str(), _nameText, nameLen, inputTextFlags)) {
          node->setName(_nameText);
        }
      }
    }
    
  };
  
}
