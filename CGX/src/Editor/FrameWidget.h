#pragma once

#include "Widget.h"

class FrameWidget : public Widget 
{
public:
	void Initialize() override;
	void Update(float deltatime) override;
	void ShutDown() override;
};