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
#include <fr/Imgui/NodeEditorWindow.h>
#include <fr/RequirementsManager/PqDatabase.h>
#include <fr/RequirementsManager/GraphNode.h>

namespace fr::Imgui {

  class GraphNodeWindow : public NodeWindow {
    static const size_t titleTextLen = 201;
    char _titleText[titleTextLen];
    std::string _titleTextLabel;
    std::string _saveLabel;
    using WorkerThread = fr::RequirementsManager::WorkerThread;
    using ThreadPool = fr::RequirementsManager::ThreadPool<WorkerThread>;
    using SaveNodesNode = fr::RequirementsManager::SaveNodesNode<WorkerThread>;
    std::shared_ptr<SaveNodesNode> _saver;
    std::shared_ptr<ThreadPool> _threadpool;
    
    void setTitleText() {
      auto node = dynamic_pointer_cast<fr::RequirementsManager::GraphNode>(_node);
      if (node) {
        strncpy(_titleText, node->getTitle().c_str(), titleTextLen - 1);
      }
    }

  public:

    using Type = GraphNodeWindow;
    using Parent = NodeWindow;
    using PtrType = std::shared_ptr<Type>;
    using NodeType = fr::RequirementsManager::GraphNode;

    GraphNodeWindow(std::string title = "GraphNode")
      : Parent(title) {
      memset(_titleText, '\0', titleTextLen);
      _titleTextLabel = getUniqueLabel("##Title");
      _saveLabel = getUniqueLabel("Save to Database");
    }

    virtual ~GraphNodeWindow() {}

    void init() override {
      if (!_node) {
        _node = std::make_shared<fr::RequirementsManager::GraphNode>();
      }
      setTitleText();
      Parent::init();
    }

    void begin() override {
      Parent::begin();
      auto inputTextFlags = ImGuiInputTextFlags_ReadOnly;
      if (_editable) {
        inputTextFlags = (ImGuiInputTextFlags_) 0;
      }

      ImGui::Text("Title: ");
      ImGui::SameLine();
      if (ImGui::InputText(_titleTextLabel.c_str(), _titleText, titleTextLen - 1, inputTextFlags)) {
        auto node = dynamic_pointer_cast<fr::RequirementsManager::GraphNode>(_node);
        if (node) {
          node->setTitle(_titleText);
        }
      }

      if (ImGui::Button(_saveLabel.c_str())) {
        // Right now I'm just going to set all the nodes' change flags to true
        // and kick off a save. I'll need to do a visual indicator to indicate that
        // the save actually worked and clean this up a bit. It'd be nice to only
        // display this button if something in the graph has actually changed.
        _node->changed = true;
        _node->traverse([](fr::RequirementsManager::Node::PtrType nextNode) {
          nextNode->changed = true;
        });
        // If our parent is a NodeEditorWindow, get its threadpool
        auto parentNodeEditor = std::dynamic_pointer_cast<NodeEditorWindow<AllWindowList>>(_parent);
        if (!_threadpool && parentNodeEditor) {
          _threadpool = parentNodeEditor->threadpool;
        } else {
          if (!_threadpool) {
            _threadpool = std::make_shared<ThreadPool>();
            _threadpool->startThreads(4);
          }
        }
        if (!_saver) {
          _saver = std::make_shared<SaveNodesNode>(_node);
        }
        // TODO: Set saver's complete callback up to indicate data was saved
        _threadpool->enqueue(_saver);
      }
    }
    
  };

  namespace Registration {

    template <>
    struct Record<GraphNodeWindow> {
      using Type = GraphNodeWindow;
      using NodeType = GraphNodeWindow::NodeType;

      static constexpr char name[] = "Graph Node";
      static constexpr char topMenuName[] = "Utility Nodes";

      static constexpr void init(std::shared_ptr<Type> window) {
        window->setDisplayDebugButton(true);
      }

      static constexpr ImVec2 startingSize() { return ImVec2(300,300); }
      
    };
    
  }
  
}
