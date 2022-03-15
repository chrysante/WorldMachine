#include "ImGuiExt.hpp"

#include <string>
#include <array>
#include <sstream>
#include <cassert>
#include <imgui/imgui.h>
#include <utl/format.hpp>

using namespace mtl::short_types;

namespace ImGui {
	
	
	static std::array<std::string, 4> printRows(mtl::double4x4 x) {
		std::stringstream sstr;
		sstr << x;
				
		std::array<std::string, 4> result;

		std::string item;
		int i = 0;
		while (std::getline(sstr, item/*, '\n'*/)) {
			for (auto itr = item.begin(); itr != item.end();) {
				if (*itr < 0x20) {
					itr = item.erase(itr);
					continue;
				}
				++itr;
			}
			item.insert(item.begin(), '|');
			item.insert(item.end(), '|');
			result[i++] = item;
		}
		assert(i == 4);
		return result;
	}
	
	void Matrix(std::string_view label, mtl::double4x4 x) {
		auto const rows = printRows(x);
		
		
		
		auto const font = ImGui::GetIO().Fonts->Fonts[2];
		
		assert(font);
		ImGui::PushFont(font);
		ImGui::Text("%s", utl::format("{: ^{}}{}", "", label.length(), rows[0].data()).data());
		ImGui::Text("%s", utl::format("{}{}",          label.data(),   rows[1].data()).data());
		ImGui::Text("%s", utl::format("{: ^{}}{}", "", label.length(), rows[2].data()).data());
		ImGui::Text("%s", utl::format("{: ^{}}{}", "", label.length(), rows[3].data()).data());
		ImGui::PopFont();
	}
	
	bool SliderDouble2(const char *label,
					   double *v,
					   double v_min,
					   double v_max) {
		auto& doubleVector = *reinterpret_cast<double2*>(v);
		float2 fv = doubleVector;
		bool result = ImGui::SliderFloat2(label, fv.data(), v_min, v_max);
		doubleVector = fv;
		return result;
	}
	
}
