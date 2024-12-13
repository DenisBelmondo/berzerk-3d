#include <cmath>
#include <cstring>
#include <iostream>
#include <raylib.h>
#include "berzerk.hpp"
#include "math.hpp"
#include "sounds.hpp"

using namespace bm;
using namespace bm::berzerk;

bool bm::berzerk::shouldStop = false;
void (*bm::berzerk::playSound)(SoundID) = nullptr;
std::vector<std::vector<int>> bm::berzerk::layouts = {
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0,-2, 0, 0, 0, 0, 0, 1,
        1, 0, 1, 1, 0, 0, 0, 0, 0, 1,
        1, 0, 1, 1, 0,-2, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 1, 1, 0, 1,
        1, 0, 0, 0, 0, 0,-2, 1, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1, 0, 1,
        1, 0,-1, 0, 0, 0, 0, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    },
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0,-2, 1, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 1, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 1, 1, 1, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 1, 0, 1, 0, 1,
        1, 0, 0, 1, 1, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0,-2, 1, 1,-2, 1,
        1, 0,-1, 0, 0, 0, 1, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 1, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    },
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0,-2, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 1, 1, 1, 1, 0, 0, 1,
        1, 0, 0, 1, 1, 1, 1, 0, 0, 1,
        1, 0, 0, 1, 1, 1, 1, 0, 0, 1,
        1, 0, 0, 0, 0, 0,-2, 0, 0, 1,
        1, 0,-1, 0, 0, 0, 0, 0, 1, 1,
        1, 0, 0, 0, 0, 0, 0,-2, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    },
};

void World::killRobot(Linked<Robot> &robot) {
    robot.data.state = Robot::State::DYING;
    robot.data.frame = 2;
    playSound(SoundID::ROBOT_EXPLODE);
}

void World::tick(double delta) {
    tickPlayer(delta);
    tickWeapon(delta);

    auto [robotBegin, robotEnd] = robot.iterator();

    for (auto it = robotBegin; it != robotEnd; it++) {
        tickRobot(it->value, delta);
    }

    auto [playerBulletBegin, playerBullentEnd] = playerBullet.iterator();

    for (auto it = playerBulletBegin; it != playerBullentEnd; it++) {
        tickPlayerBullet(it->value, delta);
    }

    std::tie(robotBegin, robotEnd) = robot.iterator();

    for (auto it = robotBegin; it != robotEnd; it++) {
        if (it->value.flaggedForDeletion) {
            removeRobot(it->sparseIndex);
        }
    }

    std::tie(playerBulletBegin, playerBullentEnd) = playerBullet.iterator();

    for (auto it = playerBulletBegin; it != playerBullentEnd; it++) {
        if (it->value.flaggedForDeletion) {
            removePlayerBullet(it->sparseIndex);
        }
    }
}

void World::tickPlayer(double delta) {
    static const float LOOK_SPEED = 4;
    static const float MOVE_SPEED = 5;

    float look = float(IsKeyDown(KEY_RIGHT)) - float(IsKeyDown(KEY_LEFT));

    look *= LOOK_SPEED;
    look *= delta;
    playerTransform.direction = playerTransform.direction.Rotate(look);

    float forwardAxis = float(IsKeyDown(KEY_W)) - float(IsKeyDown(KEY_S));
    float strafeAxis = float(IsKeyDown(KEY_D)) - float(IsKeyDown(KEY_A));

    raylib::Vector2 forwardMove {};

    forwardMove += playerTransform.direction;
    forwardMove *= forwardAxis;

    raylib::Vector2 strafeMove {};

    strafeMove += raylib::Vector2(-playerTransform.direction.y, playerTransform.direction.x);
    strafeMove *= strafeAxis;

    raylib::Vector2 moveVector {};

    moveVector += forwardMove;
    moveVector += strafeMove;
    moveVector = moveVector.Normalize();
    moveVector *= MOVE_SPEED;
    moveVector *= delta;
    playerTransform.direction = playerTransform.direction;
    move(playerTransform, moveVector);
    playerBob = moveVector.LengthSqr() > 0;
}

void World::tickWeapon(double delta) {
    if (IsKeyDown(KEY_LEFT_CONTROL) && weaponState == WeaponState::READY) {
        weaponState = WeaponState::FIRING;
        weaponTimerSeconds = WEAPON_COOLDOWN_SECONDS;
        spawnPlayerBullet(playerTransform);
        playSound(SoundID::PISTOL_FIRE);
    }

    if (weaponState == WeaponState::FIRING) {
        weaponTimerSeconds -= delta;

        if (weaponTimerSeconds <= 0) {
            weaponState = WeaponState::READY;
            weaponTimerSeconds = WEAPON_COOLDOWN_SECONDS;
        }
    }
}

