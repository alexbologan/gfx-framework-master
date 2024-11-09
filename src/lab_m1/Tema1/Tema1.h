#pragma once

#include "components/simple_scene.h"

#include <vector>

namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
    public:
        Tema1();
        ~Tema1();

        void Init() override;

    private:
        void FrameStart() override;
        float GetHeight(float tank_x);
		void RenderTank_1(float deltaTime);
		void RenderTank_2(float deltaTime);
		void LaunchExplosionBits(float x, float y, float tank);
        void UpdateExplosionBits(float deltaTime, float tank);
        void UpdateRelease(float deltaTime);
        void UpdateCameraShaking(float deltaTimeSeconds);
        void RenderTerrain();
        void UpdateTerrainDeformation();
		void DrawTrajectoryLine(float cannonX, float cannonY, float cannon_angle, float angle, float deltaTime);
        void LaunchProjectile(float startX, float startY, float cannon_angle, float magnitude);
        void DeformTerrain(float impactX, float impactY, float radius);
		void UpdateProjectile(float deltaTime);
        void DrawLifeBar(float x, float y, float angle, float tank_life);
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

    protected:
        std::vector<float> heightMap;
        float cx, cy;
        glm::mat3 modelMatrix;
        float translateX, translateY;
        float scaleX, scaleY;
        float angularStep;
		float tank_x1, tank_y1, tank_x2, tank_y2, cannon_angle1, cannon_angle2;
		bool explosion_tank1 = false, explosion_tank2 = false;
		float tank1_life, tank2_life;

        float initialVelocity = 60.0f;
		float gravity = 9.8f;

        struct Projectile {
            glm::vec2 position;
            glm::vec2 velocity;
            bool isActive = false;
			float angle;
        };
        std::vector<Projectile> projectiles;

        struct Explosion_bit {
            glm::vec2 position;
            glm::vec2 velocity;
            float angle;
            float scale;
            float tank;
        };
        std::vector<Explosion_bit> Explosion_bits;

		struct Projectile_release_bit {
			glm::vec2 position;
			glm::vec2 velocity;
			float angle;
			float scale;
		};

		std::vector<Projectile_release_bit> Projectile_release_bits;

        bool isSpacePressed = false, isEnterPressed = false;

        bool isCameraShaking = false;
        float shakeDuration = 0.7f;
        float shakeIntensity = 5.0f;
        float shakeTime = 0.0f;
        glm::vec3 originalCameraPosition;

        GLboolean moveDiagonal, scaleUp;
    };
}   // namespace m1
