#include <SFML/Graphics.hpp>
#include "Physics.h"
#include "constants.h"
#include <iostream>

int main(){
	int caseN = 1;
	sf::RenderWindow window(sf::VideoMode(1000, 800), "Collision");
	//window.setFramerateLimit(2);
	Physics physics(window);
	switch(caseN){
		case 0:// direct hit
			physics.addObject(arma::fvec2{100, 100}, arma::fvec2{20, 10});
			physics.addObject(arma::fvec2{160, 150}, arma::fvec2{0, 0});
			break;
		case 1: // single circle
			physics.addObject(arma::fvec2{200, 50}, arma::fvec2{-50, -100});
			break;
		case 2:// direct hit
			physics.addObject(arma::fvec2{200, 300}, arma::fvec2{-5, -100});
			physics.addObject(arma::fvec2{160, 150}, arma::fvec2{0, 0});
			break;
		case 3:// test bounds
			physics.addObject(arma::fvec2{100, 300}, arma::fvec2{-50, 100});
		case 4: // test lines
			physics.addObject(arma::fvec2{300, 200}, 0.1*arma::fvec2{400, -500});
			break;
		case 5: // two moving objects
			physics.addObject(arma::fvec2{200, 300}, arma::fvec2{-5, -100});
			physics.addObject(arma::fvec2{160, 150}, arma::fvec2{50, 20});
			break;
		case 6: // ultimate stress test (with big circles)
			physics.addObject(arma::fvec2{250, 50}, arma::fvec2{-50, 150});
			physics.addObject(arma::fvec2{30, 200}, arma::fvec2{50, 300});
			physics.addObject(arma::fvec2{30, 200}, arma::fvec2{400, -500});
			physics.addObject(arma::fvec2{300, 200}, arma::fvec2{10, -500});
			physics.addObject(arma::fvec2{20, 320}, arma::fvec2{10, -10});
			physics.addObject(arma::fvec2{50, 320}, arma::fvec2{0, 0});
			physics.addObject(arma::fvec2{50, 320}, arma::fvec2{100, 0});
			physics.addObject(arma::fvec2{50, 320}, arma::fvec2{-100, 0});
			physics.addObject(arma::fvec2{50, 320}, arma::fvec2{-200, 0});
			physics.addObject(arma::fvec2{50, 320}, arma::fvec2{-500, 0});
			break;
		case 7: // four simultaneous collisions
			{
				arma::fvec2 bounds = op::toArma(window.getSize());
				arma::fvec2 middle_pos = bounds/2;
				physics.addObject(arma::fvec2{middle_pos[0]-100, middle_pos[1]}, arma::fvec2{100, 0});
				physics.addObject(arma::fvec2{middle_pos[0]+100, middle_pos[1]}, arma::fvec2{-100, 0});
				physics.addObject(arma::fvec2{middle_pos[0], middle_pos[1]-100}, arma::fvec2{0, 100});
				physics.addObject(arma::fvec2{middle_pos[0], middle_pos[1]+100}, arma::fvec2{0, -100});
				physics.addObject(middle_pos, arma::fvec2{0, 0});
			}
			break;
		case 8: // simultaneous bound collision
			physics.addObject(arma::fvec2{100, 100}, arma::fvec2{-100, -100});
			break;
		case 9: // different radius
			{
				Circle c1, c2;
				c1.setRadius(60);
				c1.setPos(arma::fvec2{100, 200});
				c1.setMass(10);
				c2.setPos(arma::fvec2{300, 100});
				c1.setVel(arma::fvec2{100, 20});
				c2.setVel(arma::fvec2{-10, 100});
				physics.addObject(c1);
				physics.addObject(c2);
			}
			break;
		case 10: // many small circles
			{
				Circle c;
				c.setRadius(10);
				for(int i = 0; i < 5; i ++){
					for(int j = 0; j < 5; j++){
						c.setPos(arma::fvec2{30 + i *30.f, 30+ j*30.f});
						c.setVel(arma::fvec2{arma::randu<float>()*50, arma::randu<float>() * 50});
						physics.addObject(c);
					}
				}
			}
			break;

	}
	sf::Clock timer;
	float deltaTime;
	while(window.isOpen()){
		deltaTime = timer.restart().asSeconds();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		physics.draw(deltaTime);
		window.display();
	}
}
