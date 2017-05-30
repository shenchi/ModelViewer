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
				{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
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

		return 0;

	} while (0);

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
}

void ModelViewer::render()
{
}

void ModelViewer::gui()
{
	gui_menu(); 
	gui_log();
	gui_hierarchy();
	gui_meshes();
	gui_animations();
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
	do
	{
		if (nullptr == scene)
			break;

		gui_hierarchy_node(scene->mRootNode);
		
	} while (0);

	ImGui::End();
}

void ModelViewer::gui_hierarchy_node(aiNode * node)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	if (nullptr != selectedNode && 0 == strcmp(node->mName.C_Str(), selectedNode))
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}
	if (node->mNumChildren == 0)
	{
		flags |= (ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
	}

	bool open = ImGui::TreeNodeEx(node->mName.C_Str(), flags);
	if (ImGui::IsItemClicked())
	{
		selectedNode = node->mName.C_Str();
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
				selectedAnimation = i;
			}
		}
		
	} while (0);

	ImGui::End();
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

	const aiScene* scene = importer->ReadFile(fn.c_str(), 0u);

	if (nullptr != scene)
	{
		this->scene = scene;
	}
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
