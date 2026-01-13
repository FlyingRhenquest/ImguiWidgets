/**
 * Copyright 2026 Bruce Ide
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

#include <fteng/signals.hpp>
#include <fr/RequirementsManager/RestFactoryApi.h>
#include <fr/Imgui/AllWindows.h>
#include <fr/Imgui/Window.h>
#include <fr/Imgui/WindowFactory.h>
#include <mutex>
#include <vector>

#ifdef __EMSCRIPTEN__
#include <fr/RequirementsManager/EmscriptenRestFactory.h>
#endif

#ifndef __EMSCRIPTEN__
#include <fr/RequirementsManager/PistacheRestFactory.h>
#endif

namespace fr::Imgui {

  /**
   * RestLocator accepts a URL (currently you have to type in manually)
   * to a REST server and queries that URL for a list of graph nodes
   * it has available. It displays those nodes and allows you to
   * select ones to load into the editor.
   *
   * TODO: Implement a "save" button for REST in GraphNodeWindow.
   *       The server already supports this.
   */

  // Anything that creates Windows requires WindowList. This should be the
  // AllWindowList I declare in AllWindows.h
  template <typename WindowList>
  requires fr::types::IsUnique<WindowList>
  class RestLocator : public Window {
    // Stores the URL to query
    static const size_t urlLen = 301;
    // Only show window when "show" is true. Will also use _displayWindow
    // to keep menu frames in sync.
    bool _show;
    bool _displayWindow;    

    // Factories used to query REST services
#ifdef __EMSCRIPTEN__
    // Emscripten version of these factories are just references to
    // a singleton object. 
    fr::RequirementsManager::EmscriptenServerLocatorFactory& _locatorFactory = fr::RequirementsManager::EmscriptenServerLocatorFactory::instance();
    fr::RequirementsManager::EmscriptenGraphNodeFactory& _graphFactory = fr::RequirementsManager::EmscriptenGraphNodeFactory::instance();
#endif

#ifndef __EMSCRIPTEN__
    // Define currently null objects that exist but don't do anything
    fr::RequirementsManager::PistacheLocatorNodeFactory _locatorFactory;
    fr::RequirementsManager::PistacheGraphNodeFactory _graphFactory;
#endif    
    
    // Track subscriptions so we can unsubscribe from factories if we ever need to.
    std::vector<fteng::connection> _subscriptions;
    // Window factory used to create windows
    std::shared_ptr<WindowFactory<WindowList>> _windowFactory;

    // Mutex to control access to _graphs so the vector doesn't get updated
    // while we're trying to display it. This will get locked once per frame
    // when the window is displaying, so closing it when you don't need it
    // might help if noticably impacts performance (It shouldn't, though.)
    std::mutex _graphsMutex;
    // Store the server locator nodes for the graphs we can load
    std::vector<std::shared_ptr<fr::RequirementsManager::ServerLocatorNode>> _graphs;
    
    // Labels for various controls

    char _url[urlLen];
    std::string _urlLabel;

    // Label for the text box to input the server URL into
    std::string _serverInputLabel;
    // Label to refresh the contents of the window
    std::string _refreshButtonLabel;
    // Label to display if the backing factories don't
    // actually do anything
    std::string _noFactoriesLabel;
    // Text label to store error messages in
    std::string _errorMessage;
    // Label for our loading table
    std::string _tableLabel;
    // Column names
    std::string _uuidColumn;
    std::string _titleColumn;
    std::string _loadColumn;
    
    void subscribe() {
      auto locatorAvailableSub =
        _locatorFactory.available.connect([&](std::shared_ptr<fr::RequirementsManager::ServerLocatorNode> node) {
          std::lock_guard<std::mutex> lock(_graphsMutex);
          std::cout << "locator pushing back " << node->idString() << std::endl;
          _graphs.push_back(node);
      });
      auto displayGraphSub =
        _graphFactory.available.connect([&](std::shared_ptr<fr::RequirementsManager::Node> node) {
          std::cout << "Creating windows for " << node->idString() << std::endl;
          _windowFactory->add(node);
        });
      auto locatorFailSub =
        _locatorFactory.error.connect([](const std::string& message) {
          std::cout << "Locator error: " << message << std::endl;
        });
      auto graphFailSub =
        _graphFactory.error.connect([](const std::string& message) {
          std::cout << "Graph error: " << message << std::endl;
        });
      _subscriptions.push_back(std::move(locatorAvailableSub));
      _subscriptions.push_back(std::move(displayGraphSub));
      _subscriptions.push_back(std::move(locatorFailSub));
      _subscriptions.push_back(std::move(graphFailSub));
    }

    void unsubscribe() {
      // Destroying the connections automatically unsubscribes from them
      _subscriptions.clear();
    }

  public:
    using Type = RestLocator;
    using PtrType = std::shared_ptr<Type>;
    using Parent = Window;
    
    RestLocator(const std::string& label = "REST Service Locator") : Parent(label) {
      memset(_url, '\0', urlLen);
      _show = false;
      _displayWindow = false;

      _urlLabel = getUniqueLabel("##URL");
      _serverInputLabel = getUniqueLabel("##serverInput");
      _refreshButtonLabel = getUniqueLabel("Refresh");
      _tableLabel = getUniqueLabel("##LoadTable");
      _uuidColumn = getUniqueLabel("UUID");
      _titleColumn = getUniqueLabel("Title");
      _loadColumn = getUniqueLabel("Load");
      
      // If your window factory doesn't open windows, I probably forgot to
      // call addEditorWindow on this object in main.cpp
      _windowFactory = std::make_shared<WindowFactory<WindowList>>();
      // This label is always defined but only displays if the build
      // type doesn't have a locator factory that actually does something.
      // (non-emscripten builds, currently)
      _noFactoriesLabel = "This object doesn't have working factories and will not actually load graphs.";
      subscribe();
      setStartingSize(350,400);
    }

    virtual ~RestLocator() {
      // Drop all subscriptions (Not doing this can cause a crash if this
      // object is deleted before the rest of the application stops)
      unsubscribe();
    }

    // Passes the NodeEditorWindow to windowFactory so that windowFactory
    // can add the windows it creates to NodeEditorWindow.
    void addEditorWindow(NodeEditorWindow<WindowList> *editor) {
      _windowFactory->addEditorWindow(editor);
      _windowFactory->addNodeFactory(&_graphFactory);
      editor->setGraphNodeFactory(&_graphFactory);
    }

    // Enables displaying/closing this window
    void setShow(bool show) {
      _show = true;
      _displayWindow = true;
    }

    // Lets you check if the DisplayWindow is being displayed
    bool getShow() {
      return _show;
    }

    void Begin() override {
      if (_displayWindow) {
        ImGui::Begin(_label.c_str(), &_show);
      }
    }

    void begin() override {
      if (_displayWindow) {
        Parent::begin();
        ImGui::Text("URL: ");
        ImGui::SameLine();
        ImGui::InputText(_urlLabel.c_str(),
                         _url,
                         urlLen - 1);

        // We'll be accessing _graphs for few lines, so lock it.
        std::lock_guard<std::mutex> lock(_graphsMutex);
        if (ImGui::Button(_refreshButtonLabel.c_str())) {
          // Clear out the graphs vector so we don't end up with thousands of
          // load buttons in our table.
          std::cout << "Requesting loaderfactory fetch " << _url << std::endl;
          _graphs.clear();
          _locatorFactory.fetch(_url);
        }
        
        if (ImGui::BeginTable(_tableLabel.c_str(), 3)) {
          ImGui::TableSetupColumn(_loadColumn.c_str());
          ImGui::TableSetupColumn(_uuidColumn.c_str());
          ImGui::TableSetupColumn(_titleColumn.c_str());

          for (auto _graph : _graphs) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            std::string loadButton("Load##");
            loadButton.append(_graph->getGraphUuid());
            if (ImGui::Button(loadButton.c_str())) {
              _graphFactory.fetch(_graph->getGraphAddress());
            }
            ImGui::TableNextColumn();
            ImGui::Text(_graph->getGraphUuid().c_str());
            ImGui::TableNextColumn();
            ImGui::Text(_graph->getGraphTitle().c_str());
          }
        }
        ImGui::EndTable();
      }
    }

    void end() override {
      if (_displayWindow) {
        Parent::end();
        _displayWindow = _show;
      }
    }
    
  };
  
}
