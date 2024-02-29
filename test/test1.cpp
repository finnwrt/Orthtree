#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Orthtree.h"

class Renderer : public olc::PixelGameEngine
{
    Orthtree<2> tree;
public:
    Renderer() { sAppName = "Orthtree Test"; }

    bool OnUserCreate() override
    {
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        Clear(olc::BLACK);

        Orthtree<2>::VecN camera = {{ (float)GetMouseX(), (float)GetMouseY() }};

        tree.Generate({{ 0.0f, 0.0f }}, {{ 512.0f, 512.0f }}, 8, [&](auto& node) {
            auto dist = camera.Distance(node.centre);
            return dist < node.size[0] * 2.0f;
        });

        for (auto& node : tree)
            DrawRect({ (int)node.pos[0], (int)node.pos[1] }, { (int)node.size[0], (int)node.size[1] });

        return true;
    }
};

int main()
{
    Renderer renderer;
    if (renderer.Construct(512, 512, 1, 1))
        renderer.Start();

    return 0;
}
