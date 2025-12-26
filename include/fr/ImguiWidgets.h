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

#include <fr/Imgui/Registration.h>

#include <fr/Imgui/ActorWindow.h>
#include <fr/Imgui/CommitableNodeWindow.h>
#include <fr/Imgui/CompletedWindow.h>
#include <fr/Imgui/EffortWindow.h>
#include <fr/Imgui/EventWindow.h>
#include <fr/Imgui/EmailAddressWindow.h>
#include <fr/Imgui/GoalWindow.h>
#include <fr/Imgui/GraphNodeWindow.h>
#include <fr/Imgui/GridWindow.h>
#include <fr/Imgui/InternationalAddressWindow.h>
#include <fr/Imgui/KeyValueWindow.h>
#include <fr/Imgui/NodeAnchor.h>
#include <fr/Imgui/NodeEditorWindow.h>
#include <fr/Imgui/NodeWindow.h>
#include <fr/Imgui/OrganizationWindow.h>
#include <fr/Imgui/PersonWindow.h>
#include <fr/Imgui/PhoneNumberWindow.h>
#include <fr/Imgui/ProductWindow.h>
#include <fr/Imgui/ProjectWindow.h>
#include <fr/Imgui/PurposeWindow.h>
#include <fr/Imgui/RequirementWindow.h>
#include <fr/Imgui/RoleWindow.h>
#include <fr/Imgui/StoryWindow.h>
#include <fr/Imgui/TextWindow.h>
#include <fr/Imgui/TimeEstimateWindow.h>
#include <fr/Imgui/USAddressWindow.h>
#include <fr/Imgui/UseCaseWindow.h>
#include <fr/Imgui/Widget.h>
#include <fr/Imgui/WidgetApi.h>
#include <fr/Imgui/Window.h>

// Set up a typelist with all window types. If you build an external
// library that adds more windows, Typelist has a merge function you
// can use to merge this typelist with a typelist you export to
// easily enable window menu placement and creation for all the windows.

#include <fr/types/Typelist.h>

namespace fr::Imgui {
  using AllWindowList = fr::types::Typelist<
      fr::Imgui::ActorWindow, fr::Imgui::CompletedWindow,
      fr::Imgui::EffortWindow, fr::Imgui::EventWindow,
      fr::Imgui::EmailAddressWindow, fr::Imgui::GoalWindow,
      fr::Imgui::GraphNodeWindow, fr::Imgui::InternationalAddressWindow,
      fr::Imgui::KeyValueWindow, fr::Imgui::OrganizationWindow,
      fr::Imgui::PersonWindow, fr::Imgui::PhoneNumberWindow,
      fr::Imgui::ProductWindow, fr::Imgui::ProjectWindow,
      fr::Imgui::PurposeWindow, fr::Imgui::RequirementWindow,
      fr::Imgui::RoleWindow, fr::Imgui::StoryWindow, fr::Imgui::TextWindow,
      fr::Imgui::TimeEstimateWindow, fr::Imgui::USAddressWindow,
      fr::Imgui::UseCaseWindow>;
}

