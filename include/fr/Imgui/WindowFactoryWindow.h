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

#include <fr/Imgui/WindowFactory.h>
#include <fr/ImguiWidgets.h>
#include <fr/RequirementsManager/GraphNodeLocator.h>

namespace fr::Imgui {

  template <typename WindowList>
  requires fr::types::IsUnique<WindowList>
  class WindowFactory;
  
  template <typename WindowList>
  requires fr::types::IsUnique<WindowList>
  class WindowFactoryWindow : public Window {

    // Imgui likes to work with raw pointers, so it can set _show
    // to false at some point, and that gets my window display logic
    // out of sync. So I need use _displayWindow to keep the individual
    // frames in sync, only calling ImGui::Begin on the window if _displayWindow
    // is set and synching _displayWindow to _show at the end of the window logic
    bool _show;
    bool _displayWindow;

    // Locates Nodes to load
    fr::RequirementsManager::GraphNodeLocator _locator;
    fr::Imgui::WindowFactory<WindowList> _factory;

    std::string _tableName;
    std::string _uuidColumn;
    std::string _titleColumn;
    std::string _load;
    int _selectedRow;
  public:
    using Parent = Window;
    using Type = WindowFactoryWindow;
    using PtrType = std::shared_ptr<Type>;    
    
    WindowFactoryWindow(const std::string &label = "Load Graph") : Parent(label), _show(false), _displayWindow(false) {
      _tableName = getUniqueLabel("##LoadGraphTable");
      _uuidColumn = getUniqueLabel("UUID");
      _titleColumn = getUniqueLabel("Title");
      _load = getUniqueLabel("Load");
      _selectedRow = 0;
    }
    virtual ~WindowFactoryWindow() {}

    // Forward addEditorWindow on to factory
    void addEditorWindow(NodeEditorWindow<WindowList> *editor) {
      _factory.addEditorWindow(editor);
    }

    void setShow(bool show) {
      // Re-query database on show
      _locator.query();
      _show = show;
      _displayWindow = show;
    }

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
        if (ImGui::BeginTable(_tableName.c_str(), 3)) {
          ImGui::TableSetupColumn(_load.c_str());
          ImGui::TableSetupColumn(_uuidColumn.c_str());
          ImGui::TableSetupColumn(_titleColumn.c_str());
          ImGui::TableHeadersRow();
          
          int row = 0;
          // Each node in locator.nodes is a row
          for(auto [uuid,title] : _locator.nodes) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            std::string loadButton("Load##");
            loadButton.append(uuid);
            if (ImGui::Button(loadButton.c_str())) {
              _factory.load(uuid);
            }
            ImGui::TableNextColumn();
            ImGui::Text(uuid.c_str());
            ImGui::TableNextColumn();
            ImGui::Text(title.c_str());
          }
          
          ImGui::EndTable();
        }
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
