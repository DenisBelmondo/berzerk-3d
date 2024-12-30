#ifndef BERZERK_H
#define BERZERK_H

#include "math.hpp"
#include "sounds.hpp"
#include "sparse_set.hpp"
#include <vector>

namespace bm::berzerk {

enum class WeaponState {
    READY,
    FIRING,
};

struct TileMap {
    const std::vector<int> *flat = nullptr;
    std::size_t pitch = 0;

    TileMap() = default;
    TileMap(const std::vector<int> *flat)
        : flat(flat), pitch(std::sqrt(flat->size())) {}
};

struct Robot {
    enum class State {
        CHASING,
        DYING,
    };

    math::Transform transform {};
    State state = State::CHASING;
    float frame = 0;
};

struct World {
    template <typename T> struct Linked {
        T data;
        bool flaggedForDeletion = false;

        Linked(const T &data) : data(data) {}
    };

    const float WEAPON_COOLDOWN_SECONDS = 0.25F;

    TileMap tileMap;
    SparseSet<Linked<Robot>> robot;
    SparseSet<Linked<math::Transform>> robotBullet;
    SparseSet<Linked<math::Transform>> playerBullet;
    math::Transform playerTransform;
    int playerScore;
    WeaponState weaponState;
    float playerBob;
    float weaponTimerSeconds;

private:
    void tickPlayer(double delta);
    void tickPlayerBullet(Linked<math::Transform> &linked, double delta);
    void tickWeapon(double delta);
    void tickRobot(Linked<Robot> &linked, double delta);
    void tickRobotBullet(Linked<math::Transform> &linked, double delta);

public:
    void tick(double delta);
    void spawnPlayerBullet(const math::Transform &where);
    void spawnRobot(const math::Transform &where);
    void spawnRobotBullet(const math::Transform &where);
    void killRobot(Linked<Robot> &robot);
    void removeRobot(std::size_t sparseIndex);
    void removeRobotBullet(std::size_t sparseIndex);
    void removePlayerBullet(std::size_t sparseIndex);
    void move(math::Transform &transform, const raylib::Vector2 &motion);
    void nextLayout();
};

extern void (*playSound)(SoundID);
extern bool shouldStop;
extern std::vector<std::vector<int>> layouts;

} // namespace bm::berzerk

#endif // BERZERK_H
