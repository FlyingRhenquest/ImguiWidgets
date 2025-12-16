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

#include <time.h>
#include <chrono>
#include <ctime>
#include <format>
#include <ImGuiDatePicker.hpp>
#include <fr/Imgui/NodeWindow.h>
#include <fr/RequirementsManager/UtilityNodes.h>
#include <imgui_stdlib.h>
#include <memory>

namespace fr::Imgui {

  class TimeEstimateWindow : public NodeWindow {
  protected:
    std::string _text;
    std::string _textLabel;
    std::string _estimateLabel;
    unsigned long _estimate;
    std::string _startedLabel;
    bool _started;
    std::string _startedTimestampLabel;
    time_t _startedTimestamp;
    // Making these class members so I don't have to
    // query the system clock ever single frame
    time_t _now;
    tm _tmNow;

  public:
    using Type = TimeEstimateWindow;
    using PtrType = std::shared_ptr<Type>;
    using Parent = NodeWindow;
    using NodeType = fr::RequirementsManager::TimeEstimate;

    TimeEstimateWindow(const std::string &title = "TimeEstimate") : Parent(title) {
      _textLabel = getUniqueLabel("##Text");
      _estimateLabel = getUniqueLabel("##Estimate");
      _startedLabel = getUniqueLabel("##Started");
      _startedTimestampLabel = getUniqueLabel("##StartedTimestamp");
    }

    virtual ~TimeEstimateWindow() {}

    void init() override {
      if (!_node) {
        _node = std::make_shared<NodeType>();
        _node->init();
      }
      auto node = dynamic_pointer_cast<NodeType>(_node);
      if (node) {
        _text = node->getText();
        _estimate = node->getEstimate();
        _started = node->getStarted();
        _startedTimestamp = node->getStartTimestamp();
      }
      auto now = std::chrono::system_clock::now();
      _now = std::chrono::system_clock::to_time_t(now);
      _tmNow = *std::gmtime(&_now);

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
        ImGui::Text("Text:");
        if (ImGui::InputTextMultiline(_textLabel.c_str(),
                                      &_text,
                                      ImVec2(0,0),
                                      inputTextFlags)) {
          node->setText(_text);
        }
        ImGui::Text("Stated: ");
        ImGui::SameLine();
        if (ImGui::Checkbox(_startedLabel.c_str(), &_started)) {
          node->setStarted(_started);
          if (_started) {
            auto now = std::chrono::system_clock::now();
            _startedTimestamp = std::chrono::system_clock::to_time_t(now);
            node->setStartTimestamp(_startedTimestamp);            
          }
        }
        if (ImGui::DatePicker(_estimateLabel.c_str(), _tmNow)) {
          auto estimate = std::mktime(&_tmNow);
          if (_started) {
            estimate -= node->getStartTimestamp();
          } else {
            estimate -= _now;
          }
          node->setEstimate(estimate);
        }
        if (_started) {
          // TODO: I'm not sure I want to do this many calculations over
          // a large number of windows. Consider moving this to init
          // or doing another function to compute them once into class members.
          auto started = std::chrono::system_clock::from_time_t(node->getStartTimestamp());
          auto estEnd = started + std::chrono::seconds(node->getEstimate());
          auto remaining = estEnd - std::chrono::system_clock::from_time_t(_now);
          auto daysRemaining = std::chrono::floor<std::chrono::days>(remaining);
          auto startedLabel = std::format("Started: {:%FT%TZ}", started);
          ImGui::Text(startedLabel.c_str());
          auto remainingLabel = std::format("Est. Days Remaining: {}", daysRemaining);
          ImGui::Text(remainingLabel.c_str());
        }        
      } else {
        ImGui::Text("If you're seeing this, this window somehow doesn't have a node");
        ImGui::Text("This should be impossible.");
      }
    }
  };

  namespace Registration {

    template <>
    struct Record<TimeEstimateWindow> {
      using Type = TimeEstimateWindow;
      using NodeType = TimeEstimateWindow::NodeType;

      static constexpr char name[] = "Time Estimate";
      static constexpr char topMenuName[] = "Utility Nodes";
      
      static constexpr void init(std::shared_ptr<Type> window) {}

      static constexpr ImVec2 startingSize() { return ImVec2(300, 400); }
    
    };
  }
}
