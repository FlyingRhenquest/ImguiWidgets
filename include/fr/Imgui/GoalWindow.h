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

  class GoalWindow : public NodeWindow {
    static const size_t confidenceLen = 201;
    char _confidence[confidenceLen];
    std::string _confidenceLabel;
    std::string _actionLabel;
    std::string _action;
    std::string _outcomeLabel;
    std::string _outcome;
    std::string _contextLabel;
    std::string _context;
    std::string _targetDateLabel;
    time_t _targetDate;
    std::string _alignmentLabel;
    std::string _alignment;
    tm _tmNow;

  public:
    using Type = GoalWindow;
    using PtrType = std::shared_ptr<Type>;
    using Parent = NodeWindow;
    using NodeType = fr::RequirementsManager::Goal;

    GoalWindow(const std::string &title = "Goal") : Parent(title) {
      memset(_confidence, '\0', confidenceLen);
      _confidenceLabel = getUniqueLabel("##Confidence");
      _actionLabel = getUniqueLabel("##Action");
      _outcomeLabel = getUniqueLabel("##Outcome");
      _contextLabel = getUniqueLabel("##Context");
      _targetDateLabel = getUniqueLabel("##TargetDate");
      _alignmentLabel = getUniqueLabel("##Alignment");
    }

    virtual ~GoalWindow() {}

    void init() override {
      if (!_node) {
        _node = std::make_shared<NodeType>();
        _node->init();
      }
      auto node = dynamic_pointer_cast<NodeType>(_node);
      if (node) {
        strncpy(_confidence, node->getTargetDateConfidence().c_str(), confidenceLen - 1);
        _action = node->getAction();
        _outcome = node->getOutcome();
        _context = node->getContext();
        _alignment = node->getAlignment();
        _targetDate = node->getTargetDate();
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        _tmNow = *std::gmtime(&now);
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
        ImGui::Text("Action:");
        if (ImGui::InputTextMultiline(_actionLabel.c_str(),
                                      &_action,
                                      ImVec2(0,0),
                                      inputTextFlags)) {
          node->setAction(_action);          
        }
        ImGui::Text("Outcome:");
        if (ImGui::InputTextMultiline(_outcomeLabel.c_str(),
                                      &_outcome,
                                      ImVec2(0,0),
                                      inputTextFlags)) {
          node->setOutcome(_outcome);
        }
        ImGui::Text("Context:");
        if (ImGui::InputTextMultiline(_contextLabel.c_str(),
                                      &_context,
                                      ImVec2(0,0),
                                      inputTextFlags)) {
          node->setContext(_context);
        }
        ImGui::Text("Alignment:");
        if (ImGui::InputTextMultiline(_alignmentLabel.c_str(),
                                      &_alignment,
                                      ImVec2(0,0),
                                      inputTextFlags)) {
          node->setAlignment(_alignment);

        }
        ImGui::Text("Target Date:");
        if (ImGui::DatePicker(_targetDateLabel.c_str(), _tmNow)) {
          auto estimate = std::mktime(&_tmNow);
          // This is an actual date in the POSIX epoch, we're not
          // deltaing this from when it's started.
          node->setTargetDate(estimate);
        }
        ImGui::Text("Target Date Confidence: ");
        ImGui::SameLine();
        if (ImGui::InputText(_confidenceLabel.c_str(),
                             _confidence,
                             confidenceLen - 1,
                             inputTextFlags)) {
          node->setTargetDateConfidence(_confidence);
        }
      } else {
        ImGui::Text("If you're seeing this text, this window somehow doesn't have a node.");
        ImGui::Text("This should be impossible.");
      }
    }  
  };

  namespace Registration {
    template <>
    struct Record<GoalWindow> {
      using Type = GoalWindow;
      using NodeType = GoalWindow::NodeType;

      static constexpr char name[] = "Goal";
      static constexpr char topMenuName[] = "Process Nodes";

      static constexpr void init(std::shared_ptr<Type> window) {}

      static constexpr ImVec2 startingSize() { return ImVec2(300, 750); }
    };
  }
  
}
