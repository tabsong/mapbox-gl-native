#include "glfw_renderer_frontend.hpp"

#include <mbgl/renderer/renderer.hpp>

GLFWRendererFrontend::GLFWRendererFrontend(std::unique_ptr<mbgl::Renderer> renderer_, GLFWView& glfwView_)
    : glfwView(glfwView_)
    , renderer(std::move(renderer_)) {
        glfwView.setRenderFrontend(this);
}

GLFWRendererFrontend::~GLFWRendererFrontend() = default;

void GLFWRendererFrontend::reset() {
    if (renderer) {
        renderer.reset();
    }
}

void GLFWRendererFrontend::setObserver(mbgl::RendererObserver& observer) {
    if (!renderer) return;
    renderer->setObserver(&observer);
}

void GLFWRendererFrontend::update(std::shared_ptr<mbgl::UpdateParameters> params) {
    updateParameters = std::move(params);
    glfwView.invalidate();
}

void GLFWRendererFrontend::render() {
    if (!renderer || !updateParameters) return;

    renderer->render(glfwView, *updateParameters);
}

std::vector<mbgl::Feature> GLFWRendererFrontend::queryRenderedFeatures(mbgl::ScreenLineString geometry, mbgl::RenderedQueryOptions options) const {
    if (!renderer) return {};
    return renderer->queryRenderedFeatures(geometry, options);
}

std::vector<mbgl::Feature> GLFWRendererFrontend::querySourceFeatures(std::string sourceID, mbgl::SourceQueryOptions options) const {
    if (!renderer) return {};
    return renderer->querySourceFeatures(sourceID, options);
}
