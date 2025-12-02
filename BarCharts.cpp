#include "BarCharts.h"
#include <imgui.h>
#include <vector>
#include <string>

void BarCharts::drawBarChart(const std::string& title, const std::vector<std::string>& labels, const std::vector<float>& values)
{
    if (labels.size() != values.size()) return;

    ImGui::Begin(title.c_str());

    for (size_t i = 0; i < labels.size(); ++i)
    {
        ImGui::Text("%s", labels[i].c_str());
        ImGui::SameLine(150); // décaler la barre
        ImGui::ProgressBar(values[i], ImVec2(200, 0));
    }

    ImGui::End();
}
