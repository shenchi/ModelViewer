#include "ModelViewer.h"

#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#ifdef _DEBUG
#pragma comment (lib, "assimp-vc140-mtd.lib")
#else
#pragma comment (lib, "assimp-vc140-mt.lib")
#endif

#include "imgui/imgui.h"

int32_t ModelViewer::init_assets()
{
	importer = new Assimp::Importer();
	return 0;
}

void ModelViewer::cleanup_assets()
{
	delete importer;
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
	gui_hierarchy();
}

void ModelViewer::gui_menu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{

			if (ImGui::MenuItem("Open", "CTRL+O"))
			{
				wchar_t filename[1024] = {};
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

void ModelViewer::gui_hierarchy()
{
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
	if (ImGui::TreeNode(node->mName.C_Str()))
	{
		for (auto i = 0; i < node->mNumChildren; ++i)
		{
			gui_hierarchy_node(node->mChildren[i]);
		}

		ImGui::TreePop();
	}
}

void ModelViewer::load_model(const wchar_t * filename)
{
	std::wstring wfn(filename);
	std::string fn(wfn.begin(), wfn.end());

	importer->FreeScene();

	const aiScene* scene = importer->ReadFile(fn.c_str(), 0u);

	if (nullptr != scene)
	{
		this->scene = scene;
	}
}
