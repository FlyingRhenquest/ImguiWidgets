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

#include <fr/Imgui/Window.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace fr::Imgui::Registration {

  /**
   * Record provides a way for individual windows
   * to register how they should be created in a manner that
   * NodeEditorWindow can consume. This is the NotFound version
   * of Record. Each created window can specialize
   * this structure following this API and NodeEditorWindow can
   * just iterate through a typelist of these to generate the
   * UI.
   */

  template <typename WindowType>
  struct Record {
    // Type holds the type of window to create
    using Type = void;
    // NodeType holds the type of node to create
    using NodeType = void;

    // TopMenuName should be the name of the top level menu
    // item this window should be created in
    static constexpr char topMenuName[] = "NOTFOUND";
    static constexpr char name[] = "NOTFOUND";

    /**
     * You can use init in your registration record to add specific
     * initializaiton to all new windows of this type
     */
    
    static constexpr void init(std::shared_ptr<WindowType> window) {}

    /**
     * Returns initial starting size of the window
     */
    
    static constexpr ImVec2 startingSize() { return ImVec2(300,200); }

    
  };

  
  /**     
   * concept has_add is looking for a NodeEditorWindow-like object
   * that implements add with a const std::string& id and a
   * shared pointer to a Window
   */

  template <typename Editor>
  concept has_add = requires(Editor object,
                             const std::string& id,
                             std::shared_ptr<fr::Imgui::Window> window)
  {
    { object.add(id, window) };
  };

  /**
   * concept has_init is looking for a WindowType that implements
   * init. If your object derives from Window, it will satisfy
   * this requirement.
   */

  template <typename WindowType>
  concept has_init = requires(WindowType object)
  {
    { object.init() };
  };

  /**
   * createWindow creates a window of WindowType and adds it
   * to an Editor of EditorType.
   */

  template <typename EditorType, typename WindowType>
  requires has_init<WindowType> && has_add<EditorType>
  void createWindow(EditorType& editor) {
    auto window = std::make_shared<WindowType>();
    window->init();
    auto startingSize = fr::Imgui::Registration::Record<WindowType>::startingSize();
    window->setStartingSize(startingSize.x, startingSize.y);
    fr::Imgui::Registration::Record<WindowType>::init(window);
    editor.add(window->idString(), window);
  }

    
}
