#include "LineCharts.h"
#include <imgui.h>
#include <vector>
#include <string>

void LineCharts::drawLineChart(const std::string& title, const std::vector<float>& values)
{
    ImGui::Begin(title.c_str());

    if (!values.empty())
    {
        ImGui::PlotLines("##LineChart", values.data(), values.size(), 0, nullptr, 0.0f, 20.0f, ImVec2(0, 150));
    }

    ImGui::End();
}
