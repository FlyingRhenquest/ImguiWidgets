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

#include <fr/Imgui/NodeWindow.h>

namespace fr::Imgui {

bool NodeWindow::_defaultEditable = false;
bool NodeWindow::_defaultDisplayEditabilityCheckbox = true;

NodeWindow::NodeWindow(const std::string &label)
    : Parent(label), _editable(_defaultEditable),
      _displayEditable(_defaultDisplayEditabilityCheckbox),
      _displayDebugButton(false), _initted(false) {
  memset(_idText, '\0', idTextLen);
  ImU32 white = IM_COL32(255, 255, 255, 255);
  ImU32 red = IM_COL32(255, 0, 0, 255);
  _upAnchor = std::make_shared<NodeAnchor>("##UpAnchor", ImVec2(0, 0), 5.0,
                                           white, red, AnchorType::Up);
  _downAnchor = std::make_shared<NodeAnchor>("##DownAnchor", ImVec2(0, 0), 5.0,
                                             white, red, AnchorType::Down);
  _enableEditingLabel = getUniqueLabel("Enable Editing");
  _nodeIdLabel = getUniqueLabel("##ID:");
  _debugButtonLabel = getUniqueLabel("DEBUG!");
}

void NodeWindow::addNode(fr::RequirementsManager::Node::PtrType node) {
  _node = node;
}

fr::RequirementsManager::Node::PtrType NodeWindow::getNode() { return _node; }

void NodeWindow::setDisplayDebugButton(bool d) { _displayDebugButton = d; }

void NodeWindow::init() {
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
  auto sub =
      this->moved.connect([&](Window::PtrType parent, const ImVec2 &pos) {
        _min = pos;
        // This always places the upListcenter anchor
        // 25 pixels below the titlebar
        ImVec2 upListCenter(_currentSize.x / 2.0, 25.0);
        _upAnchor->setCenter(screenCoordinate(upListCenter));
        // There seems to be some slop on the bottom, so
        // _currentSize is more than adequate as an offset from
        // there (I should check and see if I can make it a positive
        // number and exceed the bound down into the slop.
        ImVec2 downListCenter(_currentSize.x / 2.0, _currentSize.y);
        _downAnchor->setCenter(screenCoordinate(downListCenter));
      });

  _subscriptions.push_back(sub);

  _initted = true;
}

std::string NodeWindow::idString() {
  std::string ret;
  if (_node) {
    ret = _node->idString();
  }
  return ret;
}

void NodeWindow::beginning() {
  if (!_node || !_initted) {
    init();
  }
  if (!strlen(_idText)) {
    setIdText();
  }
  Parent::beginning();
}

void NodeWindow::begin() {
  Parent::begin();

  if (_displayDebugButton) {
    if (ImGui::Button(_debugButtonLabel.c_str())) {
      std::cout << std::endl << _node->to_json() << std::endl;
    }
  }

  if (_displayEditable) {
    ImGui::Checkbox(_enableEditingLabel.c_str(), &_editable);
  }
  ImGui::Text("Node ID: ");
  ImGui::SameLine();
  ImGui::InputText(_nodeIdLabel.c_str(), _idText, idTextLen - 1,
                   ImGuiInputTextFlags_ReadOnly);
}

void NodeWindow::end() { Parent::end(); }
} // namespace fr::Imgui
