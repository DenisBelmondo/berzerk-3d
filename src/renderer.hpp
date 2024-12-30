#ifndef RENDERER_H
#define RENDERER_H

#include "berzerk.hpp"
#include <Camera3D.hpp>
#include <raylib.hpp>

namespace bm::berzerk {

struct Renderer {
    static Model playerBulletModel;
    static Mesh wallMesh;
    static Shader wallShader;
    static Material wallMaterial;
    static Texture textureRobot;
    static Texture textureVPistol0;
    static Texture textureVPistol1;
    static Sound sounds[std::size_t(SoundID::COUNT)];

    const World *world;
    raylib::Camera camera;
    raylib::Camera vSpriteCamera;
    raylib::Vector2 vSpriteOffset;

    Renderer(const World *world) : world(world) {}
    void initialize();
    void deinitialize();
    void render();
};

}; // namespace bm::berzerk

#endif // RENDERER_H
