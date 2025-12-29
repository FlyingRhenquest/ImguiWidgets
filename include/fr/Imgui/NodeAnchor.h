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

#include <imgui.h>
#include <fr/Imgui/NodeWindow.h>
#include <fr/Imgui/Widget.h>
#include <fr/RequirementsManager.h>
#include <memory>
#include <unordered_map>

namespace fr::Imgui {

  class NodeAnchor;
  class NodeWindow;
  
  /**
   * NodeAnchor needs to know what type of anchor it is
   */
  enum class AnchorType {
    UNKNOWN,
    Up, // up list type
    Down, // Down list type
    // Used in CommitableNode
    Left, // left list type
    Right // right list type
  };

  /**
   * Drag and drop payload to connect nodes
   */

  struct NodeDragPayload {
    std::shared_ptr<NodeAnchor> dragSource;
    fr::RequirementsManager::Node::PtrType sourceNode;
    AnchorType anchorType;
  };
  
  /**
   * NodeAnchor displays an anchor that can be used to connect
   * nodes.
   *
   * NodeAnchor uses a draw list and its frame of reference is
   * screen coordinates, so use screenCoordinate when setting
   * up the center value from the parent window.
   */
  
  class NodeAnchor : public Widget, public std::enable_shared_from_this<NodeAnchor> {
    // Global default link color
    static const ImU32 _defaultLinkColor = IM_COL32(255,255,255,255);

    // Color the anchor usually is
    ImU32 _color;
    // Color when you hover the mouse over the anchor
    ImU32 _hoverColor;
    // Anchor Type
    AnchorType _type;
    // Center of Anchor
    ImVec2 _center;
    // Anchor radius
    float _radius;
    // Top left of the circle bounding box
    ImVec2 _min;
    // Bounding Box Size
    ImVec2 _bbsize;
    // Anchor hovered flag
    bool _hovered;
    // Indicates dragging is happening
    bool _dragging;
    // Parent node
    ImU32 _linkColor;
    fr::RequirementsManager::Node::PtrType _node;

    // Imgui doesn't seem to like passing shared pointers around
    // so I'm just going to hook into the drag/drop instrumentation
    // but stash the initiating drag data here. There should only
    // ever be one at a time.
    static std::shared_ptr<NodeDragPayload> _currentDrag;

    /**
     * These are all the connections for the node the parent window
     * holds. You can make new ones in the GUI but when the nodes are
     * initally loaded, the existing connections will have to be
     * populated by some external (to this NodeAnchor) entity.
     */
    std::unordered_map<std::string, std::shared_ptr<NodeDragPayload>> _connections;
    
  public:

    using Type = Widget;
    using PtrType = std::shared_ptr<NodeAnchor>;
    using Parent = Widget;
    
    NodeAnchor(const std::string& label, ImVec2 center, float radius, ImU32 color, ImU32 hoverColor, AnchorType t) :
      Parent(label),
      _color(color),
      _hoverColor(hoverColor),
      _type(t),
      _center(center),
      _radius(radius),
      _min(0,0),
      _bbsize(0,0),
      _hovered(false),
      _dragging(false),
      _linkColor(_defaultLinkColor) {
    }

    virtual ~NodeAnchor() {}

    void setParent(std::shared_ptr<NodeWindow> p);
    
    void setLinkColor(ImU32 color);

    ImVec2 getCenter() {
      return _center;
    }

    // Establish a connection with another NodeAnchor. When an
    // Imgui DragDropTarget payload is processed in begin,
    // the payload should be dropped into this anchor's
    // connections and then this node should create a NodeDragPayload
    // for itself and use establishConnection to provide it to the other
    // NodeAnchor.
    //
    // If a connection already exists, the connections should be removed
    // instead. removeConnection will handle that.
    //
    // The second parameter is used to prevent establishConnection
    // from trying to modify the node (Required if we're loading a graph)
    void establishConnection(std::shared_ptr<NodeDragPayload> connection,
                             bool modifyNode = true);

    // Remove a connection betrween two anchors
    void removeConnection(std::shared_ptr<NodeDragPayload> connection);

    // Draw connections between nodes
    void drawConnections();
    
    void begin() override;

    void end() override;

    // (Re)set center coordinate
    void setCenter(ImVec2 center);
    
  };
  
}
