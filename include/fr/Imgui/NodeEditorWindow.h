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
#include <fr/types/Typelist.h>
#include <fr/types/Concepts.h>
#include <imgui.h>
#include <string>
#include <memory>
#include <unordered_map>

namespace fr::Imgui {

  /**
   * This is the background window for nodes. This provides
   * menu operations to create, load and save nodes. It is designed
   * to fill entirety of the ImGui native window at all times.
   *
   */
  
  class NodeEditorWindow : public GridWindow {

    // Iteminfo associates menu items and the function to create that window

    struct ItemInfo {
      std::string name;
      std::function<void()> create;
    };
    
    ImU32 _gridLineColor;
    float _squareSize;

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

    void Begin() override;

    void begin() override;
    
  };
  
}
