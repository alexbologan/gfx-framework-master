#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>

#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D.h"

using namespace std;
using namespace m1;
using namespace irrklang;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

ISoundEngine* SoundEngine = createIrrKlangDevice();

Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}



float TerrainFunction(float x) {
	float A1 = 10, A2 = 10, A3 = 5, A4 = 5;  // Amplitudes
	float W1 = 0.017, W2 = 0.032, W3 = 0.025, W4 = 0.0425;  // Frequencies
    return A1 * sin(W1 * x) + A2 * sin(W2 * x) + A3 * sin(W3 * x) + A4 * sin(W4 * x);
}

void Tema1::Init()
{
	SoundEngine->setSoundVolume(0.1f);
	SoundEngine->play2D("media/Tanki_Online-Soundtrack.mp3", true);
    glm::ivec2 resolution = window->GetResolution();
	printf("Resolution: %d %d\n", resolution.x, resolution.y);

    for (float x = 0; x <= resolution.x; x += 1) {
        heightMap.push_back(10 * TerrainFunction(x/2));
    }

	tank_x1 = 300.1f;
	tank_y1 = heightMap[tank_x1];
	cannon_angle1 = 0;
	tank1_life = 100;

	tank_x2 = 1100.1f;
	tank_y2 = heightMap[tank_x2];
	cannon_angle2 = 180;
	tank2_life = 100;

    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
	originalCameraPosition.x = 0;
	originalCameraPosition.y = 0;
	originalCameraPosition.z = 50;
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    glm::vec3 corner = glm::vec3(0, 0, 0);
    float squareSide = 100;

    // TODO(student): Compute coordinates of a square's center, and store
    // then in the `cx` and `cy` class variables (see the header). Use
    // `corner` and `squareSide`. These two class variables will be used
    // in the `Update()` function. Think about it, why do you need them?

    cx = corner.x + squareSide / 2;
    cy = corner.y + squareSide / 2;

    // Initialize tx and ty (the translation steps)
    translateX = 0;
    translateY = 0;

    // Initialize sx and sy (the scale factors)
    scaleX = 1;
    scaleY = 1;

    // Initialize angularStep
    angularStep = 0;

	Mesh* square1 = object2D::CreateSquare("square1", corner, 4, glm::vec3(1, 1, 1), true);
	AddMeshToList(square1);

    Mesh* square4 = object2D::CreateSquare("square4", corner, 1500, glm::vec3(0.1294, 0.2588, 0.6941), true);
    AddMeshToList(square4);

    Mesh* square5 = object2D::CreateSquare("square5", corner, squareSide, glm::vec3(0.4980, 0.6941, 0.1294), true);
    AddMeshToList(square5);

	Mesh* release_bit = object2D::CreateSquare("release_bit", corner, 6, glm::vec3(0, 0, 0), true);
	AddMeshToList(release_bit);

	// Create the meshes for the first tank
	Mesh* bottomTrapezoid_1 = object2D::CreateTrapezoid("bottomTrapezoid_1",
                                       corner, 50, 70, 10, glm::vec3(0.4588, 0.3961, 0.3058), true);
	AddMeshToList(bottomTrapezoid_1);

	Mesh* topTrapezoid_1 = object2D::CreateTrapezoid("topTrapezoid_1",
		                               corner, 80, 100, 22, glm::vec3(0.8039, 0.6823, 0.5333), true);
	AddMeshToList(topTrapezoid_1);

	Mesh* turret_1 = object2D::CreateDisk("turret_1", corner, 15, glm::vec3(0.8039, 0.6823, 0.5333));
	AddMeshToList(turret_1);

	Mesh* cannon_1 = object2D::CreateRectangle("cannon_1", corner, 40, 4, glm::vec3(0, 0, 0), true);
	AddMeshToList(cannon_1);

	Mesh* explosion_bits_1 = object2D::CreateSquare("explosion_bits_1", corner, 15, glm::vec3(0.8039, 0.6823, 0.5333), true);
	AddMeshToList(explosion_bits_1);


	// Create the meshes for the second tank
	Mesh* bottomTrapezoid_2 = object2D::CreateTrapezoid("bottomTrapezoid_2",
		                               corner, 50, 70, 10, glm::vec3(0.1608, 0.1608, 0.1765), true);
	AddMeshToList(bottomTrapezoid_2);

	Mesh* topTrapezoid_2 = object2D::CreateTrapezoid("topTrapezoid_2",
		                               corner, 80, 100, 22, glm::vec3(0.3059, 0.3882, 0.2275), true);
	AddMeshToList(topTrapezoid_2);

	Mesh* turret_2 = object2D::CreateDisk("turret_2", corner, 15, glm::vec3(0.3059, 0.3882, 0.2275));
	AddMeshToList(turret_2);

	Mesh* cannon_2 = object2D::CreateRectangle("cannon_2", corner, 40, 4, glm::vec3(0, 0, 0), true);
	AddMeshToList(cannon_2);

	Mesh* explosion_bits_2 = object2D::CreateSquare("explosion_bits_2", corner, 15, glm::vec3(0.3059, 0.3882, 0.2275), true);
	AddMeshToList(explosion_bits_2);

	Mesh* projectile = object2D::CreateDisk("projectile", corner, 5, glm::vec3(0, 0, 0));
	AddMeshToList(projectile);
}


