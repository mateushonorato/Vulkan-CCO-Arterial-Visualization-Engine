#pragma once
#include <string>
#include <vector>
#include "vertex.h"

// Minimal interface for reading VTK arterial tree data.
// Implementation will support both 2D and 3D datasets, including partial loads.
struct ArterialSegment {
    Vertex v0;
    Vertex v1;
    float radius;
};

class VtkReader {
public:
    // Load file and return segments. Mode may select 2D/3D parsing.
    static std::vector<ArterialSegment> load(const std::string& filePath, bool is3D = false);
};
