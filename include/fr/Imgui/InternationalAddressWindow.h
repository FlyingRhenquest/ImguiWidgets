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
#include <fr/Imgui/NodeAnchor.h>
#include <fr/RequirementsManager/UtilityNodes.h>
#include <imgui_stdlib.h>

namespace fr::Imgui {

  class InternationalAddressWindow : public NodeWindow {
    static const size_t countryCodeLen = 21;
    static const size_t localityLen = 201;
    static const size_t postalCodeLen = 51;
    char _countryCode[countryCodeLen];
    char _locality[localityLen];
    char _postalCode[postalCodeLen];
    std::string _countryCodeLabel;
    std::string _localityLabel;
    std::string _postalCodeLabel;
    std::string _addressLines;
    std::string _addressLinesLabel;

  public:
    using Type = InternationalAddressWindow;
    using PtrType = std::shared_ptr<Type>;
    using Parent = NodeWindow;
    using NodeType = fr::RequirementsManager::InternationalAddress;

    InternationalAddressWindow(const std::string &title = "International Address") : Parent(title) {
      memset(_countryCode, '\0', countryCodeLen);
      memset(_locality, '\0', localityLen);
      memset(_postalCode, '\0', postalCodeLen);
      _countryCodeLabel = getUniqueLabel("##CountryCode");
      _localityLabel = getUniqueLabel("##Locality");
      _postalCodeLabel = getUniqueLabel("##PostalCode");
      _addressLinesLabel = getUniqueLabel("##AddressLines");
    }

    virtual ~InternationalAddressWindow() {
    }

    void init() override {
      if (!_node) {
        _node = std::make_shared<NodeType>();
        _node->init();
      }
      auto node = dynamic_pointer_cast<NodeType>(_node);
      if (node) {
        strncpy(_countryCode, node->getCountryCode().c_str(), countryCodeLen - 1);
        strncpy(_locality, node->getLocality().c_str(), localityLen - 1);
        strncpy(_postalCode, node->getPostalCode().c_str(), postalCodeLen - 1);
        _addressLines = node->getAddressLines();
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
        
        ImGui::Text("Country code: ");
        ImGui::SameLine();
        if (ImGui::InputText(_countryCodeLabel.c_str(),
                             _countryCode,
                             countryCodeLen - 1,
                             inputTextFlags)) {
          node->setCountryCode(_countryCode);
        }
        ImGui::Text("Address: ");
        if (ImGui::InputTextMultiline(_addressLinesLabel.c_str(),
                                      &_addressLines,
                                      ImVec2(0,0),
                                      inputTextFlags)) {
          node->setAddressLines(_addressLines);
        }
        ImGui::Text("Locality: ");
        ImGui::SameLine();
        if (ImGui::InputText(_localityLabel.c_str(),
                             _locality,
                             localityLen - 1,
                             inputTextFlags)) {
          node->setLocality(_locality);
        }
        ImGui::Text("Postal Code: ");
        ImGui::SameLine();
        if (ImGui::InputText(_postalCodeLabel.c_str(),
                             _postalCode,
                             postalCodeLen - 1,
                             inputTextFlags)) {
          node->setPostalCode(_postalCode);
        }
      } else {
        ImGui::Text("If you're seeing this text, this window somehow doesn't have a node.");
        ImGui::Text("This should be impossible.");
      }
    }
  };

  namespace Registration {

    template<>
    struct Record<InternationalAddressWindow> {
      using Type = InternationalAddressWindow;
      using NodeType = InternationalAddressWindow::NodeType;

      static constexpr char name[] = "International Address";
      static constexpr char topMenuName[] = "Utility Nodes";

      static constexpr void init(std::shared_ptr<Type> window) {}
      static constexpr ImVec2 startingSize() { return ImVec2(300, 400); }
    };
    
  }
  
}
