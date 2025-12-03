#include "app.h"
#include <iostream>
#include <filesystem>

App::App() {}
App::~App() { cleanup(); }

void App::run() {
    initWindow();
    initVulkan();
    // Discover user-provided 2D VTK files under data/ (source dir)
#ifdef DATA_DIR
    std::filesystem::path dataDir = std::filesystem::path(DATA_DIR);
#else
    std::filesystem::path dataDir = std::filesystem::path("data");
#endif
    std::filesystem::path data2D = dataDir / "2D";
    // Prefer data/2D; fallback to data/; search recursively
    std::filesystem::path scanRoot = std::filesystem::exists(data2D) ? data2D : dataDir;
    auto candidates = discoverVtkFiles(scanRoot);
    if (candidates.empty()) {
        std::cerr << "[App] No .vtk files found under: " << scanRoot << std::endl;
    } else {
        std::cout << "[App] Found .vtk files (enter index to load):" << std::endl;
        for (size_t i = 0; i < candidates.size(); ++i) {
            std::cout << "  [" << i << "] " << candidates[i] << std::endl;
        }
        std::cout << "[App] Selection: " << std::flush;
        size_t idx = 0;
        if (!(std::cin >> idx) || idx >= candidates.size()) {
            std::cout << "[App] Invalid selection. Defaulting to [0]." << std::endl;
            idx = 0;
            // Clear error state and ignore rest of line
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        dataFilePath = candidates[idx];
        segments2D = VtkReader::load(dataFilePath, /*is3D=*/false);
        std::cout << "[App] Loaded 2D segments: " << segments2D.size() << " from " << dataFilePath << std::endl;
    }
    mainLoop();
    cleanup();
}

void App::initWindow() {
    // TODO: Create GLFW window
    std::cout << "[App] initWindow (placeholder)" << std::endl;
}

void App::initVulkan() {
    // TODO: Create Vulkan instance, device, swapchain, pipeline
    std::cout << "[App] initVulkan (placeholder)" << std::endl;
}

void App::mainLoop() {
    // TODO: Event loop and draw calls
    std::cout << "[App] mainLoop (placeholder). Segments ready: " << segments2D.size() << std::endl;
}

void App::cleanup() {
    // TODO: Destroy Vulkan and window resources
    std::cout << "[App] cleanup (placeholder)" << std::endl;
}

std::vector<std::string> App::discoverVtkFiles(const std::filesystem::path& root) {
    std::vector<std::string> files;
    if (!std::filesystem::exists(root)) return files;
    std::error_code ec;
    for (auto it = std::filesystem::recursive_directory_iterator(root, ec);
         it != std::filesystem::recursive_directory_iterator(); ++it) {
        if (ec) break;
        const auto& p = *it;
        if (p.is_regular_file()) {
            auto ext = p.path().extension().string();
            if (ext == ".vtk" || ext == ".VTK") {
                files.push_back(p.path().string());
            }
        }
    }
    return files;
}
