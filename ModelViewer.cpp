#include "ModelViewer.h"

#include <string>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#ifdef _DEBUG
#pragma comment (lib, "assimp-vc140-mtd.lib")
#else
#pragma comment (lib, "assimp-vc140-mt.lib")
#endif

#include "imgui/imgui.h"

#include <d3dcompiler.h>
#pragma comment (lib, "d3dcompiler.lib")

#pragma warning(disable : 4996) 

using namespace tofu;
using namespace tofu::math;

int32_t ModelViewer::init_assets()
{
	logBuffer = new ImGuiTextBuffer();

	importer = new Assimp::Importer();

	do
	{
		HRESULT ret = S_OK;
		{
			ID3DBlob* blob = nullptr;
			if (0 != load_file_to_blob(L"VertexShader.cso", &blob))
			{
				break;
			}

			ret = device->CreateVertexShader(
				blob->GetBufferPointer(),
				blob->GetBufferSize(),
				nullptr,
				&vertexShader);

			if (S_OK != ret)
			{
				blob->Release();
				break;
			}


			D3D11_INPUT_ELEMENT_DESC descs[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			uint32_t numDesc = sizeof(descs) / sizeof(D3D11_INPUT_ELEMENT_DESC);
			ret = device->CreateInputLayout(
				descs,
				numDesc,
				blob->GetBufferPointer(),
				blob->GetBufferSize(),
				&inputLayout);

			blob->Release();

			if (S_OK != ret)
			{
				break;
			}
		}


		{
			ID3DBlob* blob = nullptr;
			if (0 != load_file_to_blob(L"PixelShader.cso", &blob))
			{
				break;
			}

			ret = device->CreatePixelShader(
				blob->GetBufferPointer(),
				blob->GetBufferSize(),
				nullptr,
				&pixelShader);

			blob->Release();

			if (S_OK != ret)
			{
				break;
			}
		}

		{
			CD3D11_BUFFER_DESC cbDesc(
				sizeof(float) * 16,
				D3D11_BIND_CONSTANT_BUFFER,
				D3D11_USAGE_DYNAMIC,
				D3D11_CPU_ACCESS_WRITE);

			if (S_OK != device->CreateBuffer(&cbDesc, nullptr, &instanceCB))
				break;

			cbDesc.ByteWidth = sizeof(float) * 32;

			if (S_OK != device->CreateBuffer(&cbDesc, nullptr, &frameCB))
				break;
		}

		{
			CD3D11_RASTERIZER_DESC rsDesc(D3D11_DEFAULT);
			rsDesc.FillMode = D3D11_FILL_WIREFRAME;
			ret = device->CreateRasterizerState(&rsDesc, &rsState);
			if (S_OK != ret)
			{
				break;
			}

			CD3D11_DEPTH_STENCIL_DESC dsDesc(D3D11_DEFAULT);
			ret = device->CreateDepthStencilState(&dsDesc, &dsState);
			if (S_OK != ret)
			{
				break;
			}

			context->RSSetState(rsState);
			context->OMSetDepthStencilState(dsState, 0);
		}

		bonesCB = nullptr;

		bones.clear();
		boneNameArray.clear();

		vertexBuffer = nullptr;
		indexBuffer = nullptr;
		numVertices = 0;
		numIndices = 0;
		meshes.clear();

		return 0;

	} while (0);

	if (nullptr != instanceCB) instanceCB->Release();
	if (nullptr != frameCB) frameCB->Release();
	if (nullptr != rsState) rsState->Release();
	if (nullptr != dsState) dsState->Release();
	if (nullptr != vertexShader) vertexShader->Release();
	if (nullptr != pixelShader) pixelShader->Release();
	if (nullptr != inputLayout) inputLayout->Release();

	return 0;
}

void ModelViewer::cleanup_assets()
{
	delete importer;

	if (nullptr != vertexBuffer) vertexBuffer->Release();
	if (nullptr != indexBuffer) indexBuffer->Release();

	if (nullptr != bonesCB) bonesCB->Release();
	if (nullptr != instanceCB) instanceCB->Release();
	if (nullptr != frameCB) frameCB->Release();
	if (nullptr != rsState) rsState->Release();
	if (nullptr != dsState) dsState->Release();
	if (nullptr != vertexShader) vertexShader->Release();
	if (nullptr != pixelShader) pixelShader->Release();
	if (nullptr != inputLayout) inputLayout->Release();

	delete logBuffer;
}

void ModelViewer::update()
{
	gui();

	D3D11_MAPPED_SUBRESOURCE res = {};
	if (S_OK != context->Map(frameCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &res))
		return;

	{
		float4x4* data = reinterpret_cast<float4x4*>(res.pData);

		*data = translate(0.0f, 0.0f, 2.0f);

		float fov = 3.14159f * 0.5f;
		float aspect = (float)bufferWidth / bufferHeight;
		float zNear = 0.01f;
		float zFar = 100.0f;

		*(data + 1) = perspective(fov, aspect, zNear, zFar);

		context->Unmap(frameCB, 0);
	}

	if (S_OK != context->Map(instanceCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &res))
		return;

	{
		float4x4* data = reinterpret_cast<float4x4*>(res.pData);

		*(data) = identity();

		context->Unmap(instanceCB, 0);
	}

}

void ModelViewer::render()
{
	//render_meshes();
	render_scene();
}

void ModelViewer::gui()
{
	gui_menu();
	gui_log();
	gui_hierarchy();
	gui_meshes();
	gui_animations();
	gui_tracks();
	gui_skeleton();
}

void ModelViewer::gui_menu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{

			if (ImGui::MenuItem("Open", "CTRL+O"))
			{
				wchar_t cwd[1024] = {};
				wchar_t filename[1024] = {};

				GetCurrentDirectory(1024, cwd);

				OPENFILENAME ofn = {};
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFile = filename;
				ofn.nMaxFile = 1024;
				ofn.Flags = OFN_FILEMUSTEXIST;
				if (GetOpenFileName(&ofn))
				{
					load_model(ofn.lpstrFile);
				}

				SetCurrentDirectory(cwd);
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Quit", "ALT+F4"))
			{
				running = false;
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void ModelViewer::gui_log()
{
	ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Log")) return;

	ImGui::TextUnformatted(logBuffer->begin());

	ImGui::End();
}

void ModelViewer::gui_hierarchy()
{
	ImGui::SetNextWindowPos(ImVec2(10, 40), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(200, 600), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Hierarchy")) return;

	if (ImGui::Button("Generate Skeleton") && selectedNode != nullptr)
	{
		generate_skeleton(selectedNode);
	}

	do
	{
		if (nullptr == scene)
			break;

		gui_hierarchy_node(scene->mRootNode);

		if (selectedNode != nullptr)
		{
			ImGui::Separator();

			float* m = &(selectedNode->mTransformation.a1);
	
			ImGui::InputFloat4("r1", m, -1, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat4("r2", m + 4, -1, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat4("r3", m + 8, -1, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat4("r4", m + 12, -1, ImGuiInputTextFlags_ReadOnly);
		}

	} while (0);

	ImGui::End();
}

void ModelViewer::gui_hierarchy_node(aiNode * node)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	if (nullptr != selectedNode && node == selectedNode)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}
	if (node->mNumChildren == 0)
	{
		flags |= (ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
	}


	bool open = ImGui::TreeNodeEx(node->mName.C_Str(), flags, "%s%s", node->mName.C_Str(), node->mNumMeshes == 0 ? "" : "*");
	if (ImGui::IsItemClicked())
	{
		selectedNode = node;
		//if (node->me)
		/*node->mTransformation*/
	}

	if (open && node->mNumChildren > 0)
	{
		for (uint32_t i = 0; i < node->mNumChildren; ++i)
		{
			gui_hierarchy_node(node->mChildren[i]);
		}

		ImGui::TreePop();
	}
}

void ModelViewer::gui_meshes()
{
	ImGui::SetNextWindowPos(ImVec2(210, 40), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(200, 600), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Meshes")) return;
	do
	{
		if (nullptr == scene)
			break;

		for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
		{
			const char* name = scene->mMeshes[i]->mName.C_Str();
			char nameBuf[1024];
			strcpy(nameBuf, name);
			if (nameBuf[0] == '\0')
			{
				sprintf(nameBuf, "_unnamed_%u", i);
			}

			if (ImGui::Selectable(nameBuf, i == selectedMesh))
			{
				selectedMesh = i;
			}
		}

	} while (0);

	ImGui::End();
}

void ModelViewer::gui_animations()
{
	ImGui::SetNextWindowPos(ImVec2(210, 40), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(200, 600), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Animations")) return;

	do
	{
		if (nullptr == scene || !scene->HasAnimations())
			break;

		for (uint32_t i = 0; i < scene->mNumAnimations; ++i)
		{
			const char* name = scene->mAnimations[i]->mName.C_Str();
			if (ImGui::Selectable(name, i == selectedAnimation))
			{
				if (selectedAnimation != i)
				{
					generate_animation(scene->mAnimations[i]);
				}
				selectedAnimation = i;
			}
		}

	} while (0);

	ImGui::End();
}

void ModelViewer::gui_tracks()
{
	ImGui::SetNextWindowPos(ImVec2(10, 40), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(200, 600), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Tracks")) return;

	if (scene != nullptr && selectedAnimation != -1)
	{
		aiAnimation* a = scene->mAnimations[selectedAnimation];
		for (uint32_t i = 0; i < a->mNumChannels; i++)
		{
			aiNodeAnim* ch = a->mChannels[i];
			ImGui::Text("%s (T: %u, R: %u, S: %u)", 
				ch->mNodeName.C_Str(), 
				ch->mNumPositionKeys,
				ch->mNumRotationKeys,
				ch->mNumScalingKeys);
		}
	}

	ImGui::End();
}

void ModelViewer::gui_skeleton()
{
	ImGui::SetNextWindowPos(ImVec2(10, 40), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(200, 600), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Skeleton")) return;

	if (bones.size() > 0)
	{
		gui_skeleton_node(0);

		ImGui::Separator();

		if (selectedBone >= 0 && selectedBone < bones.size())
		{
			float* m = bones[selectedBone].matrix;
			
			ImGui::InputFloat4("r1", m, -1, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat4("r2", m + 4, -1, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat4("r3", m + 8, -1, ImGuiInputTextFlags_ReadOnly);
		}
	}

	ImGui::End();
}

void ModelViewer::gui_skeleton_node(int32_t node)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	if (node == selectedBone)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}
	if (bones[node].firstChild == -1)
	{
		flags |= (ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
	}

	bool open = ImGui::TreeNodeEx(&(boneNameArray[bones[node].name]), flags);
	if (ImGui::IsItemClicked())
	{
		selectedBone = node;
	}

	if (open && bones[node].firstChild != -1)
	{
		int32_t child = bones[node].firstChild;
		while (child != -1)
		{
			gui_skeleton_node(child);
			child = bones[child].nextSibling;
		}

		ImGui::TreePop();
	}
}

void ModelViewer::load_model(const wchar_t * filename)
{
	std::wstring wfn(filename);
	std::string fn(wfn.begin(), wfn.end());

	importer->FreeScene();
	this->scene = nullptr;
	selectedMesh = -1;
	selectedAnimation = -1;
	selectedNode = nullptr;

	vertexBuffer = nullptr;
	indexBuffer = nullptr;
	numVertices = 0;
	numIndices = 0;
	meshes.clear();

	const aiScene* scene = importer->ReadFile(fn.c_str(), 
		aiProcess_Triangulate | 
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_ConvertToLeftHanded);

	if (scene == nullptr)
		return;

	this->scene = scene;

	for (uint32_t i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* m = scene->mMeshes[i];
		numVertices += m->mNumVertices;
		numIndices += m->mNumFaces * 3;
	}

	if (numVertices == 0) return;

	SkinnedVertex* vertices = new SkinnedVertex[numVertices];
	int32_t* indices = new int32_t[numIndices];
	uint32_t vid = 0, iid = 0;
	for (uint32_t i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* m = scene->mMeshes[i];
		for (uint32_t j = 0; j < m->mNumVertices; j++)
		{
			auto& pos = m->mVertices[j];
			auto& norm = m->mNormals[j];
			auto& tan = m->mTangents[j];
			auto& uv = m->mTextureCoords[0][j];

			vertices[vid + j].position = float3{ pos.x, pos.y, pos.z };
			vertices[vid + j].normal = float3{ norm.x, norm.y, norm.z };
			vertices[vid + j].tangent = float3{ tan.x, tan.y, tan.z };
			vertices[vid + j].uv = float3{ uv.x, uv.y, uv.z };
			//vertices[vid + j].bones.x = m->
		}

		for (uint32_t j = 0; j < m->mNumFaces; j++)
		{
			indices[iid + j * 3] = m->mFaces[j].mIndices[0];
			indices[iid + j * 3 + 1] = m->mFaces[j].mIndices[1];
			indices[iid + j * 3 + 2] = m->mFaces[j].mIndices[2];
		}

		Mesh mesh;
		mesh.startVertex = vid;
		mesh.startIndex = iid;
		mesh.numVertices = m->mNumVertices;
		mesh.numIndices = m->mNumFaces * 3;
		meshes.push_back(mesh);

		vid += m->mNumVertices;
		iid += m->mNumFaces * 3;
	}

	do
	{
		CD3D11_BUFFER_DESC vbDesc(
			numVertices * sizeof(SkinnedVertex),
			D3D11_BIND_VERTEX_BUFFER);
		D3D11_SUBRESOURCE_DATA vbData = { vertices, 0, 0 };
		if (S_OK != device->CreateBuffer(&vbDesc, &vbData, &vertexBuffer))
			break;

		CD3D11_BUFFER_DESC ibDesc(
			numIndices * sizeof(uint32_t),
			D3D11_BIND_INDEX_BUFFER);
		D3D11_SUBRESOURCE_DATA ibData = { indices, 0, 0 };
		if (S_OK != device->CreateBuffer(&ibDesc, &ibData, &indexBuffer))
			break;

	} while (0);

	delete[] vertices;
	delete[] indices;

	if (nullptr == vertexBuffer || nullptr == indexBuffer)
	{
		if (nullptr != vertexBuffer) vertexBuffer->Release();
		if (nullptr != indexBuffer) indexBuffer->Release();
	}
}

void ModelViewer::render_meshes()
{
	if (meshes.empty()) return;

	UINT strides[] = { sizeof(SkinnedVertex) };
	UINT offsets[] = { 0 };

	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);
	context->IASetInputLayout(inputLayout);
	context->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offsets);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->RSSetState(rsState);
	context->OMSetDepthStencilState(dsState, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D11Buffer* cbs[] = { instanceCB, frameCB };
	context->VSSetConstantBuffers(0, 2, cbs);

	for (uint32_t i = 0; i < meshes.size(); i++)
	{
		Mesh& m = meshes[i];

		context->DrawIndexed(m.numIndices, m.startIndex, m.startVertex);
	}
}

void ModelViewer::render_scene()
{
	if (meshes.empty() || nullptr == scene) return;

	UINT strides[] = { sizeof(SkinnedVertex) };
	UINT offsets[] = { 0 };

	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);
	context->IASetInputLayout(inputLayout);
	context->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offsets);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->RSSetState(rsState);
	context->OMSetDepthStencilState(dsState, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D11Buffer* cbs[] = { instanceCB, frameCB };
	context->VSSetConstantBuffers(0, 2, cbs);

	render_scene_node(scene->mRootNode, translate(0.0f, -1.0f, 0.0f) * 
		rotate(quat(3.14159f * totalTime, float3{0.0f, 1.0f, 0.0f})) *
		scale(0.01f));
}

void ModelViewer::render_scene_node(aiNode * node, float4x4 parentTransform)
{
	float4x4 current = parentTransform * reinterpret_cast<float4x4&>(node->mTransformation);

	if (node->mNumMeshes > 0)
	{
		D3D11_MAPPED_SUBRESOURCE res = {};
		if (S_OK == context->Map(instanceCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &res))
		{
			float4x4* data = reinterpret_cast<float4x4*>(res.pData);

			*(data) = current;

			context->Unmap(instanceCB, 0);
		}
	}

	for (uint32_t i = 0; i < node->mNumMeshes; i++)
	{
		Mesh& m = meshes[node->mMeshes[i]];

		context->DrawIndexed(m.numIndices, m.startIndex, m.startVertex);
	}

	for (uint32_t i = 0; i < node->mNumChildren; i++)
	{
		render_scene_node(node->mChildren[i], current);
	}
}

int32_t ModelViewer::generate_skeleton(aiNode * node)
{
	selectedBone = -1;
	bones.clear();
	boneNameArray.clear();
	boneTable.clear();

	aiNode* root = node;

	{
		aiNode* p = node->mParent;
		while (p)
		{
			std::string nodeName = p->mName.C_Str();
			if (nodeName.find("_$AssimpFbx$_") == std::string::npos)
				break;

			root = p;
			p = p->mParent;
		}
	}

	return generate_skeleton_node(root, -1);
}

int32_t ModelViewer::generate_skeleton_node(aiNode * node, int32_t parentBoneIdx)
{
	std::string nodeName = node->mName.C_Str();
	aiMatrix4x4 matrix = node->mTransformation;

	size_t pos = nodeName.find("_$AssimpFbx$_");
	if (pos != std::string::npos)
	{
		nodeName = nodeName.substr(0, pos);

		while (node->mNumChildren == 1)
		{
			aiNode* n = node->mChildren[0];
			std::string nName = n->mName.C_Str();
			if (nName == nodeName ||
				nName.find(nodeName + "_$AssimpFbx$_") != std::string::npos)
			{
				matrix = matrix * n->mTransformation;
				node = n;
				continue;
			}
			break;
		}
	}

	int32_t boneId = int32_t(bones.size());
	bones.push_back(Bone());
	Bone& bone = bones[boneId];
	bone.parent = parentBoneIdx;
	bone.firstChild = -1;
	bone.nextSibling = -1;

	bone.name = int32_t(boneNameArray.size());
	boneNameArray.resize(boneNameArray.size() + nodeName.length() + 1);
	strcpy(&boneNameArray[bone.name], nodeName.c_str());

	assert(boneTable.find(nodeName) == boneTable.end());
	boneTable.insert(std::pair<std::string, int32_t>(nodeName, boneId));

	for (uint32_t i = 0; i < 3; i++)
		for (uint32_t j = 0; j < 4; j++)
			bone.matrix[i * 4 + j] = matrix[i][j];

	int32_t lastChild = -1;
	for (uint32_t i = 0; i < node->mNumChildren; ++i)
	{
		int32_t bId = generate_skeleton_node(node->mChildren[i], boneId);

		Bone& bone = bones[boneId];
		if (bone.firstChild == -1)
			bone.firstChild = bId;
		else
			bones[lastChild].nextSibling = bId;

		lastChild = bId;
	}

	return boneId;
}

int32_t ModelViewer::generate_animation(aiAnimation* a)
{
	if (nullptr == a) return -1;

	if (nullptr != bonesCB)
	{
		bonesCB->Release();
		bonesCB = nullptr;
	}

	{
		CD3D11_BUFFER_DESC desc(
			sizeof(float4x4) * bones.size(),
			D3D10_BIND_CONSTANT_BUFFER,
			D3D11_USAGE_DYNAMIC,
			D3D11_CPU_ACCESS_WRITE
		);

		if (S_OK != device->CreateBuffer(&desc, nullptr, &bonesCB))
		{
			return -1;
		}
	}

	anim = Animation();
	anim.frameRate = a->mTicksPerSecond == 0.0 ? 1.0f : float(a->mTicksPerSecond);
	anim.duration = float(a->mDuration);
	anim.numTracks = a->mNumChannels;

	tracks.clear();
	tracks.resize(bones.size());

	vectorFrames.clear();
	quatFrames.clear();

	for (uint32_t i = 0; i < a->mNumChannels; ++i)
	{
		auto& ch = a->mChannels[i];
		std::string nodeName(ch->mNodeName.C_Str());
		size_t pos = nodeName.find("_$AssimpFbx$_");
		if (pos != std::string::npos)
		{
			nodeName = nodeName.substr(0, pos);
		}

		if (boneTable.find(nodeName) == boneTable.end())
			continue;
		
		Track& t = tracks[boneTable[nodeName]];
		
		t.numTransFrames = ch->mNumPositionKeys;
		t.transFrames = uint32_t(vectorFrames.size());
		for (uint32_t f = 0; f < t.numTransFrames; f++)
		{
			auto& k = ch->mPositionKeys[f];
			
			vectorFrames.push_back(VectorFrame{
				float3{ k.mValue.x, k.mValue.y, k.mValue.z},
				float(k.mTime)
			});
		}

		t.numRotFrames = ch->mNumRotationKeys;
		t.rotFrames = uint32_t(quatFrames.size());
		for (uint32_t f = 0; f < t.numRotFrames; f++)
		{
			auto& k = ch->mRotationKeys[f];
			
			quatFrames.push_back(QuaternionFrame{
				float4{ k.mValue.x, k.mValue.y, k.mValue.z, k.mValue.w },
				float(k.mTime)
			});
		}

		t.numScaleFrames = ch->mNumScalingKeys;
		t.scaleFrames = uint32_t(vectorFrames.size());
		for (uint32_t f = 0; f < t.numScaleFrames; f++)
		{
			auto& k = ch->mScalingKeys[f];

			vectorFrames.push_back(VectorFrame{
				float3{ k.mValue.x, k.mValue.y, k.mValue.z },
				float(k.mTime)
			});
		}
	}

	return 0;
}

int32_t ModelViewer::compile_shader(const char * src, uint32_t size, const char * entry, const char * target, ID3DBlob ** blob)
{
	UINT flag1 = 0;
	UINT flag2 = 0;

#ifdef _DEBUG
	flag1 |= D3DCOMPILE_DEBUG;
	flag1 |= D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	flag1 |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	ID3DBlob* errMsg = nullptr;

	HRESULT ret = D3DCompile(
		src,
		size,
		nullptr,
		nullptr,
		nullptr,
		entry,
		target,
		flag1,
		flag2,
		blob,
		&errMsg);

	if (S_OK != ret)
	{
		char* buf = new char[errMsg->GetBufferSize() + 1];
		strncpy(buf, reinterpret_cast<char*>(errMsg->GetBufferPointer()), errMsg->GetBufferSize());
		buf[errMsg->GetBufferSize()] = 0;

		logBuffer->append(buf);
		delete[] buf;
	}

	if (nullptr != errMsg)
	{
		errMsg->Release();
	}

	return int32_t(ret);
}

int32_t ModelViewer::load_file_to_blob(const wchar_t * filename, ID3DBlob ** blob)
{
	return int32_t(D3DReadFileToBlob(filename, blob));
}
