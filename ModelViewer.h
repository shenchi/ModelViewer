#pragma once

#include "Application.h"

struct aiScene;
struct aiNode;

namespace Assimp
{
	class Importer;
}

class ModelViewer : public Application
{
protected:
	int32_t init_assets() override;
	void cleanup_assets() override;

	void update() override;
	void render() override;

private:
	Assimp::Importer* importer;
	const aiScene* scene;

private:

	void gui();

	void gui_menu();

	void gui_hierarchy();

	void gui_hierarchy_node(aiNode* node);

	void load_model(const wchar_t* filename);
};