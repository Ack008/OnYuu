#include "BatchRenderer.h"
BatchRender::BatchRender()
{

}

BatchRender::~BatchRender()
{
}

void BatchRender::addMeshRender(RenderMeshComponent* mesh, glm::mat4 model)
{
	if (mesh == nullptr || mesh->material == nullptr)
		return;
	BatchCouple couple = std::make_pair(mesh->material, mesh->renderingType);
	auto& batch = batches[couple];
	batch.push_back(RenderData{ mesh,model });
}

void BatchRender::clear()
{
	batches.clear();
}


std::unordered_map<BatchCouple, std::vector<BatchRender::RenderData>, BatchCoupleHash>* BatchRender::getBatches()
{
	return &batches;
}




