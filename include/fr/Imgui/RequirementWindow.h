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

#include <fr/RequirementsManager/Requirement.h>
#include <fr/Imgui/CommitableNodeWindow.h>
#include <imgui_stdlib.h>

namespace fr::Imgui {

  class RequirementWindow : public CommitableNodeWindow {
    static const size_t titleLen = 201;
    char _titleText[titleLen];
    std::string _titleLabel;
    std::string _textLabel;
    std::string _text;
    std::string _functionalLabel;
    bool _functional;

  public:

    using Type = RequirementWindow;
    using PtrType = std::shared_ptr<Type>;
    using Parent = CommitableNodeWindow;
    using NodeType = fr::RequirementsManager::Requirement;

    RequirementWindow(const std::string &title = "Requirement") : Parent(title) {
      _functional = false;
      memset(_titleText, '\0', titleLen);
      _titleLabel = getUniqueLabel("##Title");
      _textLabel = getUniqueLabel("##Text");
      _functionalLabel = getUniqueLabel("##Functional");      
    }

    virtual ~RequirementWindow() {}

    void init() override {
      if (!_node) {
        _node = std::make_shared<NodeType>();
        _node->init();
      }

      Parent::init();
    }

    void beginning() override {
      auto node = dynamic_pointer_cast<NodeType>(_node);
      if (node) {
        std::cout << "Requirement loading" << std::endl;
        std::cout << "ID: " << node->idString() << std::endl;
        std::cout << "Title: " << node->getTitle() << std::endl;
        std::cout << "Text: " << node->getText() << std::endl;
        std::cout << "Functional: " << node->isFunctional() << std::endl;
        strncpy(_titleText, node->getTitle().c_str(), titleLen - 1);
        _text = node->getText();
        _functional = node->isFunctional();
      }
      Parent::beginning();
    }
    
    void begin() override {
      Parent::begin();

      auto node = dynamic_pointer_cast<NodeType>(_node);
      if (node) {
        auto inputTextFlags = ImGuiInputTextFlags_ReadOnly;
        // Do not allow any editing if node is committed
        if (_editable && !node->isCommitted()) {
          inputTextFlags = (ImGuiInputTextFlags_) 0;
        }
        ImGui::Text("Functional: ");
        ImGui::SameLine();
        if (ImGui::Checkbox(_functionalLabel.c_str(), &_functional)) {
          if (_editable && !node->isCommitted()) {
            node->setFunctional(_functional);
          }
        }

        ImGui::Text("Title: ");
        ImGui::SameLine();
        if (ImGui::InputText(_titleLabel.c_str(),
                             _titleText,
                             titleLen - 1,
                             inputTextFlags)) {
          node->setTitle(_titleText);
        }
        ImGui::Text("Requirement Text:");
        if (ImGui::InputTextMultiline(_textLabel.c_str(),
                                      &_text,
                                      ImVec2(0,0),
                                      inputTextFlags)) {
          node->setText(_text);
        }
      } else {
        ImGui::Text("If you're seeing this text, this node somehow doesn't have a node.");
        ImGui::Text("This should be impossible.");
      }
    }
    
  };

  namespace Registration {

    template <>
    struct Record<RequirementWindow> {
      using Type = RequirementWindow;
      using NodeType = RequirementWindow::NodeType;

      static constexpr char name[] = "Requirement";
      static constexpr char topMenuName[] = "Process Nodes";

      static constexpr void init(std::shared_ptr<Type> window) {}

      static constexpr ImVec2 startingSize() { return ImVec2(300,350); }
    };
    
  }
  
}