void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

float Tema1::GetHeight(float x) {
	float t1 = (x - floor(x)) / (ceil(x) - floor(x));
	float y = 300 + heightMap[floor(x)] + t1 * (heightMap[ceil(x)] - heightMap[floor(x)]);
	return y;
}

void Tema1::DrawTrajectoryLine(float x, float y, float cannon_angle, float angle, float deltaTime) {
	float density = 40.0f;

	float initialVelocityX = initialVelocity * cos(cannon_angle);
	float initialVelocityY = initialVelocity * sin(cannon_angle);

	std::vector<glm::vec2> trajectoryPoints;

	while (density > 0) {
		x += initialVelocityX * deltaTime;
		y += initialVelocityY * deltaTime;

		initialVelocityY -= gravity * deltaTime;
		
		if (x < 1 || x > 1280 - 1 || y < 1 || y > 720 - 1 || y < GetHeight(x)) {
			break;
		}

		trajectoryPoints.push_back(glm::vec2(x, y));

		density -= deltaTime;
	}

	// Render the line using squares
	for (size_t i = 1; i < trajectoryPoints.size(); ++i) {
		glm::vec2 point = trajectoryPoints[i];

		glm::mat3 modelMatrix = glm::mat3(1);
		modelMatrix *= transform2D::Translate(point.x, point.y);

		RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);
	}
}

void Tema1::LaunchProjectile(float x, float y, float cannon_angle, float angle) {
	Projectile newProjectile;
	newProjectile.position = glm::vec2(x, y);
	newProjectile.velocity = glm::vec2(cos(cannon_angle), sin(cannon_angle)) * initialVelocity;
	newProjectile.angle = angle;
	newProjectile.isActive = true;
	newProjectile.shell_drop_time = 0.7f;
	projectiles.push_back(newProjectile);

	// Animation for launching the projectile
	for (float i = -5; i < 5; i+=0.5) {
		Projectile_release_bit newProjectileReleaseBit;
		newProjectileReleaseBit.position = glm::vec2(x, y);
		newProjectileReleaseBit.velocity = glm::vec2(cos(cannon_angle + glm::radians(i * 10)), sin(cannon_angle + glm::radians(i * 10))) * 100.0f;
		newProjectileReleaseBit.angle = angle + glm::radians(i * 10);
		newProjectileReleaseBit.scale = 1;
		Projectile_release_bits.push_back(newProjectileReleaseBit);
	}
}

void Tema1::DeformTerrain(float impactX, float impactY, float radius) {
	float radiusSquared = radius * radius;

	int startX = impactX - radius;
	int endX = impactX + radius;

	for (float x = startX; x < endX; x++) {
		if (x < 0 || x > 1280) {
			continue;
		}

		float distanceSquared = (x - impactX) * (x - impactX);

		if (distanceSquared <= radiusSquared) {
			float deviation = sqrt(radiusSquared - distanceSquared);

			float currentHeight = heightMap[x] + 300;
			float newHeight = impactY - deviation;
			
			if (currentHeight > newHeight) {
				heightMap[x] = newHeight - 300;
			}
		}
	}
}

