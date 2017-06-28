#include "android_renderer_frontend.hpp"

#include <mbgl/map/view.hpp>
#include <mbgl/renderer/renderer.hpp>

namespace mbgl {
namespace android {

AndroidRendererFrontend::AndroidRendererFrontend(
        std::unique_ptr<Renderer> renderer_,
        InvalidateCallback invalidate)
        : renderer(std::move(renderer_))
        , asyncInvalidate([=, invalidate=std::move(invalidate)]() {
            invalidate();
        }) {
}

AndroidRendererFrontend::~AndroidRendererFrontend() = default;

void AndroidRendererFrontend::reset() {
    if (renderer) {
        renderer.reset();
    }
}

void AndroidRendererFrontend::setObserver(RendererObserver& observer) {
    if (!renderer) return;
    renderer->setObserver(&observer);
}

void AndroidRendererFrontend::update(std::shared_ptr<UpdateParameters> params) {
    updateParameters = std::move(params);
    asyncInvalidate.send();
}

void AndroidRendererFrontend::render(View& view) {
    if (!updateParameters || !renderer) return;

    renderer->render(view, *updateParameters);
}

std::vector<Feature> AndroidRendererFrontend::queryRenderedFeatures(ScreenLineString geometry,
                                                                    RenderedQueryOptions options) const {
    if (!renderer) return {};
    return renderer->queryRenderedFeatures(geometry, options);
}

std::vector<Feature> AndroidRendererFrontend::querySourceFeatures(std::string sourceID,
                                                                  SourceQueryOptions options) const {
    if (!renderer) return {};
    return renderer->querySourceFeatures(sourceID, options);
}

void AndroidRendererFrontend::onLowMemory() {
    if (!renderer) return;
    renderer->onLowMemory();
}

} // namespace android
} // namespace mbgl

