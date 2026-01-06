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
#include <fr/RequirementsManager/GraphNode.h>
#include <fstream>
#include <iostream>
#include <type_traits>

#ifndef NO_SQL
#include <fr/RequirementsManager/PqDatabase.h>
#endif

#ifndef NO_JSON_LOAD_SAVE
#include <ImGuiFileDialog.h>
#endif

namespace fr::Imgui {

  class GraphNodeWindow : public NodeWindow {
    static const size_t titleTextLen = 201;
    static const size_t urlLen = 301;
    char _titleText[titleTextLen];
    char _url[urlLen];
    std::string _titleTextLabel;
    std::string _saveLabel;
    std::string _jsonSaveLabel;
    std::string _fileLabel;
    std::string _fileDialogLabel;
    std::string _restSaveLabel;
    std::string _restSaveWindowLabel;
    std::string _saveTextBoxLabel;
    std::string _restSaveButtonLabel;
    bool _display;
    bool _showPopup;
    
    ImVec2 _fileDialogSize;
    // GraphNodeFactory raw pointer -- I don't own this and can
    // be sure it will exist for the duration of the application.
    // I use this to POST the graph to REST, if it's not null.
    fr::RequirementsManager::GraphNodeFactory *_factory;
    using WorkerThread = fr::RequirementsManager::WorkerThread;
    using ThreadPool = fr::RequirementsManager::ThreadPool<WorkerThread>;
#ifndef NO_SQL
    using SaveNodesNode = fr::RequirementsManager::SaveNodesNode<WorkerThread>;

    std::shared_ptr<SaveNodesNode> _saver;
#endif
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
      : Parent(title),
        _factory(nullptr)
    {
      memset(_titleText, '\0', titleTextLen);
      memset(_url, '\0', urlLen);
      _titleTextLabel = getUniqueLabel("##Title");
      _saveLabel = getUniqueLabel("Save to Database");
      _jsonSaveLabel = getUniqueLabel("Save to JSON File");
      _fileLabel = getUniqueLabel("File");
      _fileDialogLabel = getUniqueLabel("FileDialog");
      _restSaveLabel = getUniqueLabel("Save to REST Service");
      _restSaveWindowLabel = getUniqueLabel("Save to REST");
      _saveTextBoxLabel = getUniqueLabel("##URL");
      _restSaveButtonLabel = getUniqueLabel("Save");
      _fileDialogSize.x = 600;
      _fileDialogSize.y = 400;
      _showPopup = false;
      _display = false;
    }

    virtual ~GraphNodeWindow() {}

    void setFactory(fr::RequirementsManager::GraphNodeFactory* factory) {
      _factory = factory;
    }
    
    void init() override {
      if (!_node) {
        _node = std::make_shared<fr::RequirementsManager::GraphNode>();
        _node->init();
      }
      setTitleText();
      Parent::init();
    }

    void Begin() override {
      // Override Begin so we can set this window up with a menu bar.
      ImGui::Begin(_label.c_str(), nullptr, ImGuiWindowFlags_MenuBar);      
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
     
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu(_fileLabel.c_str())) {
#ifndef NO_SQL
          if (ImGui::MenuItem(_saveLabel.c_str())) {
            // Save to Database
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
#endif
#ifndef NO_LOAD_SAVE_JSON
          if (ImGui::MenuItem(_jsonSaveLabel.c_str())) {
            ImGuiFileDialog::Instance()->OpenDialog(_fileDialogLabel, "Save to JSON", ".json");
          }
#endif
          if (_factory) {
            if (ImGui::MenuItem(_restSaveLabel.c_str())) {
              _display = true;
            }
          }
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }

#ifndef NO_LOAD_SAVE_JSON      
      if (ImGuiFileDialog::Instance()->Display(_fileDialogLabel, ImGuiWindowFlags_NoCollapse, _fileDialogSize, _fileDialogSize)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
          std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
          std::ofstream streamOut(filePathName);
          {
            cereal::JSONOutputArchive archive(streamOut);
            archive(_node);
          }
        }
        ImGuiFileDialog::Instance()->Close();
      }      
#endif

      if (_display) {
        _showPopup = true;
        ImGui::Begin(_restSaveWindowLabel.c_str(), &_showPopup, ImGuiWindowFlags_AlwaysAutoResize);
          
        ImGui::Text("URL: ");
        ImGui::SameLine();
        ImGui::InputText(_saveTextBoxLabel.c_str(), _url, urlLen - 1);
        if (ImGui::Button(_restSaveButtonLabel.c_str())) {
          _factory->post(_url, _node);
          _display = false;
        }
        ImGui::End();
        _showPopup = false;
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
