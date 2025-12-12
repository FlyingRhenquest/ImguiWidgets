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
#include <fr/RequirementsManager/Project.h>
#include <imgui_stdlib.h>

namespace fr::Imgui {

  class ProjectWindow : public NodeWindow {
    static const size_t nameLen = 200;
    char _nameText[nameLen];
    std::string _nameLabel;
    std::string _descriptionLabel;
    std::string _description;

    void setNameText() {
      auto node = dynamic_pointer_cast<fr::RequirementsManager::Project>(_node);
      if (node) {
        strncpy(_nameText, node->getName().c_str(), nameLen);
      }
    }

  public:
    using Type = ProjectWindow;
    using PtrType = std::shared_ptr<Type>;
    using Parent = NodeWindow;

    ProjectWindow(std::string title = "Project") : Parent(title) {
      memset(_nameText, '\0', nameLen);
      _nameLabel = getUniqueLabel("##Name");
      _descriptionLabel = getUniqueLabel("##Description");      
    }

    virtual ~ProjectWindow() {};

    void init() override {
      if (!_node) {
        _node = std::make_shared<fr::RequirementsManager::Project>();        
      }
      setNameText();
      Parent::init();
    }

    void begin() override {
      Parent::begin();

      auto node = dynamic_pointer_cast<fr::RequirementsManager::Project>(_node);
      if (node) {
        auto inputTextFlags = ImGuiInputTextFlags_ReadOnly;
        if (_editable) {
          inputTextFlags = (ImGuiInputTextFlags_) 0;
        }
        ImGui::Text("Project Name:");
        ImGui::SameLine();
        if (ImGui::InputText(_nameLabel.c_str(),
                             _nameText,
                             nameLen,
                             inputTextFlags)) {
          node->setName(_nameText);
        }
        ImGui::Text("Project Description:");
        if (ImGui::InputTextMultiline(_descriptionLabel.c_str(),
                                      &_description,
                                      ImVec2(0,0),
                                      inputTextFlags)) {
          node->setDescription(_description);
        }
      } else {
        ImGui::Text("If you're seeing this, this ProjectWindow does not have a node somehow.");
        ImGui::Text("This should be impossible.");
      }
    }
    
  };
  
}
