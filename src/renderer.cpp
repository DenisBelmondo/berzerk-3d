#include <Color.hpp>
#include <iostream>
#include <Matrix.hpp>
#include <MeshUnmanaged.hpp>
#include <raylib.h>
#include <raylib.hpp>
#include <raymath.hpp>
#include <Rectangle.hpp>
#include <Vector2.hpp>
#include <Vector3.hpp>
#include "berzerk.hpp"
#include "renderer.hpp"
#include "sounds.hpp"

using namespace bm;
using namespace bm::berzerk;

Model Renderer::playerBulletModel;
raylib::MeshUnmanaged Renderer::wallMesh;
raylib::ShaderUnmanaged Renderer::wallShader;
Material Renderer::wallMaterial;
raylib::TextureUnmanaged Renderer::textureRobot;
raylib::TextureUnmanaged Renderer::textureVPistol0;
raylib::TextureUnmanaged Renderer::textureVPistol1;
Sound Renderer::sounds[std::size_t(SoundID::COUNT)];

void Renderer::initialize() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(640, 480, "Berzerk");
    InitAudioDevice();
    playerBulletModel = LoadModelFromMesh(raylib::MeshUnmanaged::Cube(0.25F, -0.25F, 1.0F));
    wallMesh = raylib::MeshUnmanaged::Cube(1, 1, 1);
    wallMesh.Upload();
    wallShader = LoadShader("static/shaders/wall.vert", "static/shaders/wall.frag");
    wallShader.locs[SHADER_LOC_MATRIX_MVP] = wallShader.GetLocation("mvp");
    wallShader.locs[SHADER_LOC_VECTOR_VIEW] = wallShader.GetLocation("viewPos");
    wallMaterial = LoadMaterialDefault();
    wallMaterial.shader = wallShader;
    textureRobot.Load("static/textures/robot.png");
    textureVPistol0.Load("static/textures/v_pistol_0.png");
    textureVPistol1.Load("static/textures/v_pistol_1.png");
    sounds[std::size_t(SoundID::PISTOL_FIRE)] = LoadSound("static/audio/pistol_fire.wav");
    sounds[std::size_t(SoundID::ROBOT_EXPLODE)] = LoadSound("static/audio/robot_explode.wav");

    playSound = [](SoundID sound) {
        PlaySound(sounds[std::size_t(sound)]);
    };

    camera = raylib::Camera({}, {}, {0, 1, 0}, 90);
    vSpriteCamera = raylib::Camera({}, {0, 0, -1}, {0, 1, 0}, 1.2F, CAMERA_ORTHOGRAPHIC);
}

void Renderer::deinitialize() {
    UnloadModel(playerBulletModel);
    textureRobot.Unload();
    textureVPistol0.Unload();
    textureVPistol1.Unload();
    wallMesh.Unload();
    CloseAudioDevice();
    CloseWindow();
}

void Renderer::render() {
    raylib::Vector3 playerPosition3d(world->playerTransform.position.x, 0, world->playerTransform.position.y);
    raylib::Vector3 playerDirection3d(world->playerTransform.direction.x, 0, world->playerTransform.direction.y);

    camera.position = playerPosition3d;
    camera.target = playerPosition3d + playerDirection3d;

    static float cameraPos[3];

    cameraPos[0] = camera.position.x;
    cameraPos[1] = camera.position.y;
    cameraPos[2] = camera.position.z;
    wallShader.SetValue(wallShader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

    bm::berzerk::shouldStop |= WindowShouldClose();
    BeginDrawing();
    ClearBackground(raylib::Color::Black());
    camera.BeginMode();

    for (std::size_t i = 0; i < std::pow(world->tileMap.pitch, 2); i++) {
        if ((*world->tileMap.flat)[i] > 0) {
            float x = int(i % world->tileMap.pitch);
            float y = int(i / world->tileMap.pitch);

            x += 0.5F;
            y += 0.5F;
            raylib::Matrix transform = raylib::Matrix::Translate(x, 0, y);
            wallMesh.Draw(wallMaterial, transform);
        }
    }

    for (std::size_t i = 0; i < world->robot.count; i++) {
        int frame = world->robot.dense[i].value.data.frame;
        auto &transform = world->robot.dense[i].value.data.transform;

        DrawBillboardRec(
            camera,
            textureRobot,
            raylib::Rectangle(frame * 16, 0, 16, 18),
            { transform.position.x, 0, transform.position.y },
            raylib::Vector2::One(),
            raylib::Color::White());
    }

    for (std::size_t i = 0; i < world->playerBullet.count; i++) {
        auto &transform = world->playerBullet.dense[i].value.data;

        DrawModelEx(
            playerBulletModel,
            { transform.position.x, -0.25F, transform.position.y },
            { 0, 1, 0 },
            raylib::Vector2(0, -1)
                .Angle({ -transform.direction.x, transform.direction.y })
                * RAD2DEG,
            { 0.25F, 0.25F, 0.25F },
            raylib::Color::Yellow());
    }

    camera.EndMode();
    vSpriteCamera.BeginMode();

    raylib::TextureUnmanaged *vSpriteTexture = &textureVPistol0;

    if (world->weaponState == WeaponState::FIRING) {
        vSpriteTexture = &textureVPistol1;
    }

    vSpriteOffset.x = std::sin(GetTime() * 10.0F) / 20.0F;
    vSpriteOffset.y = -0.1F + std::sin(GetTime() * 20.0F) / 20.0F;
    vSpriteOffset *= world->playerBob;
    vSpriteCamera.DrawBillboard(*vSpriteTexture, raylib::Vector3(0, -0.1F, -1) + raylib::Vector3(vSpriteOffset.x, vSpriteOffset.y, 0), 1);
    vSpriteCamera.EndMode();

    static char scoreText[80];
    static char enemiesLeftText[120];

    std::snprintf(scoreText, 80, "%d", world->playerScore);
    DrawText(scoreText, 0, 0, 20, raylib::Color::RayWhite());
    std::snprintf(enemiesLeftText, 120, "Robots: %zu\n", world->robot.count);
    DrawText(enemiesLeftText, 0, 20, 20, raylib::Color::RayWhite());
    EndDrawing();
}
