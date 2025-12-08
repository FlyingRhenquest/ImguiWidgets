/**
 * Copyright 2025 Bruce Ide
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
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

    void begin() override {
      Window::begin();
      ImDrawList *drawList = ImGui::GetWindowDrawList();

      for (float y = 0; y < _currentSize.y; y += _squareSize) {
        drawList->AddLine(screenCoordinate(0, y), screenCoordinate(_currentSize.x, y), _gridLineColor);
      }

      for (float x = 0; x < _currentSize.x; x+= _squareSize) {
        drawList->AddLine(screenCoordinate(x, 0), screenCoordinate(x, _currentSize.y), _gridLineColor);
      }

      drawList->AddLine(screenCoordinate(0,0), screenCoordinate(0, _currentSize.y), _gridLineColor);
      drawList->AddLine(screenCoordinate(0,0), screenCoordinate(_currentSize.x, 0), _gridLineColor);
      drawList->AddLine(screenCoordinate(_currentSize.x, 0), screenCoordinate(_currentSize.x, _currentSize.y), _gridLineColor);
      drawList->AddLine(screenCoordinate(0, _currentSize.y), screenCoordinate(_currentSize.x, _currentSize.y), _gridLineColor);
        
    }

    void end() override {
      Window::end();
    }
    
  };
  
}
