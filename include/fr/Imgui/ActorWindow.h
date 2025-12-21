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
#include <fr/ImguiWidgets.h>

namespace fr::Imgui {

  class ActorWindow : public NodeWindow {
  protected:
    static const size_t actorLen = 201;
    char _actor[actorLen];
    std::string _actorLabel;

  public:
    using Type = ActorWindow;
    using PtrType = std::shared_ptr<Type>;
    using Parent = NodeWindow;
    using NodeType = fr::RequirementsManager::Actor;

    ActorWindow(const std::string &title = "Actor") : Parent(title) {
      memset(_actor, '\0', actorLen);
      _actorLabel = getUniqueLabel("##Actor");
    }

    virtual ~ActorWindow() {}

    void init() override {
      if (!_node) {
        _node = std::make_shared<NodeType>();
        _node->init();
      }
      auto node = dynamic_pointer_cast<NodeType>(_node);
      if (node) {
        strncpy(_actor, node->getActor().c_str(), actorLen - 1);
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

        ImGui::Text("Actor: ");
        ImGui::SameLine();
        if (ImGui::InputText(_actorLabel.c_str(),
                             _actor,
                             actorLen - 1,
                             inputTextFlags)) {
          node->setActor(_actor);
        }
      } else {
        ImGui::Text("If you're seeing this, this window somehow doesn't have a node");
        ImGui::Text("This should be impossible.");
      }
    }
    
  };

  namespace Registration {
    template <>
    struct Record<ActorWindow> {
      using Type = ActorWindow;
      using NodeType = ActorWindow::NodeType;

      static constexpr char name[] = "Actor";
      static constexpr char topMenuName[] = "Process Nodes";

      static constexpr void init(std::shared_ptr<Type> window) {}
      static constexpr ImVec2 startingSize() { return ImVec2(300,300); }
    };
  }
  
}
