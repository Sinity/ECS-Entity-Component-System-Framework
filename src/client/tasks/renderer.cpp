#include "renderer.h"
#include "components/positionComponent.h"
#include "components/renderingComponent.h"

void RenderingTask::update() {
    window.clear();

    std::vector<PositionComponent*> positions;
    std::vector<RenderingComponent*> graphics;
    engine.components.intersection(graphics, positions);
	
    int maxPlane = std::numeric_limits<int>::min();
    int minPlane = std::numeric_limits<int>::max();
    for(unsigned int i = 0; i < graphics.size(); i++) {
        maxPlane = (maxPlane < graphics[i]->plane ? graphics[i]->plane : maxPlane);
        minPlane = (minPlane > graphics[i]->plane ? graphics[i]->plane : minPlane);
    }

    for(int currentPlane = maxPlane; currentPlane >= minPlane; currentPlane--) {
        for(size_t i = 0; i < graphics.size(); i++) {
            if(graphics[i]->plane == currentPlane) {
                sf::Transform transform;
                transform.translate(positions[i]->x, positions[i]->y);
                sf::RenderStates states;
                states.transform = transform;

                for(auto drawableElement : graphics[i]->drawablesList) {
                    window.draw(*drawableElement, states);
                }
            }
        }
    }

    window.display();                 
}

RenderingTask::RenderingTask(Engine& engine, sf::RenderWindow& window) : Task(engine), window(window) {
	std::string winTitle = engine.config.get("tasks.renderer.windowTitle");
	int resX = engine.config.get("tasks.renderer.resolution.x", 1600);
	int resY = engine.config.get("tasks.renderer.resolution.y", 900);

	bool fullscreen = engine.config.get("tasks.renderer.fullscreen", std::string("false")) == "true";
	if (fullscreen) {
		window.create(sf::VideoMode::getFullscreenModes()[0], winTitle, sf::Style::Fullscreen);
	}
	else {
		window.create(sf::VideoMode(resX, resY, 32), winTitle);
	}

	float left = engine.config.get("tasks.renderer.initialView.left", 0.0f);
	float top = engine.config.get("tasks.renderer.initialView.top", 0.0f);
	float width = engine.config.get("tasks.renderer.initialView.width", (float)resX);
	float height = engine.config.get("tasks.renderer.initialView.height", (float)resY);
	window.setView(sf::View({ left, top, width, height }));
}

