#ifndef _INCLUDE__GEARE__GRAPHICS__PERSPECTIVE_CAMERA_
#define _INCLUDE__GEARE__GRAPHICS__PERSPECTIVE_CAMERA_

#include "../backend/GLFW.hpp"
#include "../base/Transform.hpp"
#include "../windowing/Window.hpp"

namespace geare::graphics {

struct PerspectiveCamera {
  float fov = 90.f;
  float far_plane = .1f;
  float near_plane = 100.f;
  int width, height;

  bool is_dirty = true;
  bool main = false;

  GLuint fbo;
  GLuint color_tex;
  GLuint depth_and_stencil_rbo;
};

struct PerspectiveCameraSystem : core::Executor::System {
  virtual auto task(core::Executor *exec) -> core::Executor::Task override {
    auto camera_entities =
        co_await exec
            ->get_components<PerspectiveCamera, const base::Transform>();

    static GLuint shaderProgram;
    static GLuint transformShaderLoc;

    static bool first_run = true;
    if (first_run) {
      initialize_builtin_shaders(shaderProgram, transformShaderLoc);
      first_run = false;
    }

    for (auto e : camera_entities) {
      auto [camera, transform] =
          exec->registry->get<PerspectiveCamera, const base::Transform>(e);

      glfwGetFramebufferSize(windowing::Window::instance().window,
                             &camera.width, &camera.height);

      if (camera.is_dirty) {
        initialize_camera(camera);
      }

      glBindFramebuffer(GL_FRAMEBUFFER, camera.fbo);
      // TODO: maybe don't resize every frame
      glViewport(0, 0, camera.width, camera.height);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glFlush();

      // CAMERA RENDERING LOGIC GOES HERE;
    }

    camera_entities.release();
  }

  void initialize_camera(PerspectiveCamera &camera) {
    if (camera.main)
      return;

    glGenFramebuffers(1, &camera.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, camera.fbo);

    // Generate the color texture
    glGenTextures(1, &camera.color_tex);
    glBindTexture(GL_TEXTURE_2D, camera.color_tex);

    // Bind the color texture to the framebuffer
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, camera.width, camera.height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           camera.color_tex, 0);

    // Create and bind the stencil and depth to the framebuffer
    glGenRenderbuffers(1, &camera.depth_and_stencil_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, camera.depth_and_stencil_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, camera.width,
                          camera.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, camera.depth_and_stencil_rbo);

    camera.is_dirty = false;
  }

  void initialize_builtin_shaders(GLuint &shaderProgram,
                                  GLuint &transformShaderLoc) {
    const char *vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 transform;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = transform * vec4(aPos, 1.0);\n"
        "}\0";

    const char *fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\0";

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                << infoLog << std::endl;
      std::terminate();
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                << infoLog << std::endl;
      std::terminate();
    }

    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    transformShaderLoc = glGetUniformLocation(shaderProgram, "transform");
  }
};

} // namespace geare::graphics

#endif
