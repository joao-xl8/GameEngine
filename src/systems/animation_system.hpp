#pragma once
#include "system.hpp"
#include "../components/engine_components.hpp"

class AnimationSystem : public System {
public:
    void update(EntityManager& entities, float dt) override {
        for (auto& entity : entities.getEntitiesWith<CAnimation, CSprite>()) {
            auto animation = entity->getComponent<CAnimation>();
            auto sprite = entity->getComponent<CSprite>();
            
            updateAnimation(animation, sprite->sprite, dt);
        }
    }

private:
    void updateAnimation(std::shared_ptr<CAnimation> animation, sf::Sprite& sprite, float deltaTime) {
        if (animation->animations.find(animation->currentAnimation) == animation->animations.end())
            return;

        animation->frameTimer += deltaTime;

        // Check if it's time to advance to next frame
        if (animation->frameTimer >= animation->frameTime) {
            animation->frameTimer = 0.0f;
            animation->currentFrame++;

            auto &animData = animation->animations[animation->currentAnimation];

            // Loop animation if repeat is true
            if (animation->currentFrame >= animData.frameCount) {
                if (animData.repeat) {
                    animation->currentFrame = 0;
                } else {
                    animation->currentFrame = animData.frameCount - 1;
                }
            }
        }

        // Update sprite texture rectangle
        updateSpriteFrame(animation, sprite);
    }

    void updateSpriteFrame(std::shared_ptr<CAnimation> animation, sf::Sprite& sprite) {
        if (animation->animations.find(animation->currentAnimation) == animation->animations.end())
            return;

        auto &animData = animation->animations[animation->currentAnimation];

        int frameX = animation->currentFrame * animation->frameSize.x;
        int frameY = animData.row * animation->frameSize.y;

        // Set texture rectangle with flipping support
        if (animData.flipX) {
            sprite.setTextureRect(
                sf::IntRect(frameX + animation->frameSize.x, frameY, -animation->frameSize.x, animation->frameSize.y));
        } else {
            sprite.setTextureRect(
                sf::IntRect(frameX, frameY, animation->frameSize.x, animation->frameSize.y));
        }
    }
};