void Tema1::UpdateProjectile(float deltaTime) {
	for (auto it = projectiles.begin(); it != projectiles.end();) {
		if (it->isActive) {
			it->position += it->velocity * deltaTime * 5.0f;

			it->velocity.y -= gravity * deltaTime * 5.0f;

			it->shell_drop_time -= deltaTime;

			if (it->shell_drop_time <= 0) {
				
				it->shell_drop_time = 5000;
			}

			if (it->position.x < 0) {
				SoundEngine->play2D("media/Ricochet.mp3", false);
				it->position.x = 0;
				it->velocity.x = -it->velocity.x;
			}

			if (it->position.x > 1280) {
				SoundEngine->play2D("media/Ricochet.mp3", false);
				it->position.x = 1280;
				it->velocity.x = -it->velocity.x;
			}

			if (it->position.y > 720) {
				SoundEngine->play2D("media/Ricochet.mp3", false);
				it->position.y = 720;
				it->velocity.y = -it->velocity.y;
			}

			if (it->position.y < 0) {
				it = projectiles.erase(it);
				continue;
			}

			if (distance(it->position, glm::vec2(tank_x1, GetHeight(tank_x1))) < 48 && tank1_life > 0) {
				tank1_life -= 10;
				// Tank explosion
				if (tank1_life <= 0) {
					explosion_tank1 = true;
					isCameraShaking = true;
					SoundEngine->play2D("media/tank_destroyed.wav", false);
				}
				else {
					SoundEngine->play2D("media/tank_hit.wav", false);
				}
				it = projectiles.erase(it);
				continue;
			}

			if (distance(it->position, glm::vec2(tank_x2, GetHeight(tank_x2))) < 48 && tank2_life > 0) {
				tank2_life -= 10;
				// Tank explosion
				if (tank2_life <= 0) {
					explosion_tank2 = true;
					isCameraShaking = true;
					SoundEngine->play2D("media/tank_destroyed.wav", false);
				}
				else {
					SoundEngine->play2D("media/tank_hit.wav", false);
				}
				it = projectiles.erase(it);
				continue;
			}
			
			float terrainHeight = GetHeight(it->position.x);
			if (terrainHeight > it->position.y) {
				SoundEngine->play2D("media/terrain_hit.wav", false);
				DeformTerrain(it->position.x, it->position.y, 50);
				it = projectiles.erase(it);
				continue;
			}
		}
		++it;
	}
}

void Tema1::DrawLifeBar(float x, float y, float angle, float tank_life) {
	// Draw life bar border
	// Right
	modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(x + 50, y + 60);
	modelMatrix *= transform2D::Translate(-50, -60);
	modelMatrix *= transform2D::Rotate(angle);
	modelMatrix *= transform2D::Translate(50, 60);
	modelMatrix *= transform2D::Scale(0.5, 4);
	RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);

	// Bottom
	modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(x - 50, y + 60);
	modelMatrix *= transform2D::Translate(50, -60);
	modelMatrix *= transform2D::Rotate(angle);
	modelMatrix *= transform2D::Translate(-50, 60);
	modelMatrix *= transform2D::Scale(25, 0.5);
	RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);

	// Left
	modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(x - 50, y + 60);
	modelMatrix *= transform2D::Translate(50, -60);
	modelMatrix *= transform2D::Rotate(angle);
	modelMatrix *= transform2D::Translate(-50, 60);
	modelMatrix *= transform2D::Scale(0.5, 4);
	RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);

	// Top
	modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(x - 50, y + 75);
	modelMatrix *= transform2D::Translate(50, -75);
	modelMatrix *= transform2D::Rotate(angle);
	modelMatrix *= transform2D::Translate(-50, 75);
	modelMatrix *= transform2D::Scale(25, 0.5);
	RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);

	// Draw life bar
	modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(x - 50, y + 60);
	modelMatrix *= transform2D::Translate(50, -60);
	modelMatrix *= transform2D::Rotate(angle);
	modelMatrix *= transform2D::Translate(-50, 60);
	modelMatrix *= transform2D::Scale(tank_life / 4, 4);
	RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);
}

