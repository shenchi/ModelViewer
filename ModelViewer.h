#pragma once

#include "Application.h"
#include "TofuMesh.h"
#include <unordered_map>
#include <string>

struct aiScene;
struct aiNode;
struct aiAnimation;
struct ImGuiTextBuffer;

using tofu::math::float4x4;
using tofu::Mesh;
using tofu::Bone;
using tofu::Vertex;
using tofu::Animation;

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
	aiNode*	selectedNode;
	int32_t selectedBone;

	ImGuiTextBuffer*	logBuffer;

private:

	void gui();

	void gui_menu();

	void gui_log();

	void gui_hierarchy();

	void gui_hierarchy_node(aiNode* node);

	void gui_meshes();

	void gui_animations();

	void gui_tracks();

	void gui_skeleton();

	void gui_skeleton_node(int32_t node);

	void load_model(const wchar_t* filename);

private:
	ID3D11VertexShader*	vertexShader;
	ID3D11PixelShader*	pixelShader;
	ID3D11InputLayout*	inputLayout;

	ID3D11RasterizerState*		rsState;
	ID3D11DepthStencilState*	dsState;

	ID3D11Buffer*		vertexBuffer;
	ID3D11Buffer*		indexBuffer;
	uint32_t			numVertices;
	uint32_t			numIndices;

	Mesh				meshes[1024];
	uint32_t			numMeshes;

	Bone				bones[1024];
	uint32_t			numBones;
	std::unordered_map<std::string, int32_t> boneTable;

	char				boneNameArray[2048];
	uint32_t			boneNameArraySize;

	ID3D11Buffer*		instanceCB;
	ID3D11Buffer*		frameCB;

	Animation			anim;

private:

	void render_meshes();
	void render_scene();
	void render_scene_node(aiNode* node, float4x4 parentTransform);

	int32_t generate_skeleton(aiNode* node);

	int32_t generate_skeleton_node(aiNode* node, int32_t parentBoneIdx);

	int32_t generate_animation(aiAnimation* anim);

	int32_t compile_shader(const char* src, uint32_t size, const char* entry, const char* target, ID3DBlob** blob);
	int32_t load_file_to_blob(const wchar_t* filename, ID3DBlob** blob);
};