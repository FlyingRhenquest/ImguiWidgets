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

#include <fteng/signals.hpp>
#include <fr/Imgui/NodeWindow.h>
#include <format>

namespace fr::Imgui {

  /**
   * CommittableNodeWindow provideds left/right anchors for committable
   * nodes in addition to the up/down ones.
   */

  class CommitableNodeWindow : public NodeWindow {
    std::vector<fteng::connection> _subscriptions;
  public:
    using Type = CommitableNodeWindow;
    using PtrType = std::shared_ptr<Type>;
    using Parent = NodeWindow;
    using NodeType = fr::RequirementsManager::CommitableNode;

    static constexpr char WindowTitle[] = "CommitableNode";

    std::shared_ptr<NodeAnchor> _leftAnchor;
    std::shared_ptr<NodeAnchor> _rightAnchor;

    CommitableNodeWindow(const std::string& label = "CommitableNode") :
      Parent(label) {
      ImU32 white = IM_COL32(255,255,255,255);
      ImU32 red = IM_COL32(255,0,0,255);
      _leftAnchor = std::make_shared<NodeAnchor>("##LeftAnchor", ImVec2(0,0), 5.0, white, red, AnchorType::Left);
      _rightAnchor = std::make_shared<NodeAnchor>("##RightAnchor", ImVec2(0,0), 5.0, white, red, AnchorType::Right);
    }

    virtual ~CommitableNodeWindow() {}

    void init() override {

      this->addWidget(_leftAnchor->getLabel(), _leftAnchor);
      this->addWidget(_rightAnchor->getLabel(), _rightAnchor);

      auto sub = this->moved.connect([&](Window::PtrType parent,
                                         const ImVec2& pos) {
        _min = pos;

        // Put the left/right anchor nodes at the top of the window
        // and see how that looks
        ImVec2 leftAnchor(10.0, 25.0);
        _leftAnchor->setCenter(screenCoordinate(leftAnchor));
        ImVec2 rightAnchor(_currentSize.x, 25.0);
        _rightAnchor->setCenter(screenCoordinate(rightAnchor));
      });
      _subscriptions.push_back(std::move(sub));
      Parent::init();
    }

    void begin() override {
      Parent::begin();
      
      auto node = dynamic_pointer_cast<fr::RequirementsManager::CommitableNode>(_node);
      if (node) {
        if (node->isCommitted()) {
          _displayEditable = false;
          _editable = false;          
        }
        if (!node->isCommitted()) {
          if (ImGui::Button("Commit")) {
            node->commit();
          }
          if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            std::string tooltipText = std::format("Committing the node causes it to become permanently un-editable.");
            ImGui::Text(tooltipText.c_str());
            ImGui::EndTooltip();
          }
        }
      }
    }
    
  };

  namespace Registration {
    
    template <>
    struct Record<CommitableNodeWindow> {
      using Type = CommitableNodeWindow;

      static constexpr char name[] = "CommitableWindow";
      static constexpr char topMenuName[] = "Test Node";

      static constexpr void init(std::shared_ptr<Type> window) {};

      static constexpr ImVec2 startingSize() { return ImVec2(300,100); }
    };

  }
  
}
