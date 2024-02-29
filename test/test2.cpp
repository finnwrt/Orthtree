#define OLC_PGE_APPLICATION
#include <random>
#include "olcPixelGameEngine.h"
#include "Orthtree.h"

typedef Orthtree<2, int> qt;

class Renderer : public olc::PixelGameEngine
{
    qt tree;
    std::vector<qt::VecN> points;
public:
    Renderer() { sAppName = "Orthtree Test"; }

    bool OnUserCreate() override
    {
        // Generate some random points
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<int> dist(0, 512);
        for (size_t i = 0; i < 100; ++i)
            points.push_back({{ dist(rng), dist(rng) }});
        // Generate point-region quadtree
        size_t bucketCapacity = 1;
        tree.Generate({{ 0, 0 }}, {{ 512, 512 }}, 16, [&](auto& node) {
            int nPoints = 0;
            for (auto& p : points)
                if (node.ContainsPoint(p))
                    nPoints++;
            return nPoints > bucketCapacity;
        });

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        Clear(olc::BLACK);

        for (auto& node : tree)
            DrawRect({ node.pos[0], node.pos[1] }, { node.size[0], node.size[1] });

        for (auto& p : points)
            Draw({ p[0], p[1] }, olc::RED);

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
