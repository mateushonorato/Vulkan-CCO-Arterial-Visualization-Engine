#pragma once
#include <string>
#include <vector>
#include "vtk_reader.h"
#include <filesystem>

class App {
public:
    App();
    ~App();

    void run();

private:
    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanup();

    // Placeholders for future Vulkan handles
    void* windowHandle = nullptr;

    // Data
    std::vector<ArterialSegment> segments2D;
    std::string dataFilePath;

    // Helper to find VTK files recursively
    std::vector<std::string> discoverVtkFiles(const std::filesystem::path& root);
};
