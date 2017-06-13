#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/map/query.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/geo.hpp>

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace mbgl {

class Backend;
class FileSource;
class RendererObserver;
class RenderedQueryOptions;
class Scheduler;
class SourceQueryOptions;
class UpdateParameters;
class View;

class Renderer {
public:
    Renderer(Backend&, float pixelRatio_, FileSource&, Scheduler&, MapMode = MapMode::Continuous,
             GLContextMode = GLContextMode::Unique, const optional<std::string> = {});
    ~Renderer();

    void render(View& view, const UpdateParameters&);

    void setObserver(RendererObserver*);

    std::vector<Feature> queryRenderedFeatures(const ScreenLineString&, const RenderedQueryOptions&) const;
    std::vector<Feature> querySourceFeatures(const std::string& sourceID, const SourceQueryOptions&) const;

    void dumpDebugLogs();

    void onLowMemory();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace mbgl
