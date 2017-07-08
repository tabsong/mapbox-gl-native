#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/map/query.hpp>
#include <mbgl/annotation/annotation.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/geo.hpp>

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace mbgl {

class FileSource;
class RendererBackend;
class RendererObserver;
class RenderedQueryOptions;
class Scheduler;
class SourceQueryOptions;
class UpdateParameters;
class View;

class Renderer {
public:
    Renderer(RendererBackend&, float pixelRatio_, FileSource&, Scheduler&, MapMode = MapMode::Continuous,
             GLContextMode = GLContextMode::Unique, const optional<std::string> programCacheDir = {});
    ~Renderer();

    void setObserver(RendererObserver*);

    void render(View& view, const UpdateParameters&);

    // Feature queries
    std::vector<Feature> queryRenderedFeatures(const ScreenLineString&, const RenderedQueryOptions&) const;
    std::vector<Feature> queryRenderedFeatures(const ScreenCoordinate& point, const RenderedQueryOptions&) const;
    std::vector<Feature> queryRenderedFeatures(const ScreenBox& box, const RenderedQueryOptions&) const;
    std::vector<Feature> querySourceFeatures(const std::string& sourceID, const SourceQueryOptions&) const;
    AnnotationIDs queryPointAnnotations(const ScreenBox& box) const;

    // Debug
    void dumpDebugLogs();

    // Memory
    void onLowMemory();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace mbgl
