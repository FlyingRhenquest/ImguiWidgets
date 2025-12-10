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

namespace fr::Imgui {

  class Window;
  
  class WidgetApi {
  public:
    using Type = WidgetApi;
    using PtrType = std::shared_ptr<WidgetApi>;
    
    WidgetApi() {}
    virtual ~WidgetApi() {}
    
    virtual void setParent(std::shared_ptr<Window> p) = 0;
    virtual std::string getLabel() = 0;
    virtual void begin() = 0;
    virtual void end() = 0;
  };

}
