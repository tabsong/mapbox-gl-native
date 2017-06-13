#pragma once

#include <mbgl/map/map_observer.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/geo.hpp>

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace mbgl {

class Renderer;
class RendererObserver;
class RenderedQueryOptions;
class SourceQueryOptions;
class UpdateParameters;

class RendererFrontend {
public:

    virtual ~RendererFrontend() = default;

    // Must synchronously clean up the Renderer if set
    virtual void reset() = 0;

    // Implementer must bind the renderer observer to the renderer in a
    // appropriate manner so that the callbacks occur on the main thread
    virtual void setObserver(RendererObserver&) = 0;

    // Coalescing updates is up to the implementer
    virtual void update(std::shared_ptr<UpdateParameters>) = 0;

    virtual std::vector<Feature> queryRenderedFeatures(ScreenLineString, RenderedQueryOptions) const = 0;

    virtual std::vector<Feature> querySourceFeatures(std::string, SourceQueryOptions) const = 0;
};

} // namespace mbgl
