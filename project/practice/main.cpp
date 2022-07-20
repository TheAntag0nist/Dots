#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "FileDialog/ImGuiFileDialog.h"

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <list>

#include "InnerLogic/helpers/processing_helper.h"
#include "InnerLogic/points_set/points_set.h"
#include "InnerLogic/helpers/parser_helper.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif
#include <Instance.h>

// Global Info
#pragma region - Global Data -
ImVec4 clear_color = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
std::string glfw_opengl_version = "";
GLFWwindow* window = nullptr;

bool is_grid_active = true;
bool show_position = true;

ImColor white(1.0f, 1.0f, 1.0f);
ImColor dark_grey(0.4f, 0.4f, 0.4f);

ImColor red(1.0f, 0.0f, 0.0f);
ImColor green(0.0f, 1.0f, 0.0f);

points_set<point> set_of_points;
std::list<float> distances_info;
int point_id = -1;
#pragma endregion

// Prototypes
#pragma region - Prototypes -
void InitGLFW();
void ChooseGL();
void AppCreateWindow();
void DispatchEvents();
static void glfw_error_callback(int error, const char* description);
void Render(GLFWwindow* window, ImVec4 background_color);
void Shutdown(GLFWwindow* window);

void HideConsole();
int GetWindowHeight();
int GetWindowWidth();
void PracticeApplication();

void CreateButtons();
void CreatePointsTree();

void OpenImportFileDialog();
void OpenExportFileDialog();

void MenuTree();
void Viewport();
void ViewportRender();
void DrawGrid(ImDrawList* drawList);
void DrawMainAxes(ImDrawList* drawList, float half_width,
    float width, float half_height);
void DrawDividers(ImDrawList* drawList,
    float height, float width, float half_width, float half_height);

void OnImport(std::string importPath);
void OnExport(std::string exportPath);

void DrawPoint(ImColor color, ImDrawList* drawList, ImVec2 realPos);

std::string GetStringFromVec2(ImVec2 vec);
#pragma endregion

int main(int, char**)
{
    HideConsole();

    InitGLFW();
    ChooseGL();
    AppCreateWindow();

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Dispatch Events
        DispatchEvents();
        // Main Logic
        PracticeApplication();
        // Rendering
        Render(window, clear_color);
        glfwSwapBuffers(window);
    }

    // Cleanup
    Shutdown(window);
    return 0;
}

#pragma region - Window Main Functions -
void InitGLFW() {
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        exit(1);
}

void ChooseGL() {
    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfw_opengl_version = glsl_version;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfw_opengl_version = glsl_version;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfw_opengl_version = glsl_version;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
}

void AppCreateWindow() {
    // Create window with graphics context
    window = glfwCreateWindow(1280, 720, "Industrial Practice", NULL, NULL);
    if (window == NULL)
        exit(1);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glfw_opengl_version.c_str());
}

