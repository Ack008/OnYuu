#include "ViewportPanel.h"

ViewportPanel::ViewportPanel()
{
}
ViewportPanel::~ViewportPanel()
{
}
void ViewportPanel::onImGuiRender()
{
	ImGui::Begin("Viewport");
	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	const float viewportAspectRatio = 16.0f / 9.0f;
	ImVec2 imageSize = viewportPanelSize;
	if (viewportPanelSize.x > 0.0f && viewportPanelSize.y > 0.0f)
	{
		float width = viewportPanelSize.x;
		float height = width / viewportAspectRatio;
		if (height > viewportPanelSize.y)
		{
			height = viewportPanelSize.y;
			width = height * viewportAspectRatio;
		}
		imageSize = ImVec2(width, height);
	}

	ImGui::Image(m_renderTarget->getColorAttachment(), imageSize, ImVec2(0, 1), ImVec2(1, 0));
	if (ImGui::IsItemHovered())
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	if (ImGui::IsWindowFocused()) {
		std::cout << "Viewport focused" << std::endl;
	}
	ImGui::End();
}