void Tema1::RenderTank_1(float deltaTime) {
	float x = tank_x1;
	float y = GetHeight(tank_x1);

	float yV = GetHeight(tank_x1 + 1) - GetHeight(tank_x1);
	float xV = 1;
	float angle;
	if (y < 0) {
		y = 0.001f;
		angle = 0;
	}
	else {
		angle = atan2(yV, xV);
	}

	// Draw bottom trapezoid
	glm::mat3 modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(x - 35, y - 10);
	modelMatrix *= transform2D::Translate(35, 10);
    modelMatrix *= transform2D::Rotate(angle);
	modelMatrix *= transform2D::Rotate(glm::radians(180.0f));
	modelMatrix *= transform2D::Translate(-35, -10);

	RenderMesh2D(meshes["bottomTrapezoid_1"], shaders["VertexColor"], modelMatrix);

	// Draw top trapezoid
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(x - 50, y + 10);
    modelMatrix *= transform2D::Translate(50, -10);
    modelMatrix *= transform2D::Rotate(angle);
    modelMatrix *= transform2D::Translate(-50, 10);
	RenderMesh2D(meshes["topTrapezoid_1"], shaders["VertexColor"], modelMatrix);

    // Draw turret (arc)
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(x, y + 32);
    modelMatrix *= transform2D::Translate(0, -32);
    modelMatrix *= transform2D::Rotate(angle);
    modelMatrix *= transform2D::Translate(0, 32);
	RenderMesh2D(meshes["turret_1"], shaders["VertexColor"], modelMatrix);

    // Draw cannon (rectangle)
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(x, y + 38);
    modelMatrix *= transform2D::Translate(0, -38);
    modelMatrix *= transform2D::Rotate(angle);
    modelMatrix *= transform2D::Translate(0, 38);
	glm::vec3 cannon_pos = modelMatrix * glm::vec3(0, 0, 1);
    modelMatrix *= transform2D::Translate(0, 2);
	modelMatrix *= transform2D::Rotate(glm::radians(cannon_angle1) - angle);
    modelMatrix *= transform2D::Translate(0, -2);
    RenderMesh2D(meshes["cannon_1"], shaders["VertexColor"], modelMatrix);

	
	// Compute the position of the cannon's tip
	float tip_pos_x = cos(glm::radians(cannon_angle1)) * 40 + cannon_pos.x;
	float tip_pos_y = sin(glm::radians(cannon_angle1)) * 40 + cannon_pos.y;

	// Launch projectile
	if (isSpacePressed) {
		LaunchProjectile(tip_pos_x, tip_pos_y, glm::radians(cannon_angle1), angle);
		isSpacePressed = false;
	}

	UpdateProjectile(deltaTime);
	UpdateReleaseProjectilesAnimation(deltaTime);

	for (Projectile& projectile : projectiles) {
		if (projectile.isActive) {
			glm::mat3 modelMatrix = glm::mat3(1);
			modelMatrix *= transform2D::Translate(projectile.position.x, projectile.position.y);
			RenderMesh2D(meshes["projectile"], shaders["VertexColor"], modelMatrix);
		}
	}

	// Draw trajectory line
	DrawTrajectoryLine(tip_pos_x, tip_pos_y, glm::radians(cannon_angle1), angle, deltaTime);

	// Draw life bar
	DrawLifeBar(x, y, angle, tank1_life);
}

