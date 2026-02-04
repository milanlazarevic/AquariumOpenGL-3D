#pragma once
#include <vector>
#include "foodParticle.hpp"
#include <functional>


class FoodManager {
public:
    FoodManager(Shader* shader, Model* foodParticleModel);

    void spawnFood(int count = 3);
    void update(float bottomBound);
    void draw();
    void checkCollisions(float fishXMin, float fishXMax, float fishYMin, float fishYMax, float fishZMin, float fishZMax,std::function<void()> onEat);

private:
    std::vector<FoodParticle> particles;
    Shader* shader;
    Model* foodParticleModel;
};