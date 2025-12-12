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

#include <fr/RequirementsManager/UseCase.h>
#include <fr/ImguiWidgets.h>

namespace fr::Imgui {

  class UseCaseWindow : public CommitableNodeWindow {
    static const size_t nameLen = 200;
    char _nameText[nameLen];
    std::string _nameLabel;

    void setNameText() {
      auto node = dynamic_pointer_cast<fr::RequirementsManager::UseCase>(_node);
      if (node) {
        strncpy(_nameText, node->getName().c_str(), nameLen);
      }
    }

  public:
    using Type = UseCaseWindow;
    using PtrType = std::shared_ptr<Type>;
    using Parent = CommitableNodeWindow;

    UseCaseWindow(const std::string &title = "Use Case") : Parent(title) {
      memset(_nameText, '\0', nameLen);
      _nameLabel = getUniqueLabel("##Name");
    }

    virtual ~UseCaseWindow() {}

    void init() override {
      if (!_node) {
        _node = std::make_shared<fr::RequirementsManager::UseCase>();
        _node->init();
      }
      setNameText();
      Parent::init();
    }

    void begin() override {
      Parent::begin();

      auto node = dynamic_pointer_cast<fr::RequirementsManager::UseCase>(_node);
      if (node) {
        auto inputTextFlags = ImGuiInputTextFlags_ReadOnly;
        if (_editable && !node->isCommitted()) {
          inputTextFlags = (ImGuiInputTextFlags_) 0;
        }
      
        ImGui::Text("Name: ");
        ImGui::SameLine();
        if (ImGui::InputText(_nameLabel.c_str(),
                             _nameText,
                             nameLen,
                             inputTextFlags)) {
          node->setName(_nameText);
        }
      } else {
        ImGui::Text("If you're seeing this, this node somehow doesn't have a node");
        ImGui::Text("This should be impossible");
      }
      
    }
  };
  
}
