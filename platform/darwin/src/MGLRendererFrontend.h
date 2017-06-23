#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/renderer_frontend.hpp>


/**
 The RenderFrontend is passed to the Map to facilitate rendering in a platform
 dependent way.
 */
class MGLRenderFrontend : public mbgl::RendererFrontend
{
public:
    MGLRenderFrontend(std::unique_ptr<mbgl::Renderer> renderer_, MGLMapView* nativeView_, mbgl::View* mbglView_)
        : renderer(std::move(renderer_))
        , nativeView(nativeView_)
        , mbglView(mbglView_) {
    }
    
    void reset() override {
        if (renderer) {
            renderer.reset();
        }
    }
    
    void update(std::shared_ptr<mbgl::UpdateParameters> updateParameters_) override {
        updateParameters = std::move(updateParameters_);
        [nativeView setNeedsGLDisplay];
    };
    
    std::vector<mbgl::Feature> queryRenderedFeatures(mbgl::ScreenLineString geometry, mbgl::RenderedQueryOptions options) const override  {
        if (!renderer)  return {};
        return renderer->queryRenderedFeatures(geometry, options);
    };
    
    std::vector<mbgl::Feature> querySourceFeatures(std::string sourceID, mbgl::SourceQueryOptions options) const override  {
        if (!renderer)  return {};
        return renderer->querySourceFeatures(sourceID, options);
        
    };
    
    void setObserver(mbgl::RendererObserver& observer) override {
        if (!renderer) return;
        renderer->setObserver(&observer);
    };
    
    void render() {
        if (!renderer || !updateParameters) return;
        
        renderer->render(*mbglView, *updateParameters);
    }
    
    void onLowMemory() {
        if (!renderer)  return;
        renderer->onLowMemory();
    }
    
private:
    std::unique_ptr<mbgl::Renderer> renderer;
    __weak MGLMapView *nativeView = nullptr;
    mbgl::View *mbglView = nullptr;
    std::shared_ptr<mbgl::UpdateParameters> updateParameters;
};
