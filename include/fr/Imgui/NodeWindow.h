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

  class NodeAnchor;
  
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
    static bool _defaultEditable;
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
      _defaultEditable = ed;
    }

    static void setDefaultDisplayEditabiltiy(bool ed) {
      _defaultDisplayEditabilityCheckbox = ed;
    }
    
    NodeWindow(const std::string& label = "Node");

    virtual ~NodeWindow() {}
    
    // Add a node for this window to hold.
    // Making this virtual but it shouldn't ever
    // have to be specialized.
    virtual void addNode(fr::RequirementsManager::Node::PtrType node);
    // Returns the held node. This can be a nullptr.
    virtual fr::RequirementsManager::Node::PtrType getNode();

    // Init populates the Node if it's empty and calls
    // init on it.
    void init();

    // Returns stored node id
    std::string idString();

    void begin() override;

    void end() override;
  };

}
