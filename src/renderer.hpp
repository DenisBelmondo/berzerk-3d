#ifndef RENDERER_H
#define RENDERER_H

#include <Camera3D.hpp>
#include <MeshUnmanaged.hpp>
#include <raylib.hpp>
#include <ShaderUnmanaged.hpp>
#include <TextureUnmanaged.hpp>
#include "berzerk.hpp"

namespace bm::berzerk {

struct Renderer {
    static Model playerBulletModel;
    static raylib::MeshUnmanaged wallMesh;
    static raylib::ShaderUnmanaged wallShader;
    static Material wallMaterial;
    static raylib::TextureUnmanaged textureRobot;
    static raylib::TextureUnmanaged textureVPistol0;
    static raylib::TextureUnmanaged textureVPistol1;
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
