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

#include <fr/Imgui/NodeAnchor.h>
#include <iostream>

namespace fr::Imgui {

  std::shared_ptr<NodeDragPayload> NodeAnchor::_currentDrag;
  
  void NodeAnchor::setParent(std::shared_ptr<NodeWindow> p) {
    Parent::setParent(p);
    _node = p->getNode();
  }

  void NodeAnchor::setLinkColor(ImU32 color) {
    _linkColor = color;
  }

  void NodeAnchor::establishConnection(std::shared_ptr<NodeDragPayload> connection) {
    // Check _connections and don't do anything at this point if the
    // node ID is already in the connections (This keeps us from getting
    // into an infinite loop of establishing connections)
    std::cout << "Examining " << connection->sourceNode->idString() << std::endl;
    // Reject connection if the far node is the wrong type
    if ((_type == AnchorType::Up && connection->anchorType != AnchorType::Down) ||
        (_type == AnchorType::Down && connection->anchorType != AnchorType::Up) ||
        (_type == AnchorType::Left && connection->anchorType != AnchorType::Right) ||
        (_type == AnchorType::Right && connection->anchorType != AnchorType::Left)) {
      std::cout << "Reject connection: wrong anchor type" << std::endl;
    return;
    }
      
    if (!_connections.contains(connection->sourceNode->idString())) {
      auto otherSide = std::make_shared<NodeDragPayload>();
      _connections[connection->sourceNode->idString()] = connection;
      otherSide->dragSource = shared_from_this();
      otherSide->sourceNode = _node;
      otherSide->anchorType = _type;
      if (_type == AnchorType::Up) {
        _node->addUp(connection->sourceNode);
      } else if (_type == AnchorType::Down) {
        _node->addDown(connection->sourceNode);
      }
      connection->dragSource->establishConnection(otherSide);
    }
  }

  void NodeAnchor::removeConnection(std::shared_ptr<NodeDragPayload> connection) {
    // If connections contain the node then remove it, otherwise
    // do nothing
    if (_connections.contains(connection->sourceNode->idString())) {
      auto otherSide = std::make_shared<NodeDragPayload>();
      _connections.erase(connection->sourceNode->idString());
      otherSide->dragSource = shared_from_this();
      otherSide->sourceNode = _node;
      otherSide->anchorType = _type;
      if (_type == AnchorType::Up) {
        _node->removeUp(connection->sourceNode);
      } else if (_type == AnchorType::Down) {
        _node->removeDown(connection->sourceNode);
      }
      connection->dragSource->removeConnection(otherSide);
    }
  }

  void NodeAnchor::drawConnections() {
    for (auto [id, connection] : _connections) {
      ImDrawList* drawList = ImGui::GetForegroundDrawList();
      ImVec2 p1 = _center;
      ImVec2 p4 = connection->dragSource->getCenter();
      ImVec2 p2 = ImVec2(p1.x + 50.0f, p1.y);
      ImVec2 p3 = ImVec2(p4.x - 50.0f, p4.y);
      drawList->AddBezierCubic(p1, p2, p3, p4, _linkColor, 2, 4);
    }
  }  

  void NodeAnchor::begin() {
    if (!_node) {
      if (auto p = std::dynamic_pointer_cast<NodeWindow>(_parent)) {
      _node = p->getNode();
      }
    }

    drawConnections();
    
    // Color to render this time around
    ImU32 color;
    // Set an invisible button to detect hover state
    ImGui::SetCursorScreenPos(_min);
    ImGui::InvisibleButton(_label.c_str(), _bbsize);

    if (ImGui::IsItemHovered()) {
      color = _hoverColor;
      _hovered = true;
    } else {
      color = _color;
      _hovered = false;
    }

    // Handle drags
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
      if (!_currentDrag || !_dragging) {
        std::cout << "BeginDropSource: " << _node->idString() << std::endl;
        _currentDrag = std::make_shared<NodeDragPayload>();
        _currentDrag->dragSource = shared_from_this();
        _currentDrag->sourceNode = _node;
        _currentDrag->anchorType = _type;
        _dragging = true;
      }
      ImGui::SetDragDropPayload("NodeLinkPayload", _currentDrag.get(), sizeof(NodeDragPayload*));
      
      ImGui::EndDragDropSource();
    }

    // Handle drawing the bezier between the drag start point and
    // where the mouse is now
    if (_dragging && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
      // Draw a bezier to the current mouse location while we're dragging
      ImVec2 cursor = ImGui::GetIO().MousePos;
      ImDrawList* drawList = ImGui::GetForegroundDrawList();
      ImVec2 p1 = _center;
      ImVec2 p4 = cursor;
      ImVec2 p2 = ImVec2(p1.x + 50.0f, p1.y);
      ImVec2 p3 = ImVec2(p4.x - 50.0f, p4.y);
      drawList->AddBezierCubic(p1, p2, p3, p4, _linkColor, 2, 4);
    } else if (_dragging && !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
      std::cout << "Dragging stopped" << std::endl;
      _dragging = false;
    }

    // Handle Drops
    if (ImGui::BeginDragDropTarget()) {
      std::cout << "BeginDragDropTarget to: " << _node->idString() << std::endl;
      if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("NodeLinkPayload")) {
        std::shared_ptr<NodeDragPayload> connection;
        connection.swap(_currentDrag);
        
        std::cout << "Accepted payload from: " << connection->sourceNode->idString() << std::endl;
        // If we already have a link to the payload, remove the connection instead
        // of creating it.
        if (!_connections.contains(connection->sourceNode->idString())) {
          establishConnection(connection);
        } else {
          removeConnection(connection);
        }        
      }

      ImGui::EndDragDropTarget();
    }
      
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    drawList->AddCircleFilled(_center, _radius, color);
  }

  void NodeAnchor::end() {}

  void NodeAnchor::setCenter(ImVec2 center) {
    _center = center;
    _min.x = center.x - _radius;
    _min.y = center.y - _radius;
    _bbsize.x = _radius * 2;
    _bbsize.y = _radius * 2;
  }
  
}
