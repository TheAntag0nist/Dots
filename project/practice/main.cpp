#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "FileDialog/ImGuiFileDialog.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
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

void OnImport(std::string importPath);
void OnExport(std::string exportPath);

void DrawPoint(ImColor color, ImVec2 position, ImDrawList* drawList);

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

    // Draw points Here
    DrawPoint(ImColor(0.0f, 1.0f, 0.0f), ImVec2(420, 420), drawList);
    DrawPoint(ImColor(1.0f, 0.0f, 0.0f), ImVec2(520, 520), drawList);
    DrawPoint(ImColor(0.0f, 1.0f, 0.0f), ImVec2(620, 620), drawList);
}

void CreateButtons() {
    if (ImGui::Button("Import Points", ImVec2(GetWindowWidth() * 0.2f, 20)))
        ImGuiFileDialog::Instance()->OpenDialog("Import File Dialog", "Choose File", ".pts,.txt", ".");

    if (ImGui::Button("Export Points", ImVec2(GetWindowWidth() * 0.2f, 20)))
        ImGuiFileDialog::Instance()->OpenDialog("Export File Dialog", "Choose Path", ".pts,.txt", ".");

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
    std::ifstream file(importPath);
    if (file.is_open()) {
        // Try parse file
    }
}

void OnExport(std::string exportPath) {
    std::ofstream file(exportPath);
    if (file.is_open()) {
        // Try parse file
    }
}

void DrawPoint(ImColor color, ImVec2 position, ImDrawList* drawList) {
    ImFont* font_current = ImGui::GetFont();
    ImColor white(1.0f, 1.0f, 1.0f);
    ImVec2 text_position(position.x + 4.0f, position.y - 16.0f);
    drawList->AddText(text_position, white, GetStringFromVec2(position).c_str());
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
    const int columns_cnt = 4;

    // Options
    static ImGuiTableFlags flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable
        | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
        | ImGuiTableFlags_ScrollY;

    if (ImGui::TreeNode("Points Tree")) {
        // Table with nodes
        if (ImGui::BeginTable("Points", columns_cnt, flags, ImVec2(0.0f, TEXT_BASE_HEIGHT * 15), 0.0f)) {
            // Fill Info about cols
            ImGui::TableSetupColumn("ID");
            ImGui::TableSetupColumn("X Pos");
            ImGui::TableSetupColumn("Y Pos");
            ImGui::TableSetupColumn("Summ Distance");
            ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
            ImGui::TableHeadersRow();

            // fill table with info
            for (int cell = 0; cell < 12; cell++) {
                ImGui::TableNextColumn();
                ImGui::Text("this cell %d", cell);
            }

            ImGui::EndTable();
        }

        ImGui::TreePop();
    }
}
#pragma endregion
