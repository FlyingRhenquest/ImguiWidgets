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

namespace fr::Imgui {

  class PhoneNumberWindow : public NodeWindow {
    static const size_t countryCodeLen = 11;
    static const size_t numberLen = 21;
    static const size_t typeLen = 21;

    char _countryCode[countryCodeLen];
    char _number[numberLen];
    char _phoneType[typeLen];
    std::string _countryCodeLabel;
    std::string _numberLabel;
    std::string _phoneTypeLabel;

  public:
    using Type = PhoneNumberWindow;
    using PtrType = std::shared_ptr<Type>;
    using Parent = NodeWindow;
    using NodeType = fr::RequirementsManager::PhoneNumber;

    PhoneNumberWindow(const std::string &title = "Phone Number") : Parent(title) {
      memset(_countryCode, '\0', countryCodeLen);
      memset(_number, '\0', numberLen);
      memset(_phoneType, '\0', typeLen);
      _countryCodeLabel = getUniqueLabel("##CountryCode");
      _numberLabel = getUniqueLabel("##Number");
      _phoneTypeLabel = getUniqueLabel("##PhoneType");
    }

    virtual ~PhoneNumberWindow() {}

    void init() override {
      if (!_node) {
        _node = std::make_shared<NodeType>();
        _node->init();
      }
      auto node = dynamic_pointer_cast<NodeType>(_node);
      if (node) {
        strncpy(_countryCode, node->getCountryCode().c_str(), countryCodeLen - 1);
        strncpy(_number, node->getNumber().c_str(), numberLen - 1);
        strncpy(_phoneType, node->getPhoneType().c_str(), typeLen - 1);
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
        ImGui::Text("Country Code: ");
        ImGui::SameLine();
        if (ImGui::InputText(_countryCodeLabel.c_str(),
                             _countryCode,
                             countryCodeLen - 1,
                             inputTextFlags)) {
          node->setCountryCode(_countryCode);
        }
        ImGui::Text("Number: ");
        ImGui::SameLine();
        if (ImGui::InputText(_numberLabel.c_str(),
                             _number,
                             numberLen - 1,
                             inputTextFlags)) {
          node->setNumber(_number);
        }
        ImGui::Text("Phone Type: ");
        ImGui::SameLine();
        // TODO: Check around and see if someone's implemented a
        // edit box with suggestions.
        if (ImGui::InputText(_phoneTypeLabel.c_str(),
                             _phoneType,
                             typeLen - 1,
                             inputTextFlags)) {
          node->setPhoneType(_phoneType);
        }
      } else {
        ImGui::Text("If you're seeing this text, this window somehow doesn't have a node.");
        ImGui::Text("This should be impossible.");
      }
    }
  };

  namespace Registration {
    template <>
    struct Record<PhoneNumberWindow> {
      using Type = PhoneNumberWindow;
      using NodeType = PhoneNumberWindow::NodeType;

      static constexpr char name[] = "Phone Number";
      static constexpr char topMenuName[] = "Utility Nodes";

      static constexpr void init(std::shared_ptr<Type> window) {}
      static constexpr ImVec2 startingSize() { return ImVec2(300, 350); }      
    };
  }
  
}