void Tema1::RenderTank_2(float deltaTime) {
	float x = tank_x2;
	float y = GetHeight(tank_x2);

	float yV = GetHeight(x + 1) - GetHeight(x);
	float xV = 1;
	float angle;
	if (y < 0) {
		y = 0.001f;
		angle = 0;
	}
	else {
		angle = atan2(yV, xV);
	}

	// Draw bottom trapezoid
	glm::mat3 modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(x - 35, y - 10);
	modelMatrix *= transform2D::Translate(35, 10);
	modelMatrix *= transform2D::Rotate(angle);
	modelMatrix *= transform2D::Rotate(glm::radians(180.0f));
	modelMatrix *= transform2D::Translate(-35, -10);

	RenderMesh2D(meshes["bottomTrapezoid_2"], shaders["VertexColor"], modelMatrix);

	// Draw top trapezoid
	modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(x - 50, y + 10);
	modelMatrix *= transform2D::Translate(50, -10);
	modelMatrix *= transform2D::Rotate(angle);
	modelMatrix *= transform2D::Translate(-50, 10);
	RenderMesh2D(meshes["topTrapezoid_2"], shaders["VertexColor"], modelMatrix);

	// Draw turret (arc)
	modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(x, y + 32);
	modelMatrix *= transform2D::Translate(0, -32);
	modelMatrix *= transform2D::Rotate(angle);
	modelMatrix *= transform2D::Translate(0, 32);
	RenderMesh2D(meshes["turret_2"], shaders["VertexColor"], modelMatrix);

	// Draw cannon (rectangle)
	modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(x, y + 38);
	modelMatrix *= transform2D::Translate(0, -38);
	modelMatrix *= transform2D::Rotate(angle);
	modelMatrix *= transform2D::Translate(0, 38);
	glm::vec3 cannon_pos = modelMatrix * glm::vec3(0, 0, 1);
	modelMatrix *= transform2D::Translate(0, 2);
	modelMatrix *= transform2D::Rotate(glm::radians(cannon_angle2) - angle);
	modelMatrix *= transform2D::Translate(0, -2);
	RenderMesh2D(meshes["cannon_2"], shaders["VertexColor"], modelMatrix);

	float tip_pos_x = cos(glm::radians(cannon_angle2)) * 40 + cannon_pos.x;
	float tip_pos_y = sin(glm::radians(cannon_angle2)) * 40 + cannon_pos.y;

	if (isEnterPressed) {
		LaunchProjectile(tip_pos_x, tip_pos_y, glm::radians(cannon_angle2), angle);
		isEnterPressed = false;
	}

	UpdateProjectile(deltaTime);
	UpdateReleaseProjectilesAnimation(deltaTime);

	for (Projectile& projectile : projectiles) {
		if (projectile.isActive) {
			glm::mat3 modelMatrix = glm::mat3(1);
			modelMatrix *= transform2D::Translate(projectile.position.x, projectile.position.y);
			RenderMesh2D(meshes["projectile"], shaders["VertexColor"], modelMatrix);
		}
	}

	DrawTrajectoryLine(tip_pos_x, tip_pos_y, glm::radians(cannon_angle2), angle, deltaTime);

	// Draw life bar
	DrawLifeBar(x, y, angle, tank2_life);
}

void Tema1::LaunchExplosionBits(float x, float y, float tank) {
	for (float i = 0; i < 360; ++i) {
		Explosion_bit newExplosionBit{};
		newExplosionBit.position = glm::vec2(x, y);
		newExplosionBit.velocity = glm::vec2(cos(glm::radians(i)), sin(glm::radians(i))) * 100.0f;
		newExplosionBit.angle = glm::radians(i);
		newExplosionBit.scale = 1;
		newExplosionBit.tank = tank;
		Explosion_bits.push_back(newExplosionBit);
	}
}

void Tema1::UpdateExplosionBits(float deltaTime, float tank) {
	for (auto it = Explosion_bits.begin(); it != Explosion_bits.end();) {
		if (it->scale > 0) {
			it->position += it->velocity * deltaTime * 2.0f;

			it->velocity.y -= gravity * deltaTime * 2.0f;

			it->scale -= 0.5f * deltaTime * 2.0f;

			if (it->scale <= 0 || it->position.y < 0 || it->position.y > 720 || it->position.x < 0 || it->position.x > 1280) {
				it = Explosion_bits.erase(it);
				continue;
			}
		}
		++it;
	}

	for (Explosion_bit& explosionBit : Explosion_bits) {
		if (explosionBit.scale > 0) {
			glm::mat3 modelMatrix = glm::mat3(1);
			modelMatrix *= transform2D::Translate(explosionBit.position.x + rand() % 20, explosionBit.position.y + rand() % 20);
			modelMatrix *= transform2D::Rotate(explosionBit.angle);
			modelMatrix *= transform2D::Scale(explosionBit.scale, explosionBit.scale);
			if (explosionBit.tank == 1) {
				RenderMesh2D(meshes["explosion_bits_1"], shaders["VertexColor"], modelMatrix);
			}
			else {
				RenderMesh2D(meshes["explosion_bits_2"], shaders["VertexColor"], modelMatrix);
			}
		}
	}
}

