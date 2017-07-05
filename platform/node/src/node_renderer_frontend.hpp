#pragma once

#include <mbgl/renderer/renderer_backend.hpp>
#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/util/async_task.hpp>

#include <functional>
#include <memory>

namespace node_mbgl {

class NodeRendererFrontend : public mbgl::RendererFrontend {
public:
    using ViewAccessorFunction = std::function<mbgl::View* ()>;
    NodeRendererFrontend(std::unique_ptr<mbgl::Renderer>, ViewAccessorFunction);

    ~NodeRendererFrontend();
    
    void reset() override;
    
    void setObserver(mbgl::RendererObserver&) override;

    void update(std::shared_ptr<mbgl::UpdateParameters>) override;
    
    std::vector<mbgl::Feature> queryRenderedFeatures(mbgl::ScreenLineString, mbgl::RenderedQueryOptions) const override;
    
    std::vector<mbgl::Feature> querySourceFeatures(std::string, mbgl::SourceQueryOptions) const override;

    void dumpDebugLogs();

private:
    std::unique_ptr<mbgl::Renderer> renderer;
    std::shared_ptr<mbgl::UpdateParameters> updateParameters;
    mbgl::util::AsyncTask asyncInvalidate;
};

} // namespace node_mbgl
