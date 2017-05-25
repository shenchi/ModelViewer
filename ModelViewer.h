#pragma once

#include "Application.h"

class ModelViewer : public Application
{
protected:
	int32_t init_assets() override;
	void cleanup_assets() override;

	void update() override;
	void render() override;
};