void Tema1::UpdateReleaseProjectilesAnimation(float deltaTime) {
	for (auto it = Projectile_release_bits.begin(); it != Projectile_release_bits.end();) {
		if (it->scale > 0) {
			it->position += it->velocity * deltaTime * 2.0f;

			it->velocity.y -= gravity * deltaTime * 2.0f;

			it->scale -= 0.5f * deltaTime * 4.f;

			if (it->position.x < 0) {
				it->position.x = 0;
				it->velocity.x = -it->velocity.x;
			}

			if (it->position.x > 1280) {
				it->position.x = 1280;
				it->velocity.x = -it->velocity.x;
			}

			if (it->scale <= 0) {
				it = Projectile_release_bits.erase(it);
				continue;
			}
		}
		++it;
	}

	for (Projectile_release_bit& projectileReleaseBit : Projectile_release_bits) {
		if (projectileReleaseBit.scale > 0) {
			glm::mat3 modelMatrix = glm::mat3(1);
			modelMatrix *= transform2D::Translate(projectileReleaseBit.position.x + rand() % 2, projectileReleaseBit.position.y + rand() % 2);
			modelMatrix *= transform2D::Rotate(projectileReleaseBit.angle);
			modelMatrix *= transform2D::Scale(projectileReleaseBit.scale, projectileReleaseBit.scale);
			RenderMesh2D(meshes["release_bit"], shaders["VertexColor"], modelMatrix);
		}
	}
}

void Tema1::UpdateCameraShaking(float deltaTimeSeconds) {
	float offsetX = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX) - 0.5f) * shakeIntensity;
	float offsetY = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX) - 0.5f) * shakeIntensity;

	auto camera = GetSceneCamera();
	camera->SetPosition(glm::vec3(originalCameraPosition.x + offsetX, originalCameraPosition.y + offsetY, 50));
	camera->Update();

	shakeTime += deltaTimeSeconds;

	if (shakeTime >= shakeDuration) {
		isCameraShaking = false;
		shakeTime = 0.0f;

		camera->SetPosition(originalCameraPosition);
		camera->Update();
	}
}

void Tema1::RenderTerrain() {
	for (float i = 0; i < heightMap.size() - 1; ++i) {
		float x1 = i;
		float y1 = heightMap[i];
		float x2 = (i + 1);
		float y2 = heightMap[i + 1];

		glm::mat3 modelMatrix = glm::mat3(1);
		modelMatrix *= transform2D::Translate(x1, y1 + 300);
		modelMatrix *= transform2D::Shear(0, (y2 - y1) / (x2 - x1));
		modelMatrix *= transform2D::Scale((x2 - x1) / 100, -10);

		RenderMesh2D(meshes["square5"], shaders["VertexColor"], modelMatrix);
	}
}

void Tema1::UpdateTerrainDeformation() {
	std::vector<float> newHeightMap(heightMap.size());

	for (size_t i = 0; i < heightMap.size(); ++i) {
		float sum = 0.0f;
		int count = 0;

		for (float j = -3; j <= 3; ++j) {
			float neighborIndex = i + j;

			if (neighborIndex >= 0 && neighborIndex < 1280) {
				sum += heightMap[neighborIndex];
				count++;
			}
		}

		float avgH = sum / count;

		if (abs(heightMap[i] - avgH) > 0.25) {
			newHeightMap[i] = heightMap[i] / 2 + avgH / 2;
		}
		else {
			newHeightMap[i] = heightMap[i];
		}
	}
	heightMap = newHeightMap;
}

