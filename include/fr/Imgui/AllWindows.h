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

#include <fr/types/Typelist.h>

namespace fr::Imgui {
  // Forward-declare all the windows
  class ActorWindow;
  class CompletedWindow;
  class EffortWindow;
  class EventWindow;
  class EmailAddressWindow;
  class GoalWindow;  
  class GraphNodeWindow;
  class InternationalAddressWindow;  
  class KeyValueWindow;
  class OrganizationWindow;  
  class PersonWindow;
  class PhoneNumberWindow;  
  class ProductWindow;
  class ProjectWindow;  
  class PurposeWindow;
  class RequirementWindow;  
  class RoleWindow;
  class StoryWindow;
  class TextWindow;  
  class TimeEstimateWindow;
  class USAddressWindow;  
  class UseCaseWindow;
  
  // Set up a typelist with all the windows
  using AllWindowList = fr::types::Typelist<
    ActorWindow, CompletedWindow, EffortWindow, EventWindow,
    EmailAddressWindow, GoalWindow, GraphNodeWindow,
    InternationalAddressWindow, KeyValueWindow, OrganizationWindow,
    PersonWindow, PhoneNumberWindow, ProductWindow, ProjectWindow,
    PurposeWindow, RequirementWindow, RoleWindow, StoryWindow,
    TextWindow, TimeEstimateWindow, USAddressWindow,
    UseCaseWindow>;  
}
