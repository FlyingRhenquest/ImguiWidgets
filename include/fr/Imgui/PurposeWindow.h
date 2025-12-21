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

// Set bounds for date picker
#define IMGUI_DATEPICKER_YEAR_MIN 2015
#define IMGUI_DATEPICKER_YEAR_MAX 2040

#include <ImGuiDatePicker.hpp>
#include <chrono>
#include <ctime>
#include <format>
#include <fr/Imgui/NodeWindow.h>
#include <fr/RequirementsManager/UtilityNodes.h>
#include <imgui_stdlib.h>
#include <time.h>

namespace fr::Imgui {

  class PurposeWindow : public NodeWindow {
    static const size_t confidenceLen = 201;
    char _confidence[confidenceLen];
    std::string _confidenceLabel;
    std::string _description;
    std::string _descriptionLabel;
    std::string _deadlineLabel;
    time_t _deadline;
    tm _tmDeadline;

  public:
    using Type = PurposeWindow;
    using PtrType = std::shared_ptr<Type>;
    using Parent = NodeWindow;
    using NodeType = fr::RequirementsManager::Purpose;

    PurposeWindow(const std::string &title = "Purpose") : Parent(title) {
      memset(_confidence, '\0', confidenceLen);
      _confidenceLabel = getUniqueLabel("##Confidence");
      _descriptionLabel = getUniqueLabel("##Description");
      _deadlineLabel = getUniqueLabel("##Deadline");
    }

    virtual ~PurposeWindow() {}

    void init() override {
      bool inittedNode = false;
      if (!_node) {
        inittedNode = true;
        _node = std::make_shared<NodeType>();
        _node->init();
      }
      
      auto node = dynamic_pointer_cast<NodeType>(_node);
      if (node) {
        strncpy(_confidence, node->getDeadlineConfidence().c_str(), confidenceLen - 1);
        _description = node->getDescription();
        if (inittedNode) {
          _deadline = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
          node->setDeadline(_deadline);
        } else {
          _deadline = node->getDeadline();
        }
        _tmDeadline = *std::gmtime(&_deadline);
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
        ImGui::Text("Description:");
        if (ImGui::InputTextMultiline(_descriptionLabel.c_str(),
                                      &_description,
                                      ImVec2(0,0),
                                      inputTextFlags)) {
          node->setDescription(_description);          
        }
        ImGui::Text("Deadline:");
        if (ImGui::DatePicker(_deadlineLabel.c_str(), _tmDeadline)) {
          _deadline = std::mktime(&_tmDeadline);
          // This is an actual date in the POSIX epoch, we're not
          // deltaing this from when it's started.
          node->setDeadline(_deadline);
        }
        ImGui::Text("Deadline Confidence: ");
        ImGui::SameLine();
        if (ImGui::InputText(_confidenceLabel.c_str(),
                             _confidence,
                             confidenceLen - 1,
                             inputTextFlags)) {
          node->setDeadlineConfidence(_confidence);
        }
      } else {
        ImGui::Text("If you're seeing this text, this window somehow doesn't have a node.");
        ImGui::Text("This should be impossible.");
      }
    }
    
  };

  namespace Registration {
    template <>
    struct Record<PurposeWindow> {
      using Type = PurposeWindow;
      using NodeType = PurposeWindow::NodeType;

      static constexpr char name[] = "Purpose";
      static constexpr char topMenuName[] = "Process Nodes";

      static constexpr void init(std::shared_ptr<Type> window) {}
      static constexpr ImVec2 startingSize() { return ImVec2(300,400); }
    };
  }
  
}
