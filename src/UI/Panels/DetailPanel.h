#pragma once

#include "UIPanel.h"

class TextureAtlas;

class DetailPanel : public UIPanel
{
public:
    DetailPanel(TextureAtlas& atlas) : UIPanel(atlas, "frameB9", 10, 10, 10, 10)
    {
    }
};
