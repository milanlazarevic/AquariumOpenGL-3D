#include "../Header/foodManager.hpp"
#include <cstdlib>
#include <iostream>

FoodManager::FoodManager(Shader* shader, Model* foodParticleModel)
    : shader(shader), foodParticleModel(foodParticleModel) {}

void FoodManager::spawnFood(int count) {
    for (int i = 0; i < count; i++) {
        float x = -3.0f + static_cast<float>( rand() ) / RAND_MAX * 6.0f;
        float y = 1.0f;
        float z = -3.0f;
        float speed = -0.015f - static_cast<float>( rand() ) / RAND_MAX * 0.02f;
        particles.push_back(FoodParticle(foodParticleModel, speed,x, y, z));
    }
}

void FoodManager::update(float bottomBound) {
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const FoodParticle& p) { return !p.isActive() && p.isEaten(); }),
        particles.end()
    );

    for (auto& particle : particles) {
        particle.update(bottomBound);
    }
}

void FoodManager::draw() {
    for (auto& particle : particles) {
        particle.draw(shader);
    }
}

void FoodManager::checkCollisions(float fishXMin, float fishXMax,
                                   float fishYMin, float fishYMax,
                                   float fishZMin, float fishZMax, std::function<void()> onEat) {
    for (auto& particle : particles) {
        if (particle.isActive() && !particle.isEaten()) {
            if (particle.checkCollision(fishXMin, fishXMax, fishYMin, fishYMax, fishZMin, fishZMax)) {
                particle.markAsEaten();
                if (onEat) onEat();
            }
        }
    }
}