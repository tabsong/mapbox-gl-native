
#pragma once

#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/util/async_task.hpp>

#include <functional>
#include <memory>

namespace mbgl {

class View;

class StubRendererFrontend : public RendererFrontend {
public:
    // Calls the provided callback when it's time to render
    using InvalidateCallback = std::function<void (StubRendererFrontend&)>;
    StubRendererFrontend(std::unique_ptr<Renderer>, InvalidateCallback);

    // Will render async
    StubRendererFrontend(std::unique_ptr<Renderer>, View&);

    ~StubRendererFrontend() override;

    void reset() override;

    void setObserver(RendererObserver&) override;

    void update(std::shared_ptr<UpdateParameters>) override;
    void render(View& view);

    std::vector<Feature> queryRenderedFeatures(ScreenLineString, RenderedQueryOptions) const override;
    std::vector<Feature> querySourceFeatures(std::string sourceID, SourceQueryOptions) const override;

private:
    std::unique_ptr<Renderer> renderer;
    std::shared_ptr<UpdateParameters> updateParameters;
    util::AsyncTask asyncInvalidate;
};

} // namespace mbgl
