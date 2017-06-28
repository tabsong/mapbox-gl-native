
#pragma once

#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/util/async_task.hpp>

#include <functional>
#include <memory>

namespace mbgl {

class Renderer;
class View;
class RenderedQueryOptions;
class SourceQueryOptions;

namespace android {

class AndroidRendererFrontend : public RendererFrontend {
public:
    using InvalidateCallback = std::function<void ()>;
    AndroidRendererFrontend(std::unique_ptr<Renderer>, InvalidateCallback);
    ~AndroidRendererFrontend() override;

    void reset() override;
    void setObserver(RendererObserver&) override;

    void update(std::shared_ptr<UpdateParameters>) override;
    void render(View& view);

    virtual std::vector<Feature> queryRenderedFeatures(ScreenLineString, RenderedQueryOptions) const override;

    virtual std::vector<Feature> querySourceFeatures(std::string sourceID, SourceQueryOptions) const override;

    // Memory
    void onLowMemory();

private:
    std::unique_ptr<Renderer> renderer;
    std::shared_ptr<UpdateParameters> updateParameters;
    util::AsyncTask asyncInvalidate;
};

} // namespace android
} // namespace mbgl
