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

  class USAddressWindow : public NodeWindow {
    static const size_t cityLen = 101;
    static const size_t stateLen = 41;
    static const size_t zipCodeLen = 21;
    std::string _addressLines;
    char _city[cityLen];
    char _state[stateLen];
    char _zipCode[zipCodeLen];
    std::string _addressLinesLabel;
    std::string _cityLabel;
    std::string _stateLabel;
    std::string _zipCodeLabel;

  public:
    using Type = USAddressWindow;
    using PtrType = std::shared_ptr<Type>;
    using Parent = NodeWindow;
    using NodeType = fr::RequirementsManager::USAddress;

    USAddressWindow(const std::string &title = "US Address") : Parent(title) {
      memset(_city, '\0', cityLen);
      memset(_state, '\0', stateLen);
      memset(_zipCode, '\0', zipCodeLen);
      _addressLinesLabel = getUniqueLabel("##AddressLines");
      _cityLabel = getUniqueLabel("##City");
      _stateLabel = getUniqueLabel("##State");
      _zipCodeLabel = getUniqueLabel("##ZipCode");
    }

    virtual ~USAddressWindow() {}

    void init() override {
      if (!_node) {
        _node = std::make_shared<NodeType>();
        _node->init();
      }
      auto node = dynamic_pointer_cast<NodeType>(_node);
      if (node) {
        _addressLines = node->getAddressLines();
        strncpy(_city, node->getCity().c_str(), cityLen - 1);
        strncpy(_state, node->getState().c_str(), stateLen - 1);
        strncpy(_zipCode, node->getZipCode().c_str(), zipCodeLen - 1);
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
        ImGui::Text("Address:");
        if (ImGui::InputTextMultiline(_addressLinesLabel.c_str(),
                                      &_addressLines,
                                      ImVec2(0,0),
                                      inputTextFlags)) {
          node->setAddressLines(_addressLines);
        }
        ImGui::Text("City: ");
        ImGui::SameLine();
        if (ImGui::InputText(_cityLabel.c_str(),
                             _city,
                             cityLen - 1,
                             inputTextFlags)) {
          node->setCity(_city);
        }
        ImGui::Text("State: ");
        ImGui::SameLine();
        if (ImGui::InputText(_stateLabel.c_str(),
                             _state,
                             stateLen - 1,
                             inputTextFlags)) {
          node->setState(_state);
        }
        ImGui::Text("Zip Code: ");
        ImGui::SameLine();
        if (ImGui::InputText(_zipCodeLabel.c_str(),
                             _zipCode,
                             zipCodeLen - 1,
                             inputTextFlags)) {
          node->setZipCode(_zipCode);
        }
      } else {
        ImGui::Text("If you're seeing this text, this window somehow doesn't have a node.");
        ImGui::Text("This should be impossible.");
      }
    }
  };

  namespace Registration {

    template <>
    struct Record<USAddressWindow> {
      using Type = USAddressWindow;
      using NodeType = USAddressWindow::NodeType;

      static constexpr char name[] = "US Address";
      static constexpr char topMenuName[] = "Utility Nodes";

      static constexpr void init(std::shared_ptr<Type> window) {}
      static constexpr ImVec2 startingSize() { return ImVec2(300,400); }
    };
    
  }
  
}
