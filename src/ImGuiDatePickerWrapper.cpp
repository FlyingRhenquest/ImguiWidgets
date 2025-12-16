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

// Add gmtime_s to ImGuiDatePicker in Unix envrionments

#include <time.h>
#if defined(__linux__) || defined(__APPLE__) || defined(__unix__)
#define gmtime_s(timer, result) gmtime_r(result, timer)
#endif
#include <../ImGuiDatePicker/ImGuiDatePicker.cpp>
