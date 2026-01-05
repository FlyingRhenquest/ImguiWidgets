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
#include <imgui.h>
#include <string>
#include <memory>

namespace fr::Imgui {

  /**
   * A window with a grid drawn on it.
   */
  
  class GridWindow : public Window {
  protected:
    float _squareSize;
    unsigned int _gridLineColor;

  public:

    using PtrTrype = std::shared_ptr<GridWindow>;
    using Parent = Window;

    GridWindow(const std::string &label) : Window(label), _squareSize(20.0) {
      // Default grid line color is a fairly light gray
      _gridLineColor = IM_COL32(211,211,211,50);
      setBackgroundColor(0.27f, 0.31f, 0.33f, 1.0f);
    }

    virtual ~GridWindow() {}

    void setSquareSize(float size) {
      _squareSize = size;
    }

    void setGridLineColor(unsigned char r, unsigned char g, unsigned char b,  unsigned char a) {
      _gridLineColor = IM_COL32(r,g,b,a);
    }

    virtual void drawGrid() {
      ImDrawList *drawList = ImGui::GetWindowDrawList();

      for (float y = 0.0; y < _currentSize.y; y += _squareSize) {
        drawList->AddLine(screenCoordinate(0, y), screenCoordinate(_currentSize.x, y), _gridLineColor);
      }
      
      for (float x = 0.0; x < _currentSize.x; x+= _squareSize) {
        drawList->AddLine(screenCoordinate(x, 0), screenCoordinate(x, _currentSize.y), _gridLineColor);
      }
      
      drawList->AddLine(screenCoordinate(0.0,0.0), screenCoordinate(0.0, _currentSize.y), _gridLineColor);
      drawList->AddLine(screenCoordinate(0.0,0.0), screenCoordinate(_currentSize.x, 0.0), _gridLineColor);
      drawList->AddLine(screenCoordinate(_currentSize.x, 0.0), screenCoordinate(_currentSize.x, _currentSize.y), _gridLineColor);
      drawList->AddLine(screenCoordinate(0.0, _currentSize.y), screenCoordinate(_currentSize.x, _currentSize.y), _gridLineColor);
    }

    void begin() override {
      Parent::begin();

      drawGrid();        
    }

    void end() override {
      Parent::end();
    }

  };
  
}