void Tema1::Update(float deltaTimeSeconds)
{
	if (isCameraShaking) {
		UpdateCameraShaking(deltaTimeSeconds);
	}

	UpdateTerrainDeformation();

	RenderTerrain();

	if (tank1_life > 0) {
		RenderTank_1(deltaTimeSeconds);
	}

	if (explosion_tank1) {
		LaunchExplosionBits(tank_x1, GetHeight(tank_x1), 1);
		explosion_tank1 = false;
	}
	UpdateExplosionBits(deltaTimeSeconds, 1);

	if (tank2_life > 0) {
		RenderTank_2(deltaTimeSeconds);
	}

	if (explosion_tank2) {
		LaunchExplosionBits(tank_x2, GetHeight(tank_x2), 2);
		explosion_tank2 = false;
	}
	UpdateExplosionBits(deltaTimeSeconds, 2);

	// Background
	{
		modelMatrix = glm::mat3(1);
		RenderMesh2D(meshes["square4"], shaders["VertexColor"], modelMatrix);
	}
}


void Tema1::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
	// Inputs for the first tank
	if (window->KeyHold(GLFW_KEY_A)) {
		if (tank_x1 > 15) {
			tank_x1 -= 90 * deltaTime;
		}
	}
	if (window->KeyHold(GLFW_KEY_D)) {
		if (tank_x1 < 1280 - 15) {
			tank_x1 += 90 * deltaTime;
		}
	}

	if (tank1_life > 0) {
		if (window->KeyHold(GLFW_KEY_A) && window->KeyHold(GLFW_KEY_D)) {
			if (is_moving_tank1 == 1) {
				tank1_moving->stop();
				is_moving_tank1 = -1;
			}
		}
		else if (window->KeyHold(GLFW_KEY_A) || window->KeyHold(GLFW_KEY_D)) {
			if (is_moving_tank1 == -1) {
				tank1_moving = SoundEngine->play2D("media/tank_moving.mp3", true, false, true);
				is_moving_tank1 = 1;
			}
		}
	}

	if (window->KeyHold(GLFW_KEY_W)) {
		//if (cannon_angle1 < 180) {
			cannon_angle1 += 50 * deltaTime;
		//}
	}
	if (window->KeyHold(GLFW_KEY_S)) {
		//if (cannon_angle1 > 0) {
			cannon_angle1 -= 50 * deltaTime;
		//}
	}

	// Inputs for the second tank
	if (window->KeyHold(GLFW_KEY_LEFT)) {
		if (tank_x2 > 15) {
			tank_x2 -= 90 * deltaTime;
		}
	}
	if (window->KeyHold(GLFW_KEY_RIGHT)) {
		if (tank_x2 < 1280 - 15) {
			tank_x2 += 90 * deltaTime;
		}
	}
	if (tank2_life > 0) {
		if (window->KeyHold(GLFW_KEY_LEFT) && window->KeyHold(GLFW_KEY_RIGHT)) {
			if (is_moving_tank2 == 1) {
				tank2_moving->stop();
				is_moving_tank2 = -1;
			}
		}
		else if (window->KeyHold(GLFW_KEY_LEFT) || window->KeyHold(GLFW_KEY_RIGHT)) {
			if (is_moving_tank2 == -1) {
				tank2_moving = SoundEngine->play2D("media/tank_moving.mp3", true, false, true);
				is_moving_tank2 = 1;
			}
		}
	}

	if (window->KeyHold(GLFW_KEY_UP)) {
		//if (cannon_angle2 < 180) {
			cannon_angle2 += 50 * deltaTime;
		//}
	}
	if (window->KeyHold(GLFW_KEY_DOWN)) {
		//if (cannon_angle2 > 0) {
			cannon_angle2 -= 50 * deltaTime;
		//}
	}
}


void Tema1::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_SPACE && tank1_life > 0) {
		isSpacePressed = true;
		SoundEngine->play2D("media/cannon_attack.wav", false);
		SoundEngine->play2D("media/cannon_shell_drop.mp3", false);
	}

	if (key == GLFW_KEY_ENTER && tank2_life > 0) {
		isEnterPressed = true;
		SoundEngine->play2D("media/cannon_attack.wav", false);
		SoundEngine->play2D("media/cannon_shell_drop.mp3", false);
	}
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
	if (key == GLFW_KEY_D || key == GLFW_KEY_A) {
		if (is_moving_tank1 == 1) {
			tank1_moving->stop();
			is_moving_tank1 = -1;
		}
	}

	if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_LEFT) {
		if (is_moving_tank2 == 1) {
			tank2_moving->stop();
			is_moving_tank2 = -1;
		}
	}
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}
