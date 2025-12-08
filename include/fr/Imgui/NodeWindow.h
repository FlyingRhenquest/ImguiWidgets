/**
 * Copyright 2025 Bruce Ide
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#pragma once

#include <format>
#include <fr/Imgui/NodeAnchor.h>
#include <fr/Imgui/Window.h>
#include <fr/RequirementsManager/Node.h>
#include <string>
#include <memory>

namespace fr::Imgui {

  /**
   * A window that renders a RequirementsManager Node.
   * You can put your own node in. If you render the window
   * before doing so, the window will automatically create
   * and init the node.
   */

  class NodeWindow : public Window {
  public:
    using Type = NodeWindow;
    using PtrType = std::shared_ptr<NodeWindow>;
    using Parent = Window;

    // Max ID text len
    static const size_t idTextLen = 50;
    
  protected:
    // Held node
    fr::RequirementsManager::Node::PtrType _node;
    // Default editability starting state
    static bool _defaultEditability;
    // default starting state to display the editable checkbox
    static bool _defaultDisplayEditabilityCheckbox;
        
    // Makes editable fields in this node window
    // editable. Node doesn't have any, so this
    // doesn't do anything for Node. You have to
    // check the "Editable" checkbox in the node to make
    // it editable.
    bool _editable;
    
    // Display the editable checkbox
    bool _displayEditable;

    // Node-specific buffers
    char _idText[idTextLen];

    std::shared_ptr<NodeAnchor> _upAnchor;
    std::shared_ptr<NodeAnchor> _downAnchor;
    
    bool _initted;
    // Window and Widget Labels
    std::string _enableEditingLabel;
    std::string _nodeIdLabel;
    
    void setIdText() {
      std::string text = std::format("ID: {}", _node->idString());
      strncpy(_idText, text.c_str(), idTextLen);
    }

  public:
    // You can set the default editability of a node to
    // whatever you want.
    static void setDefaultEditability(bool ed) {
      _defaultEditability = ed;
    }

    static void setDefaultDisplayEditabiltiy(bool ed) {
      _defaultDisplayEditabilityCheckbox = ed;
    }
    
    NodeWindow(const std::string& label = "Node") : Parent(label),
                                                    _editable(_defaultEditability),
                                                    _displayEditable(_defaultDisplayEditabilityCheckbox),
                                                    _initted(false) {
      memset(_idText, '\0', idTextLen);
      ImU32 white = IM_COL32(255,255,255,255);
      ImU32 red = IM_COL32(255,0,0,255);
      _upAnchor = std::make_shared<NodeAnchor>("##UpAnchor", ImVec2(0,0), 5.0, white, red, AnchorType::Up);
      _downAnchor = std::make_shared<NodeAnchor>("##DownAnchor", ImVec2(0,0), 5.0, white, red, AnchorType::Down);
      _enableEditingLabel = getUniqueLabel("Enable Editing");
      _nodeIdLabel = getUniqueLabel("ID: ");
    }

    virtual ~NodeWindow() {}
    
    // Add a node for this window to hold.
    // Making this virtual but it shouldn't ever
    // have to be specialized.
    virtual void addNode(fr::RequirementsManager::Node::PtrType node) {
      _node = node;
    }
    // Returns the held node. This can be a nullptr.
    virtual fr::RequirementsManager::Node::PtrType getNode() {
      return _node;
    }

    // Init populates the Node if it's empty and calls
    // init on it.
    void init() {
      if (!_node) {
        _node = std::make_shared<fr::RequirementsManager::Node>();
      }
      if (!_node->initted) {
        _node->init();
      }
      this->addWidget(_upAnchor->getLabel(), _upAnchor);
      this->addWidget(_downAnchor->getLabel(), _downAnchor);

      // Set up a lambda so the anchor point render better as the
      // window is being moved.
      auto sub = this->moved.connect([&](Window::PtrType parent,
                                         const ImVec2& pos) {
        _min = pos;
        ImVec2 upListCenter(_currentSize.x / 2.0, 25.0);
        _upAnchor->setCenter(screenCoordinate(upListCenter));
        ImVec2 downListCenter(_currentSize.x / 2.0, _currentSize.y);
        _downAnchor->setCenter(screenCoordinate(downListCenter));
      });

      _subscriptions.push_back(sub);

      _initted = true;
    }

    // Returns stored node id
    std::string idString() {
      std::string ret;
      if (_node) {
        ret = _node->idString();
      }
      return ret;
    }

    void begin() override {
      if (!_node || !_initted) {
        init();
      }
      setIdText();

      Parent::begin();
      
      if (_displayEditable) {
        ImGui::Checkbox(_enableEditingLabel.c_str(), &_editable);
      }
      // Node ID text is always readonly
      std::string id = getUniqueLabel("ID: ");
      ImGui::InputText(_nodeIdLabel.c_str(), _idText, idTextLen, ImGuiInputTextFlags_ReadOnly);
    }

    void end() override {
      Parent::end();      
    }
  };

}
