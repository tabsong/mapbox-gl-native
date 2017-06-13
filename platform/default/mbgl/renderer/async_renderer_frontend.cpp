#include "async_renderer_frontend.hpp"
#include <mbgl/renderer/renderer.hpp>

namespace mbgl {

AsyncRendererFrontend::AsyncRendererFrontend(std::unique_ptr<Renderer> renderer_, View& view_)
    : renderer(std::move(renderer_))
    , view(view_)
    , asyncInvalidate([this] {
        if (renderer && updateParameters) {
            renderer->render(view, *updateParameters);
        }
    }) {
}

AsyncRendererFrontend::~AsyncRendererFrontend() = default;

void AsyncRendererFrontend::reset() {
    if (!renderer) return;
    renderer.reset();
}

void AsyncRendererFrontend::update(std::shared_ptr<UpdateParameters> updateParameters_) {
    updateParameters = updateParameters_;
    asyncInvalidate.send();
}

std::vector<Feature> AsyncRendererFrontend::queryRenderedFeatures(ScreenLineString geometry, RenderedQueryOptions options) const {
    if (!renderer) return {};
    return renderer->queryRenderedFeatures(geometry, options);
}

std::vector<Feature> AsyncRendererFrontend::querySourceFeatures(std::string sourceID, SourceQueryOptions options) const {
    if (!renderer) return {};
    return renderer->querySourceFeatures(sourceID, options);
}

void AsyncRendererFrontend::setObserver(RendererObserver& observer_) {
    if (!renderer) return;
    renderer->setObserver(&observer_);
}

} // namespace mbgl
