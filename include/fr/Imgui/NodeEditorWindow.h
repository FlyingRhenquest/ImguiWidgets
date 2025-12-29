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
#include <functional>
#include <fr/ImguiWidgets.h>
#include <fr/Imgui/WindowFactoryWindow.h>
#include <fr/types/Typelist.h>
#include <fr/types/Concepts.h>
#include <imgui.h>
#include <string>
#include <memory>
#include <unordered_map>

namespace fr::Imgui {

  template <typename WindowList>
  requires fr::types::IsUnique<WindowList>
  class WindowFactoryWindow;
  
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
    std::shared_ptr<WindowFactoryWindow<WindowList>> _databaseFactory;

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

    NodeEditorWindow(const std::string &label = "Node Editor") : Parent(label) {
      _databaseFactory = std::make_shared<WindowFactoryWindow<WindowList>>();      
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
      this->add(getUniqueLabel("##DatabaseFactory"), _databaseFactory);
      _databaseFactory->addEditorWindow(this);
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
          if (ImGui::MenuItem("Load from Database")) {
            _databaseFactory->setShow(true);
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
    }
    
  };
  
}
