#include "Particle.h"

class Particle {
public:
	glm::vec2 position;
	glm::vec2 previous_position;
	glm::vec2 acceleration;

	Particle(float x, float y) : position(x, y), previous_position(x, y), acceleration(0, 0) {}

	void applyForce(const glm::vec2& force)
	{
		acceleration += force;
	}

	void update(float timeStep)
	{
		glm::vec2 velocity = position - previous_position;
		previous_position = position;
		position += velocity + acceleration * timeStep * timeStep;
		acceleration = glm::vec2(0, 0);
	}

	void constrainToBounds(float width, float height)
	{
		if (position.x < 0) position.x = 0;
		if (position.x > width) position.x = width;
		if (position.y < 0) position.y = 0;
		if (position.y > height) position.y = height;
	}
};