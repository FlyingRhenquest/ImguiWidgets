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
 *
 */

#pragma once

#include <boost/signals2.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <format>
#include <fr/Imgui/AllWindows.h>
#include <fr/Imgui/GridWindow.h>
#include <fr/Imgui/RestLocator.h>
#include <fr/Imgui/WindowFactory.h>
#include <fr/types/Concepts.h>
#include <fr/types/Typelist.h>
#include <fstream>
#include <functional>
#include <imgui.h>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

// Optional includes
#ifndef NO_SQL
#include <fr/RequirementsManager/PqDatabase.h>
#include <fr/Imgui/WindowFactoryWindow.h>
#endif

#ifndef NO_LOAD_SAVE_JSON
#include <ImGuiFileDialog.h>
#endif

namespace fr::Imgui {

#ifndef NO_SQL
  template <typename WindowList>
  requires fr::types::IsUnique<WindowList>
  class WindowFactoryWindow;
#endif
  
  /**
   * This is the background window for nodes. This provides
   * menu operations to create, load and save nodes. It is designed
   * to fill entirety of the ImGui native window at all times.
   *
   */

  template <typename WindowList>
  requires fr::types::IsUnique<WindowList>
  class NodeEditorWindow : public GridWindow {

    // Iteminfo associates menu items and the function to create that window

    struct ItemInfo {
      std::string name;
      std::function<void()> create;
    };
    
    ImU32 _gridLineColor;
    float _squareSize;
    std::string _fileDialogLabel;
    ImVec2 _fileDialogSize;
    fr::Imgui::WindowFactory<WindowList> _factory;
    std::shared_ptr<RestLocator<WindowList>> _restWindow;

#ifndef NO_SQL    
    std::shared_ptr<WindowFactoryWindow<WindowList>> _databaseFactory;
#endif

    /**
     * menus holds a top level menu item name and a vector
     * of menu items that exist under them. Each of these drives
     * the creation of a different window type.
     */
    
    std::unordered_map<std::string, std::vector<std::shared_ptr<ItemInfo>>> _menus;
    
  public:
    using Type = NodeEditorWindow;
    using PtrType = std::shared_ptr<Type>;
    using Parent = GridWindow;

    boost::signals2::signal<void()> exitEvent;

    std::shared_ptr<fr::RequirementsManager::ThreadPool<fr::RequirementsManager::WorkerThread>> threadpool;

    NodeEditorWindow(const std::string &label = "Node Editor") : Parent(label) {
#ifndef NO_SQL      
      _databaseFactory = std::make_shared<WindowFactoryWindow<WindowList>>();
      threadpool = std::make_shared<fr::RequirementsManager::ThreadPool<fr::RequirementsManager::WorkerThread>>();
      threadpool->startThreads(DEFAULT_THREADPOOL_SIZE);
#endif
      _restWindow = std::make_shared<RestLocator<WindowList>>();
      _fileDialogLabel = getUniqueLabel("FileDialog");
      _fileDialogSize.x = 600;
      _fileDialogSize.y = 400;
    }

    template <typename List>
    requires fr::types::IsUnique<List>
    void buildMenus() {
      using Rest = List::tail;
      auto info = std::make_shared<ItemInfo>();
      info->name = Registration::Record<typename List::head::type>::name;
      info->create = [&]() {
        Registration::createWindow<NodeEditorWindow, typename List::head::type>(*this);
      };
      
      _menus[Registration::Record<typename List::head::type>::topMenuName].push_back(info);
      if constexpr(!std::is_void_v<typename Rest::head::type>) {
        buildMenus<Rest>();
      }
    }

    virtual ~NodeEditorWindow() {}

    void beginning() override {
      _factory.addEditorWindow(this);
      _restWindow->addEditorWindow(this);
      this->add(getUniqueLabel("##RestWindow"), _restWindow);
#ifndef NO_SQL
      this->add(getUniqueLabel("##DatabaseFactory"), _databaseFactory);
      _databaseFactory->addEditorWindow(this);
#endif
      Parent::beginning();
    }
    
    void Begin() override {
      // I always want my NodeEditorWindow behind all the other windows
      // and I want it to always fill the entire OS Window that ImGui creates
      // as the background window.
      auto flags = ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_AlwaysVerticalScrollbar |
        ImGuiWindowFlags_AlwaysHorizontalScrollbar;
      ImVec2 displaySize = ImGui::GetMainViewport()->Size;
      ImGui::SetNextWindowSize(displaySize);
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::Begin(_label.c_str(), nullptr, flags);
    }    
    
    void begin() override {
      Parent::begin();
      if (ImGui::BeginMainMenuBar()) {
      
        // File Menu
        
        if (ImGui::BeginMenu("File")) {
#ifndef NO_SQL
          if (ImGui::MenuItem("Load from Database")) {
            _databaseFactory->setShow(true);
          }
#endif
#ifndef NO_LOAD_SAVE_JSON          
          if (ImGui::MenuItem("Load from JSON")) {
            ImGuiFileDialog::Instance()->OpenDialog(_fileDialogLabel, "Load From JSON", ".json");
          }
#endif
          if (ImGui::MenuItem("Query REST Service")) {
            _restWindow->setShow(true);
          }
          if (ImGui::MenuItem("Exit")) {
            exitEvent();
          }
          
          ImGui::EndMenu();
        }
        
        // Render Registration-based windows
        for (auto [item, infoVec] : _menus) {
          if (ImGui::BeginMenu(item.c_str())) {
            for (auto info : infoVec) {
              if (ImGui::MenuItem(info->name.c_str())) {
                info->create();
              }
            }
            ImGui::EndMenu();
          }
        }
      }
      ImGui::EndMainMenuBar();

#ifndef NO_LOAD_SAVE_JSON      
      ImVec4 fileDialogBackground{0,0,0,255};
      ImGui::PushStyleColor(ImGuiCol_WindowBg, fileDialogBackground);
      if (ImGuiFileDialog::Instance()->Display(_fileDialogLabel, ImGuiWindowFlags_NoCollapse, _fileDialogSize, _fileDialogSize)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
          std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
          std::shared_ptr<fr::RequirementsManager::Node> node;
          
          std::ifstream streamIn(filePathName);
          {
            cereal::JSONInputArchive archive(streamIn);
            archive(node);
          }
          if (node) {
            _factory.add(node);
          }
        }
        ImGuiFileDialog::Instance()->Close();
      }
      ImGui::PopStyleColor();
#endif
    }

  };
  
}
