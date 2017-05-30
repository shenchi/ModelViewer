#pragma once

#include "Application.h"

struct aiScene;
struct aiNode;
struct ImGuiTextBuffer;

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

	int32_t selectedMesh;
	int32_t selectedAnimation;
	const char* selectedNode;

	ImGuiTextBuffer*	logBuffer;

private:

	void gui();

	void gui_menu();

	void gui_log();

	void gui_hierarchy();

	void gui_hierarchy_node(aiNode* node);

	void gui_meshes();

	void gui_animations();

	void load_model(const wchar_t* filename);

private:
	ID3D11VertexShader*	vertexShader;
	ID3D11PixelShader*	pixelShader;
	ID3D11InputLayout*	inputLayout;

	ID3D11RasterizerState*		rsState;
	ID3D11DepthStencilState*	dsState;

	//ID3D11Buffer*		vertexBuffer;
	//ID3D11Buffer*		indexBuffer;

private:

	int32_t compile_shader(const char* src, uint32_t size, const char* entry, const char* target, ID3DBlob** blob);
	int32_t load_file_to_blob(const wchar_t* filename, ID3DBlob** blob);
};