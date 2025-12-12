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

#include <fr/RequirementsManager/Product.h>
#include <fr/Imgui/CommitableNodeWindow.h>
#include <imgui_stdlib.h>

namespace fr::Imgui {

  class ProductWindow : public CommitableNodeWindow {
    static const size_t titleLen = 200;
    char _titleText[titleLen];
    std::string _titleLabel;
    std::string _descriptionLabel;
    std::string _description;

    void setTitleText() {
      auto node = dynamic_pointer_cast<fr::RequirementsManager::Product>(_node);
      if (node) {
        strncpy(_titleText, node->getTitle().c_str(), titleLen);
      }
    }
    
  public:

    using Type = ProductWindow;
    using PtrType = std::shared_ptr<Type>;
    using Parent = CommitableNodeWindow;

    ProductWindow(std::string title = "Product") : Parent(title) {
      memset(_titleText, '\0', titleLen);
      _titleLabel = getUniqueLabel("##Title");
      _descriptionLabel = getUniqueLabel("##Description");
    }

    virtual ~ProductWindow() {}

    void init() override {
      if (!_node) {
        _node = std::make_shared<fr::RequirementsManager::Product>();
      }
      setTitleText();
      Parent::init();
    }

    void begin() override {
      Parent::begin();
      auto node = dynamic_pointer_cast<fr::RequirementsManager::Product>(_node);
      if (node) {
        auto inputTextFlags = ImGuiInputTextFlags_ReadOnly;
        // Do not allow any editing if node is committed
        if (_editable && !node->isCommitted()) {
          inputTextFlags = (ImGuiInputTextFlags_) 0;
        }

        ImGui::Text("Title: ");
        ImGui::SameLine();
        if (ImGui::InputText(_titleLabel.c_str(), _titleText, titleLen, inputTextFlags)) {
          node->setTitle(_titleText);
        }
        ImGui::Text("Product Description:");
        if (ImGui::InputTextMultiline(_descriptionLabel.c_str(),
                                      &_description,
                                      ImVec2(0,0),
                                      inputTextFlags)) {
          node->setDescription(_description);
        }
      } else {
        ImGui::Text("If you're seeing this, this ProductWindow doesn't have a node somehow.");
        ImGui::Text("This should be impossible.");
      }
    }
    
  };
  
}
