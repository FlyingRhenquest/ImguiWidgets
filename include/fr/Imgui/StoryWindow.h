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

#include <fr/RequirementsManager/Story.h>
#include <fr/ImguiWidgets.h>
#include <imgui_stdlib.h>

namespace fr::Imgui {

  class StoryWindow : public CommitableNodeWindow {
    static const size_t titleLen = 201;
    char _titleText[titleLen];
    std::string _titleLabel;
    std::string _goalLabel;
    std::string _goal;
    std::string _benefitLabel;
    std::string _benefit;

    void setTitleText() {
      auto node = dynamic_pointer_cast<fr::RequirementsManager::Story>(_node);
      if (node) {
        strncpy(_titleText, node->getTitle().c_str(), titleLen - 1);
      }
    }

  public:
    using Type = StoryWindow;
    using PtrType = std::shared_ptr<StoryWindow>;
    using Parent = CommitableNodeWindow;
    using NodeType = fr::RequirementsManager::Story;

    StoryWindow(const std::string &title = "Story") : Parent(title) {
      memset(_titleText, '\0', titleLen);
      _titleLabel = getUniqueLabel("##Title");
      _goalLabel = getUniqueLabel("##Goal");
      _benefitLabel = getUniqueLabel("##Benefit");
    }

    virtual ~StoryWindow() {}

    void init() override {
      if (!_node) {
        _node = std::make_shared<fr::RequirementsManager::Story>();
        _node->init();
      }
      setTitleText();
      Parent::init();
    }

    void begin() override {
      Parent::begin();

      auto node = dynamic_pointer_cast<fr::RequirementsManager::Story>(_node);
      if (node) {
        auto inputTextFlags = ImGuiInputTextFlags_ReadOnly;
        if (_editable && !node->isCommitted()) {
          inputTextFlags = (ImGuiInputTextFlags_) 0;
        }
        ImGui::Text("Title: ");
        ImGui::SameLine();
        if (ImGui::InputText(_titleLabel.c_str(),
                             _titleText,
                             titleLen - 1,
                             inputTextFlags)) {
          node->setTitle(_titleText);
        }
        ImGui::Text("Goal:");
        if (ImGui::InputTextMultiline(_goalLabel.c_str(),
                                      &_goal,
                                      ImVec2(0,0),
                                      inputTextFlags)) {
          node->setGoal(_goal);
        }
        ImGui::Text("Benefit:");
        if (ImGui::InputTextMultiline(_benefitLabel.c_str(),
                                      &_benefit,
                                      ImVec2(0,0),
                                      inputTextFlags)) {
          node->setBenefit(_benefit);
        }
        
      } else {
        ImGui::Text("If you're seeing this text, this node somehow doesn't have a node.");
        ImGui::Text("This should be impossible.");
      }
    
    }

  };

  namespace Registration {

    template <>
    struct Record<StoryWindow> {
      using Type = StoryWindow;
      using NodeType = StoryWindow::NodeType;

      static constexpr char name[] = "Story";
      static constexpr char topMenuName[] = "Process Nodes";

      static constexpr void init(std::shared_ptr<Type> window) {}

      static constexpr ImVec2 startingSize() { return ImVec2(300,500); }
    };
    
  }
  
}
