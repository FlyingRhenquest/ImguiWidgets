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

#include <format>
#include <fr/Imgui/NodeAnchor.h>
#include <fr/Imgui/Window.h>
#include <fr/Imgui/Registration.h>
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
    using NodeType = fr::RequirementsManager::Node;

    // Max ID text len
    static constexpr size_t idTextLen = 51;
    // Default window title
    static constexpr char WindowTitle[] = "Node";
    
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
    // Debug button prints _node.to_json() to console
    bool _displayDebugButton;

    // Node-specific buffers
    char _idText[idTextLen];

    std::shared_ptr<NodeAnchor> _upAnchor;
    std::shared_ptr<NodeAnchor> _downAnchor;
    
    bool _initted;
    // Window and Widget Labels
    std::string _enableEditingLabel;
    std::string _nodeIdLabel;
    std::string _debugButtonLabel;
    
    void setIdText() {
      std::string text = std::format("{}", _node->idString());
      strncpy(_idText, text.c_str(), idTextLen - 1);
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
    
    NodeWindow(const std::string& label = WindowTitle);

    virtual ~NodeWindow() {}
    
    // Add a node for this window to hold.
    // Making this virtual but it shouldn't ever
    // have to be specialized. Call this before init
    // gets called or the window will create a new
    // node instead.
    virtual void addNode(fr::RequirementsManager::Node::PtrType node);
    // Returns the held node. This can be a nullptr.
    virtual fr::RequirementsManager::Node::PtrType getNode();
    // Enable/Disable debug button
    void setDisplayDebugButton(bool d);
    
    // Init populates the Node if it's empty and calls
    // init on it.
    virtual void init();

    // Returns stored node id
    std::string idString();

    void beginning() override;
    void begin() override;

    void end() override;
  };

  namespace Registration {

    template <>
    struct Record<NodeWindow> {
      using Type = NodeWindow;
      using NodeType = NodeWindow::NodeType;

      static constexpr char name[] = "Node";
      // This will be put in a "Test Node" top level menu item
      // when NodeEditorWindow encounters it
      static constexpr char topMenuName[] = "Test Node";

      static constexpr void init(std::shared_ptr<Type> window) {}

      static constexpr ImVec2 startingSize() { return ImVec2(300, 100); }
      
    };
        
  }
  
}