void World::tickRobot(Linked<Robot> &linked, double delta) {
    if (linked.data.state == Robot::State::CHASING) {
        move(linked.data.transform, (playerTransform.position - linked.data.transform.position).Normalize() * delta);
        linked.data.frame += 3.0F * delta;
        linked.data.frame = std::fmod(linked.data.frame, 2);
    } else if (linked.data.state == Robot::State::DYING) {
        linked.data.frame += 8.0F * delta;

        if (linked.data.frame >= 6.0F) {
            linked.flaggedForDeletion = true;
        }
    }
}

void World::tickRobotBullet(Linked<math::Transform> &linked, double delta) {
    static const auto ROBOT_BULLET_SPEED = 20.0F;
    static const auto ROBOT_BULLET_DELETE_DISTANCE_SQUARED = 40.0F * 40.0F;
    bool shouldDelete = false;

    linked.data.position += linked.data.direction * ROBOT_BULLET_SPEED * delta;
    shouldDelete |= linked.data.position.DistanceSqr({}) >= ROBOT_BULLET_DELETE_DISTANCE_SQUARED;
    shouldDelete |= (*tileMap.flat)[int(linked.data.position.y) * tileMap.pitch + int(linked.data.position.x)] > 0;

    // TODO: broadphase

    if (linked.data.position.DistanceSqr(playerTransform.position) < 0.5F) {
        // TODO: kill player
        shouldDelete |= true;
    }

    linked.flaggedForDeletion = shouldDelete;
}

void World::tickPlayerBullet(Linked<math::Transform> &linked, double delta) {
    static const auto PLAYER_BULLET_SPEED = 40.0F;
    static const auto PLAYER_BULLET_DELETE_DISTANCE_SQUARED = 40.0F * 40.0F;
    bool shouldDelete = false;

    linked.data.position += linked.data.direction * PLAYER_BULLET_SPEED * delta;
    shouldDelete |= linked.data.position.DistanceSqr({}) >= PLAYER_BULLET_DELETE_DISTANCE_SQUARED;
    shouldDelete |= (*tileMap.flat)[int(linked.data.position.y) * tileMap.pitch + int(linked.data.position.x)] > 0;

    // TODO: broadphase

    auto [begin, end] = robot.iterator();

    for (auto it = begin; it != end; it++) {
        if (it->value.data.state == Robot::State::DYING) {
            continue;
        }

        if (linked.data.position.DistanceSqr(it->value.data.transform.position) < 0.5F) {
            shouldDelete |= true;
            killRobot(it->value);
            playerScore += 100;
            break;
        }
    }

    linked.flaggedForDeletion = shouldDelete;
}

void World::spawnRobot(const math::Transform &where) {
    robot.add(Robot { where });
}

void World::spawnRobotBullet(const math::Transform &where) {
    robotBullet.add(where);
}

void World::spawnPlayerBullet(const math::Transform &where) {
    playerBullet.add(where);
}

void World::removeRobot(std::size_t handle) {
    robot.remove(handle);

    if (robot.count <= 0) {
        nextLayout();
    }
}

void World::removePlayerBullet(std::size_t handle) {
    playerBullet.remove(handle);
}

void World::move(math::Transform &transform, const raylib::Vector2 &motion) {
    transform.position += motion;

    if ((*tileMap.flat)[int(transform.position.y) * tileMap.pitch + int(transform.position.x)] > 0) {
        transform.position.x = std::round(transform.position.x);
        transform.position.y = std::round(transform.position.y);
    }
}

void World::nextLayout() {
    tileMap = TileMap(&layouts[GetRandomValue(0, layouts.size() - 1)]);

    for (std::size_t i = 0; i < tileMap.flat->size(); i++) {
        int value = (*tileMap.flat)[i];
        int x = i % tileMap.pitch;
        int y = i / tileMap.pitch;
        raylib::Vector2 center = raylib::Vector2::One() * (tileMap.pitch / 2.0F);

        center += raylib::Vector2::One() * 0.5F;

        if (value == -1) {
            playerTransform.position.x = x;
            playerTransform.position.y = y;
            playerTransform.position += raylib::Vector2::One() * 0.5F;
            playerTransform.direction = (center - playerTransform.position).Normalize();
        } else if (value == -2) {
            spawnRobot(math::Transform { raylib::Vector2(x, y) + raylib::Vector2::One() * 0.5F });
        }
    }
}
