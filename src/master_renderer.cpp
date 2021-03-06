
#include "master_renderer.hpp"

#include "camera.hpp"
#include "entity.hpp"
#include "light.hpp"
#include "loader.hpp"
#include "player.hpp"
#include "entity_renderer.hpp"
#include "static_shader.hpp"
#include "terrain_renderer.hpp"
#include "terrain_shader.hpp"
#include "textured_model.hpp"

#include "SDL.h"

#include <glad/glad.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <unordered_map>
#include <vector>

namespace {

constexpr float fov = 70; // degrees
constexpr float near_plane = 0.1f;
constexpr float far_plane = 1000.0f;
constexpr float sky_red = 0.35f;//0.53f;
constexpr float sky_green = 0.46f;//0.81f;
constexpr float sky_blue = 0.5;//0.92f;

}


namespace jac {

glm::mat4 master_renderer::create_projection_matrix()
{
    int w, h;
    SDL_GL_GetDrawableSize(SDL_GL_GetCurrentWindow(), &w, &h);
    // This is certainly easier than the Java version...
    return glm::perspectiveFov(glm::radians(fov), float(w), float(h),
                               near_plane, far_plane);
}


master_renderer::master_renderer(loader& loader)
    : skybox_renderer{loader, projection_matrix}
{
    enable_culling();
}

void master_renderer::prepare() const
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(sky_red, sky_green, sky_blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void master_renderer::render(const std::vector<light>& lights,
                             const jac::camera& cam,
                             const glm::vec4& clip_plane)
{
    prepare();
    shader.start();
    shader.load_clip_plane(clip_plane);
    shader.load_sky_colour(sky_red, sky_green, sky_blue);
    shader.load_lights(lights);
    shader.load_view_matrix(cam);
    entity_renderer.render(entities);
    shader.stop();

    terrain_shader.start();
    terrain_shader.load_clip_plane(clip_plane);
    terrain_shader.load_sky_colour(sky_red, sky_green, sky_blue);
    terrain_shader.load_lights(lights);
    terrain_shader.load_view_matrix(cam);
    terrain_renderer.render(terrains);
    terrain_shader.stop();

    skybox_renderer.render(cam, sky_red, sky_green, sky_blue);

    terrains.clear();
    entities.clear();
}

void master_renderer::process_terrain(const terrain& terrain)
{
    terrains.push_back(&terrain);
}

void master_renderer::process_entity(const entity& entity)
{
    const auto& entity_model = entity.get_model();

    auto& batch = entities[entity_model];
    batch.push_back(&entity);
}

void master_renderer::enable_culling()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void master_renderer::disable_culling()
{
    glDisable(GL_CULL_FACE);
}

void master_renderer::render_scene(const std::vector<jac::entity>& entities,
                                   const jac::player& player,
                                   const jac::terrain& terrain,
                                   const std::vector<light>& lights,
                                   const jac::camera& camera,
                                   const glm::vec4& clip_plane)
{
    process_terrain(terrain);
    process_entity(player);

    for (const auto& e : entities) {
        process_entity(e);
    }

    render(lights, camera, clip_plane);
}


}
