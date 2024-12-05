#pragma once
#include <glbinding/gl46core/gl.h>
using namespace gl46core;
#include <SDL3/SDL_init.h>
#include <glm/glm.hpp>
#include <fmt/base.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include "time.hpp"
#include "window.hpp"
#include "input.hpp"
#include "pipeline.hpp"
#include "entities/camera.hpp"
#include "entities/model.hpp"

struct Engine {
    void init() {
        Time::init();
        _window.init(1280, 720, "OpenGL Renderer", 4);
        _camera.set_perspective(1280, 720, 70);
        // create pipeline for textured objects
        _pipeline.init("../shaders/default.vert", "../shaders/default.frag");
        _pipeline_shadows.init("../shaders/shadows.vert", "../shaders/shadows.frag");
        _pipeline_shadows.create_framebuffer();
        // create the two cubes
        _cube.init(Mesh::eCube, "../textures/grass.png");
        _cube._transform._position = glm::vec3(-3, 0, -5);
        _sphere.init(Mesh::eSphere);
        _sphere._transform._position = glm::vec3(+3, 0, -5);
        _sponza.init("../models/sponza/sponza.obj");
        _sponza._transform._scale = glm::vec3(.01, .01, .01);
        // initialize ImGui for UI rendering
        ImGui::CreateContext();
        ImGui_ImplSDL3_InitForOpenGL(_window._window_p, _window._context);
        ImGui_ImplOpenGL3_Init();

        ////////////// stuff for the point light with shadows
        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &_shadow_texture);
        glTextureStorage2D(_shadow_texture, 1, GL_DEPTH_COMPONENT32F, _shadow_width, _shadow_height);
        // set wrapping/magnification behavior
        glTextureParameteri(_shadow_texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(_shadow_texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(_shadow_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(_shadow_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // create shadow camera matrices
        float range = 100; // range of the "light", therefore also the range of our shadow casting
        _shadow_projection = glm::perspectiveFov<float>(glm::radians(90.0f), _shadow_width, _shadow_height, 1.0f, range);
        _shadow_views[0] = glm::lookAt(_shadow_position, _shadow_position + glm::vec3(+1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)); // right
        _shadow_views[1] = glm::lookAt(_shadow_position, _shadow_position + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)); // left
        _shadow_views[2] = glm::lookAt(_shadow_position, _shadow_position + glm::vec3( 0.0f, +1.0f,  0.0f), glm::vec3(0.0f,  0.0f, +1.0f)); // top
        _shadow_views[3] = glm::lookAt(_shadow_position, _shadow_position + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)); // bottom
        _shadow_views[4] = glm::lookAt(_shadow_position, _shadow_position + glm::vec3( 0.0f,  0.0f, +1.0f), glm::vec3(0.0f, -1.0f,  0.0f)); // back
        _shadow_views[5] = glm::lookAt(_shadow_position, _shadow_position + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)); // front
    }
    void destroy() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        _pipeline.destroy();
        _window.destroy();
    }
    auto execute_event(SDL_Event* event_p) -> SDL_AppResult {
        // pass event over to imgui as well
        ImGui_ImplSDL3_ProcessEvent(event_p);
        // let input system process event
        Input::register_event(*event_p);
        switch (event_p->type) {
            case SDL_EventType::SDL_EVENT_QUIT: return SDL_AppResult::SDL_APP_SUCCESS;
            default: break;
        }
        return SDL_AppResult::SDL_APP_CONTINUE;   
    }
    void execute_input() {
        // camera movement
        float speed = 0.1f;
        if (Keys::down(SDLK_W)) _camera.translate(0, 0, -speed);
        if (Keys::down(SDLK_S)) _camera.translate(0, 0, +speed);
        if (Keys::down(SDLK_A)) _camera.translate(-speed, 0, 0);
        if (Keys::down(SDLK_D)) _camera.translate(+speed, 0, 0);
        if (Keys::down(SDLK_Q)) _camera.translate(0, -speed, 0);
        if (Keys::down(SDLK_E)) _camera.translate(0, +speed, 0);
        // camera rotation
        if (_mouse_captured) {
            float rotationSpeed = 0.005f;
            _camera._rotation.x -= rotationSpeed * Mouse::delta().second;
            _camera._rotation.y -= rotationSpeed * Mouse::delta().first;
        }

        // handle mouse capture
        if (_mouse_captured && Keys::pressed(SDLK_ESCAPE)) {
            _mouse_captured = false;
            SDL_SetWindowRelativeMouseMode(_window._window_p, _mouse_captured);
        }
        if (!_mouse_captured && Mouse::pressed(Mouse::ids::left)) {
            _mouse_captured = true;
            SDL_SetWindowRelativeMouseMode(_window._window_p, _mouse_captured);
        }
    }
    void execute_frame() {
        // update timer for accurate Time::get_delta()
        Time::update();
        // start new imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // draw some ui
        ImGui::Begin("FPS window");
        ImGui::Text("%.1f fps, %.2f ms", ImGui::GetIO().Framerate, Time::get_delta() * 1000.0);
        ImGui::End();

        // handle all the inputs such as camera movement
        execute_input();
        _cube._transform._rotation += Time::get_delta();

        // draw shadows
        {
            _pipeline_shadows.bind();
            glViewport(0, 0, _shadow_width, _shadow_height);
            // render into each cubemap face
            for (int face = 0; face < 6; face++) {
                // set framebuffer texture and clear it
                glNamedFramebufferTextureLayer(_pipeline_shadows._framebuffer, GL_DEPTH_ATTACHMENT, _shadow_texture, 0, face);
                glClear(GL_DEPTH_BUFFER_BIT);
                // bind the light view+projection matrices (act like it is the camera)
                glUniformMatrix4fv( 8, 1, false, glm::value_ptr(_shadow_views[face]));
                glUniformMatrix4fv(12, 1, false, glm::value_ptr(_shadow_projection));
                // draw the stuff
                _cube.draw(false);
                _sphere.draw(false);
                _sponza.draw(false);
            }
        }

        // draw color
        {
            // bind pipeline
            _pipeline.bind();
            // clear screen before drawing
            glClearColor(0.1, 0.1, 0.1, 0.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glViewport(0, 0, 1280, 720);
            glBindTextureUnit(1, _shadow_texture);
            _camera.bind();
            _cube.draw();
            _sphere.draw();
            _sponza.draw();
        }

        // present to the screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(_window._window_p);
        Input::flush();
    }

    Window _window;
    Camera _camera;
    Pipeline _pipeline;
    Pipeline _pipeline_shadows;
    Model _cube;
    Model _sphere;
    Model _sponza;
    // other
    bool _mouse_captured = false;

    /////////////// stuff for a single point light with shadows
    GLuint _shadow_width = 512;
    GLuint _shadow_height = 512;
    GLuint _shadow_texture; // cube map (6 textures)
    std::array<glm::mat4x4, 6> _shadow_views; // one view for each texture in cube map
    glm::mat4x4 _shadow_projection;
    glm::vec3 _shadow_position = {1.0, 3.0, -0.5};
};