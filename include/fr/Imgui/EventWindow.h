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

  class EventWindow : public NodeWindow {
    static const size_t nameLen = 201;
    char _name[nameLen];
    std::string _nameLabel;
    std::string _description;
    std::string _descriptionLabel;

  public:
    using Type = EventWindow;
    using PtrType = std::shared_ptr<Type>;
    using Parent = NodeWindow;
    using NodeType = fr::RequirementsManager::Event;

    EventWindow(const std::string &title = "Event") : Parent(title) {
      memset(_name, '\0', nameLen);
      _nameLabel = getUniqueLabel("##Name");
      _descriptionLabel = getUniqueLabel("##Description");
    }

    virtual ~EventWindow() {}

    void init() override {
      if (!_node) {
        _node = std::make_shared<NodeType>();
        _node->init();
      }
      auto node = dynamic_pointer_cast<NodeType>(_node);
      if (node) {
        strncpy(_name, node->getName().c_str(), nameLen - 1);
        _description = node->getDescription();
      }
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
        ImGui::Text("Event Name: ");
        ImGui::SameLine();
        if (ImGui::InputText(_nameLabel.c_str(),
                             _name,
                             nameLen - 1,
                             inputTextFlags)) {
          node->setName(_name);
        }
        ImGui::Text("Event Description:");
        if (ImGui::InputTextMultiline(_descriptionLabel.c_str(),
                                      &_description,
                                      ImVec2(0,0),
                                      inputTextFlags)) {
          node->setDescription(_description);
        }
      } else {
        ImGui::Text("If you're seeing this text, this window somehow doesn't have a node.");
        ImGui::Text("This should be impossible.");
      }
      
    }
    
  };

  namespace Registration {

    template <>
    struct Record<EventWindow> {
      using Type = EventWindow;
      using NodeType = EventWindow::NodeType;

      static constexpr char name[] = "Event";
      static constexpr char topMenuName[] = "Process Nodes";

      static constexpr void init(std::shared_ptr<Type> window) {}
      static constexpr ImVec2 startingSize() { return ImVec2(300,300); }
    };
    
  }
  
}
