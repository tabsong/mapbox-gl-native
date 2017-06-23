#include "node_renderer_frontend.hpp"
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/map/backend_scope.hpp>

namespace node_mbgl {

NodeRendererFrontend::NodeRendererFrontend(std::unique_ptr<mbgl::Renderer> renderer_, ViewAccessorFunction getView)
    : renderer(std::move(renderer_))
    , asyncInvalidate([&, this, getView] {
        if (renderer && updateParameters) {
            renderer->render(*getView(), *updateParameters);
        }
    }) {
}

NodeRendererFrontend::~NodeRendererFrontend() = default;
    
void NodeRendererFrontend::reset() {
    if (renderer) {
        renderer.reset();
    }
}
    
void NodeRendererFrontend::setObserver(mbgl::RendererObserver& observer_) {
    if (!renderer) return;
    renderer->setObserver(&observer_);
}

void NodeRendererFrontend::update(std::shared_ptr<mbgl::UpdateParameters> updateParameters_) {
    updateParameters = updateParameters_;
    asyncInvalidate.send();
}

std::vector<mbgl::Feature> NodeRendererFrontend::queryRenderedFeatures(mbgl::ScreenLineString geometry, mbgl::RenderedQueryOptions options) const {
    if (!renderer) return {};
    return renderer->queryRenderedFeatures(std::move(geometry), std::move(options));
}

std::vector<mbgl::Feature> NodeRendererFrontend::querySourceFeatures(std::string sourceID, mbgl::SourceQueryOptions options) const {
    if (!renderer) return {};
    return renderer->querySourceFeatures(std::move(sourceID), std::move(options));
}

void NodeRendererFrontend::dumpDebugLogs() {
    if (!renderer) return;
    return renderer->dumpDebugLogs();
}

} // namespace node_mbgl
