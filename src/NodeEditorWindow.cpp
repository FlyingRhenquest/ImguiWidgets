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

#include <fr/ImguiWidgets.h>

namespace fr::Imgui {

  void NodeEditorWindow::Begin() {
      // I always want my NodeEditorWindow behind all the other windows
      // and I want it to always fill the entire OS Window that ImGui creates
      // as the background window.
      auto flags =
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_AlwaysVerticalScrollbar | 
        ImGuiWindowFlags_AlwaysHorizontalScrollbar;
      ImVec2 displaySize = ImGui::GetMainViewport()->Size;
      ImGui::SetNextWindowSize(displaySize);
      ImGui::SetNextWindowPos(ImVec2(0,0));
      ImGui::Begin(_label.c_str(), nullptr, flags);
  }
  
  void NodeEditorWindow::begin() {
      Parent::begin();
      if (ImGui::BeginMainMenuBar()) {

        // File Menu
        
        if (ImGui::BeginMenu("File")) {
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
  
}
