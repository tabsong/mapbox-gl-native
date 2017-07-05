#include <mbgl/test/util.hpp>

#include <mbgl/algorithm/update_stencils.hpp>

using namespace mbgl;

namespace {

struct StencilRenderable {
    UnwrappedTileID id;
    std::set<CanonicalTileID> stencil;
    bool used = true;
};

bool operator==(const StencilRenderable& lhs, const StencilRenderable& rhs) {
    return lhs.id == rhs.id && lhs.stencil == rhs.stencil;
}

::std::ostream& operator<<(::std::ostream& os, const StencilRenderable& rhs) {
    os << "StencilRenderable{ " << rhs.id << ", { ";
    bool first = true;
    for (auto& id : rhs.stencil) {
        if (!first) {
            os << ", ";
        } else {
            first = false;
        }
        os << id;
    }
    return os << " } }";
}

} // namespace

void validate(const std::vector<StencilRenderable> expected) {
    std::vector<StencilRenderable> actual = expected;
    std::for_each(actual.begin(), actual.end(),
                  [](auto& renderable) { renderable.stencil.clear(); });
    algorithm::updateStencils<StencilRenderable>({ actual.begin(), actual.end() });
    EXPECT_EQ(expected, actual);
}

TEST(UpdateStencils, NoChildren) {
    validate({
        StencilRenderable{ UnwrappedTileID{ 0, 0, 0 }, { CanonicalTileID{ 0, 0, 0 } } },
    });

    validate({
        StencilRenderable{ UnwrappedTileID{ 4, 3, 8 }, { CanonicalTileID{ 0, 0, 0 } } },
    });

    validate({
        StencilRenderable{ UnwrappedTileID{ 1, 0, 0 }, { CanonicalTileID{ 0, 0, 0 } } },
        StencilRenderable{ UnwrappedTileID{ 1, 1, 1 }, { CanonicalTileID{ 0, 0, 0 } } },
    });

    validate({
        StencilRenderable{ UnwrappedTileID{ 1, 0, 0 }, { CanonicalTileID{ 0, 0, 0 } } },
        StencilRenderable{ UnwrappedTileID{ 2, 2, 3 }, { CanonicalTileID{ 0, 0, 0 } } },
    });
}

TEST(UpdateStencils, ParentAndFourChildren) {
    validate({
        // Stencil is empty (== not rendered!) because we have four covering children.
        StencilRenderable{ UnwrappedTileID{ 0, 0, 0 }, {} },
        // All four covering children
        StencilRenderable{ UnwrappedTileID{ 1, 0, 0 }, { CanonicalTileID{ 0, 0, 0 } } },
        StencilRenderable{ UnwrappedTileID{ 1, 0, 1 }, { CanonicalTileID{ 0, 0, 0 } } },
        StencilRenderable{ UnwrappedTileID{ 1, 1, 0 }, { CanonicalTileID{ 0, 0, 0 } } },
        StencilRenderable{ UnwrappedTileID{ 1, 1, 1 }, { CanonicalTileID{ 0, 0, 0 } } },
    });
}

TEST(UpdateStencils, OneChild) {
    validate({
        StencilRenderable{ UnwrappedTileID{ 0, 0, 0 },
                    // Only render the three children that aren't covering the other tile.
                    { CanonicalTileID{ 1, 0, 1 },
                      CanonicalTileID{ 1, 1, 0 },
                      CanonicalTileID{ 1, 1, 1 } } },
        StencilRenderable{ UnwrappedTileID{ 1, 0, 0 }, { CanonicalTileID{ 0, 0, 0 } } },
    });
}

TEST(UpdateStencils, Complex) {
    validate({
        StencilRenderable{ UnwrappedTileID{ 0, 0, 0 },
                           { CanonicalTileID{ 1, 0, 1 }, CanonicalTileID{ 1, 1, 0 },
                             CanonicalTileID{ 2, 2, 3 }, CanonicalTileID{ 2, 3, 2 },
                             CanonicalTileID{ 3, 6, 7 }, CanonicalTileID{ 3, 7, 6 } } },
        StencilRenderable{ UnwrappedTileID{ 0, { 1, 0, 0 } }, { CanonicalTileID{ 0, 0, 0 } } },
        StencilRenderable{ UnwrappedTileID{ 0, { 2, 2, 2 } }, { CanonicalTileID{ 0, 0, 0 } } },
        StencilRenderable{ UnwrappedTileID{ 0, { 3, 7, 7 } }, { CanonicalTileID{ 0, 0, 0 } } },
        StencilRenderable{ UnwrappedTileID{ 0, { 3, 6, 6 } }, { CanonicalTileID{ 0, 0, 0 } } },
    });

    validate({
        StencilRenderable{ UnwrappedTileID{ 0, 0, 0 },
                           { CanonicalTileID{ 1, 0, 1 }, CanonicalTileID{ 1, 1, 0 },
                             CanonicalTileID{ 1, 1, 1 }, CanonicalTileID{ 2, 0, 0 },
                             CanonicalTileID{ 2, 0, 1 }, CanonicalTileID{ 2, 1, 0 },
                             CanonicalTileID{ 3, 2, 3 }, CanonicalTileID{ 3, 3, 2 },
                             CanonicalTileID{ 3, 3, 3 }, CanonicalTileID{ 4, 4, 5 },
                             CanonicalTileID{ 4, 5, 4 }, CanonicalTileID{ 4, 5, 5 } } },
        StencilRenderable{ UnwrappedTileID{ 4, 4, 4 }, { CanonicalTileID{ 0, 0, 0 } } },
    });
}
