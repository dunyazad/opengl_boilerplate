#include <iostream>
#include <functional>
#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <GLFW/glfw3.h>
#include <imgui.h>

#ifdef LIB_GLFW
	#include <imgui_impl_glfw.h>
#endif

#ifdef LIB_SDL
	#include <imgui_impl_sdl.h>
#endif

#include <imgui_impl_opengl3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

class WindowClass {
public:
	WindowClass() {
		glfwInit();
	}

	~WindowClass() {
		destroyWindow();
		glfwTerminate();
	}

	bool create(unsigned int width, unsigned int height, const char * title) {
		destroyWindow();
		window = glfwCreateWindow(width, height, title, nullptr, nullptr);

		if (!window) {
			return false;
		}

		glfwMakeContextCurrent(window);

		return true;
	}

	void start(std::function<void()> onUpdate) {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			onUpdate();

			int display_w, display_h;
			glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glfwSwapBuffers(window);
		}
	}

	void destroyWindow() {
		if (window) {
			glfwDestroyWindow(window);
			window = nullptr;
		}
	}

	GLFWwindow * getWindow() const {
		return window;
	}

private:
	GLFWwindow * window = nullptr;
};

int main () {
	WindowClass window;

	std::cout << "Creating window..." << std::flush;

	if (!window.create(640, 480, "glub")) {
		std::cout << "FAILED" << std::endl;
		return 1;
	}

	std::cout << "OK" << std::endl;
	
	std::cout << "Initializing GLAD..." << std::flush;

	#ifdef LIB_GLFW
		auto gladAddress = glfwGetProcAddress;
	#endif

	#ifdef LIB_SDL
		auto gladAddress = SDL_GL_GetProcAddress;
	#endif

	if (!gladLoadGLLoader((GLADloadproc) gladAddress)) {
		std::cout << "FAILED" << std::endl;
		return 1;
	}


	std::cout << "OK" << std::endl;
	std::cout << "Initializing Freetype..." << std::flush;

	FT_Library ft;

	if (FT_Init_FreeType(&ft)) {
		std::cout << "FAILED" << std::endl;
		return 1;
	}

	std::cout << "OK" << std::endl;
	std::cout << "Initializing Imgui..." << std::flush;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void) io;
	ImGui::StyleColorsDark();

	#ifdef LIB_GLFW
		ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
	#endif

	#ifdef LIB_SDL
		ImGui_ImplSDL2_InitForOpenGL(window.getWindow(), window.getContext());
	#endif

	ImGui_ImplOpenGL3_Init("#version 330");
	bool demoWindow = true;

	std::cout << "OK" << std::endl;
	std::cout << "Initializing STB..." << std::flush;

	int width = 100;
	int height = 100;
	int channels = 3;
	int index = 0;
	uint8_t * pixels = new uint8_t[width * height * channels];

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			pixels[index++] = int(255.99 * ((float)j / (float)width));
			pixels[index++] = int(255.99 * ((float)i / (float)height));
			pixels[index++] = int(255.99 * 0.5);
		}
	}

	stbi_write_png("stb_test.png", width, height, channels, pixels, width * channels);

	unsigned char * img = stbi_load("stb_test.png", &width, &height, &channels, 0);

	if (img == nullptr) {
		std::cout << "Failed" << std::endl;
		return 1;
	}

	std::cout << "OK" << std::endl;

	std::function<void()> update = [&] () {
		ImGui_ImplOpenGL3_NewFrame();

		#ifdef LIB_GLFW
			ImGui_ImplGlfw_NewFrame();
		#endif

		#ifdef LIB_SDL
			ImGui_ImplSDL2_NewFrame(window.getWindow());
		#endif

		ImGui::NewFrame();
		ImGui::ShowDemoWindow(&demoWindow);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	};

	window.start(update);

	return 0;
}