void DispatchEvents() {
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Render(GLFWwindow* window, ImVec4 background_color) {
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(background_color.x * background_color.w, background_color.y * background_color.w,
        background_color.z * background_color.w, background_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Shutdown(GLFWwindow* window) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
#pragma endregion

#pragma region - Practice Logic - 
void HideConsole() {
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

int GetWindowHeight() {
    int width, height;
    glfwGetWindowSize( window, &width, &height);
    return height;
}

int GetWindowWidth() {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    return width;
}

void PracticeApplication() {
    // Start create menu
    MenuTree();
    Viewport();
}

void MenuTree() {
    float height = GetWindowHeight();
    // Set position and size
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(GetWindowWidth() * 0.25f, height));

    ImGui::Begin("Main menu", nullptr, ImGuiWindowFlags_NoCollapse);
    CreateButtons();
    CreatePointsTree();
    ImGui::End();
}

void Viewport() {
    float height = GetWindowHeight();
    // Set position and size
    ImGui::SetNextWindowPos(ImVec2(GetWindowWidth() * 0.25f, 0));
    ImGui::SetNextWindowSize(ImVec2(GetWindowWidth() - GetWindowWidth() * 0.25f, height));

    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
    ViewportRender();
    ImGui::End();
}

void ViewportRender() {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    // Draw Grid
    DrawGrid(drawList);

    // Draw points Here
    if (set_of_points.get_size() > 0) {
        auto item = set_of_points[0];
        int id = 0;

        while(item != nullptr) {
            if(id != point_id)
                DrawPoint( green, drawList, item->data.position);
            else
                DrawPoint( red, drawList, item->data.position);
            item = item->next;
            ++id;
        }
    }
}

void DrawGrid(ImDrawList* drawList) {
    const float height = GetWindowHeight();
    const float width = GetWindowWidth();
    const float half_height = height / 2.0f;
    const float half_width = 0.75f * width / 2.0f + 0.25f * width;

    DrawMainAxes(drawList, half_width, width, half_height);
    DrawDividers(drawList,height, width, half_width, half_height);

    // Add Main Lines For X and Y
    ImColor white(0.7f, 0.7f, 0.7f);
    drawList->AddLine(ImVec2(half_width, 0), ImVec2(half_width, height), ImU32(white));
    drawList->AddLine(ImVec2(0, half_height), ImVec2(width, half_height), ImU32(white));
}

void DrawMainAxes(ImDrawList* drawList, float half_width, float width, float half_height) {
    std::string x = "x";
    std::string y = "y";

    // Add Y Text
    drawList->AddText(ImVec2(half_width + 4.0f, 20.0f), white, y.c_str());
    // Add X Text
    drawList->AddText(ImVec2(width - 20.0f, half_height), white, x.c_str());
}

void DrawDividers(ImDrawList* drawList, float height, float width, float half_width, float half_height) {
    ImVec2 center(half_width, half_height);
    // X Axis Dividers
    for (int i = 1; i < 12; ++i) {
        // Draw Grid Lines
        if (is_grid_active) {
            drawList->AddLine(ImVec2(center.x + 50.0f * i, 0), ImVec2(center.x + 50.0f * i, height), ImU32(dark_grey));
            drawList->AddLine(ImVec2(center.x - 50.0f * i, 0), ImVec2(center.x - 50.0f * i, height), ImU32(dark_grey));
        }

        drawList->AddLine(ImVec2(center.x + 50.0f * i, center.y + 4.0f), ImVec2(center.x + 50.0f * i, center.y - 4.0f), ImU32(white));
        drawList->AddLine(ImVec2(center.x - 50.0f * i, center.y + 4.0f), ImVec2(center.x - 50.0f * i, center.y - 4.0f), ImU32(white));
    }
    // Y Axis Dividers
    for (int i = 1; i < 12; ++i) {
        // Draw Grid Lines
        if (is_grid_active) {
            drawList->AddLine(ImVec2(0, center.y + 50.0f * i), ImVec2(width, center.y + 50.0f * i), ImU32(dark_grey));
            drawList->AddLine(ImVec2(0, center.y - 50.0f * i), ImVec2(width, center.y - 50.0f * i), ImU32(dark_grey));
        }

        drawList->AddLine(ImVec2(center.x + 4.0f, center.y + 50.0f * i), ImVec2(center.x - 4.0f, center.y + 50.0f * i), ImU32(white));
        drawList->AddLine(ImVec2(center.x + 4.0f, center.y - 50.0f * i), ImVec2(center.x - 4.0f, center.y - 50.0f * i), ImU32(white));
    }
}

void CreateButtons() {
    if (ImGui::Button("Import Points", ImVec2(GetWindowWidth() * 0.2f, 20)))
        ImGuiFileDialog::Instance()->OpenDialog("Import File Dialog", "Choose File", ".txt,.pts", ".");

    if (ImGui::Button("Export Points", ImVec2(GetWindowWidth() * 0.2f, 20)))
        ImGuiFileDialog::Instance()->OpenDialog("Export File Dialog", "Choose Path", ".txt,.pts", ".");

    ImGui::Checkbox("Activating the Grid", &is_grid_active);
    ImGui::Checkbox("Show Position", &show_position);

    if(point_id >= 0)
        ImGui::Text("Point ID with Lowest Summ Dist: %d", point_id);
    else
        ImGui::Text("Point ID with Lowest Summ Dist: unknown");

    OpenImportFileDialog();
    OpenExportFileDialog();
}

void OpenImportFileDialog() {
    // display
    if (ImGuiFileDialog::Instance()->Display("Import File Dialog", 32, ImVec2(200, 200))) {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            // action
            OnImport(filePathName);
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }
}

void OpenExportFileDialog() {
    // display
    if (ImGuiFileDialog::Instance()->Display("Export File Dialog", 32, ImVec2(200, 200))) {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            // action
            OnExport(filePathName);
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }
}

void OnImport(std::string importPath) {
    set_of_points.clear();

    std::ifstream file(importPath);
    std::string line;
    int id = 0;

    if (file.is_open()) {
        // Try parse file
        while (std::getline(file, line)) {
            auto pt = get_point(line);
            pt.id = id;
            set_of_points.push(pt);
            ++id;
        }
    }

    point_id = slow_algorithm(set_of_points);
    distances_info = get_distances(set_of_points);
}

void OnExport(std::string exportPath) {
    std::ofstream file(exportPath);
    if (file.is_open()) {
        for (int i = 0; i < set_of_points.get_size(); ++i) {
            auto item = set_of_points[i];
            file << item->data.position.x << "\t" << item->data.position.y << std::endl;
        }
    }
}

void DrawPoint(ImColor color, ImDrawList* drawList, ImVec2 realPos) {
    const float height = GetWindowHeight();
    const float width = GetWindowWidth();
    const float half_height = height / 2.0f;
    const float half_width = 0.75f * width / 2.0f + 0.25f * width;

    ImVec2 position(half_width + realPos.x, half_height - realPos.y);

    ImFont* font_current = ImGui::GetFont();
    ImVec2 text_position(position.x + 4.0f, position.y - 16.0f);
    if(show_position)
        drawList->AddText(text_position, white, GetStringFromVec2(realPos).c_str());
    drawList->AddCircleFilled(position, 2.0f, ImU32(color), 16);
}

std::string GetStringFromVec2(ImVec2 vec) {
    std::string result = "";
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << "[" << vec.x << "f," << vec.y << "f]";
    result = stream.str();
    return result;
}

void CreatePointsTree() {
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    const int columns_cnt = 3;

    // Options
    static ImGuiTableFlags flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable
        | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
        | ImGuiTableFlags_ScrollY;

    // Table with nodes
    if (ImGui::BeginTable("Points", columns_cnt + 1, flags, ImVec2(0.0f, TEXT_BASE_HEIGHT * 15), 0.0f)) {
        // Fill Info about cols
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("X Pos");
        ImGui::TableSetupColumn("Y Pos");
        ImGui::TableSetupColumn("Summ Dist");
        ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
        ImGui::TableHeadersRow();

        // fill table with info
        for (int cell = 0; cell < set_of_points.get_size(); ++cell) {
            std::list<float>::iterator it;
            if (distances_info.size() > 0)
                it = std::next(distances_info.begin(), cell);
            else
                it = distances_info.end();
            auto item = set_of_points[cell];
            auto data = item->data;
            ImGui::TableNextColumn();
            ImGui::Text("%d", data.id);
            ImGui::TableNextColumn();
            ImGui::Text("%f", data.position.x);
            ImGui::TableNextColumn();
            ImGui::Text("%f", data.position.y);
            ImGui::TableNextColumn();
            if (it != distances_info.end())
                ImGui::Text("%f", *it);
            else
                ImGui::Text("unknown");
        }

        ImGui::EndTable();
    }
}
#pragma endregion
