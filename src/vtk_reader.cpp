#include "vtk_reader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

// Very lightweight ASCII VTK POLYDATA 2D reader (POINTS + LINES + CELL_DATA radius)
// Assumptions: ASCII format, float points, LINES section lists segments as "2 i j"
// Focused on 2D: z ignored, normals default, color default.

static bool starts_with(const std::string& s, const std::string& prefix) {
    return s.rfind(prefix, 0) == 0;
}

std::vector<ArterialSegment> VtkReader::load(const std::string& filePath, bool is3D) {
    (void)is3D; // Currently focusing on 2D parsing

    std::ifstream in(filePath);
    if (!in) {
        std::cerr << "[VtkReader] Failed to open: " << filePath << std::endl;
        return {};
    }

    std::vector<std::array<float,3>> points;
    std::vector<std::pair<int,int>> edges;
    std::vector<float> radii;

    std::string line;
    enum Section { None, Points, Lines, CellData, Scalars, Radii };
    Section sec = None;
    int remainingPoints = 0;
    int expectedLines = 0;

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        // Trim leading spaces
        while (!line.empty() && (line.front()=='\t' || line.front()==' ')) line.erase(line.begin());

        if (starts_with(line, "POINTS ")) {
            // e.g., POINTS 4 float
            std::istringstream iss(line);
            std::string tag; int count; std::string type;
            iss >> tag >> count >> type;
            remainingPoints = count;
            sec = Points;
            continue;
        }
        if (starts_with(line, "LINES ")) {
            std::istringstream iss(line);
            std::string tag; int numLines; int entries;
            iss >> tag >> numLines >> entries;
            expectedLines = numLines;
            sec = Lines;
            continue;
        }
        if (starts_with(line, "CELL_DATA ")) {
            sec = CellData;
            continue;
        }
        if (sec == CellData && starts_with(line, "SCALARS radius")) {
            sec = Scalars;
            continue;
        }
        if (sec == Scalars && starts_with(line, "LOOKUP_TABLE")) {
            sec = Radii;
            continue;
        }

        // Parse contents
        if (sec == Points && remainingPoints > 0) {
            std::istringstream iss(line);
            float x,y,z; iss >> x >> y >> z; points.push_back({x,y,z});
            remainingPoints--;
            if (remainingPoints == 0) sec = None;
            continue;
        }
        if (sec == Lines && (int)edges.size() < expectedLines) {
            std::istringstream iss(line);
            int n, a, b; iss >> n >> a >> b; // expect n==2
            if (n == 2) edges.emplace_back(a,b);
            if ((int)edges.size() == expectedLines) sec = None;
            continue;
        }
        if (sec == Radii) {
            std::istringstream iss(line);
            float r; if (iss >> r) radii.push_back(r);
            continue;
        }
    }

    std::vector<ArterialSegment> segs;
    segs.reserve(edges.size());
    for (size_t i = 0; i < edges.size(); ++i) {
        auto [a,b] = edges[i];
        if (a < 0 || b < 0 || a >= (int)points.size() || b >= (int)points.size()) continue;
        float radius = (i < radii.size()) ? radii[i] : 0.1f;
        Vertex v0{}; Vertex v1{};
        v0.position = {points[a][0], points[a][1], 0.0f};
        v1.position = {points[b][0], points[b][1], 0.0f};
        v0.normal = {0.0f, 0.0f, 1.0f};
        v1.normal = {0.0f, 0.0f, 1.0f};
        v0.color = {1.0f, 1.0f, 1.0f, 1.0f};
        v1.color = {1.0f, 1.0f, 1.0f, 1.0f};
        v0.uv = {0.0f, 0.0f};
        v1.uv = {1.0f, 1.0f};
        segs.push_back({v0, v1, radius});
    }

    std::cout << "[VtkReader] Loaded 2D segments: " << segs.size() << " from " << filePath << std::endl;
    return segs;
}
