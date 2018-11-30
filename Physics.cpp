// credits to http://ericleong.me/research/circle-circle/#static-circle-circle-collision-response
#include "Physics.h"
#include <cmath>
#include "constants.h"

using namespace arma;

Physics::Physics(sf::RenderWindow& _window) : window(_window){
	bounds = op::toArma(window.getSize());
	lines_array.setPrimitiveType(sf::PrimitiveType::Lines);

	// add the bounds
	addLine(fvec2{0, 0}, fvec2{bounds[0], 0});
	addLine(fvec2{0, 0}, fvec2{0, bounds[1]});
	addLine(fvec2{bounds[0], 0}, fvec2{bounds[0], bounds[1]});
	addLine(fvec2{0, bounds[1]}, fvec2{bounds[0], bounds[1]});
}

void Physics::addObject(fvec2 pos, fvec2 vel){
	Circle object;
	object.setPos(pos);
	object.setVel(vel);
	objects.push_back(object);
}

void Physics::addObject(fvec2 pos, fvec2 vel, float radius){
	Circle object;
	object.setPos(pos);
	object.setVel(vel);
	object.setRadius(radius);
	objects.push_back(object);
}

void Physics::addObject(Circle object){
	objects.push_back(object);
}

void Physics::addLine(fvec2 begin, fvec2 end){
	lines.push_back(Line(begin, end));
}

void Physics::update(float deltaTime){
	static bool once = true;
	bool hasCollided = true;
	float timeLeft = deltaTime;
	int circleID = -1;
	int circle2ID = -1;
	int lineID = -1;
	bool circleCircle;
	int count = 0;
	while(hasCollided){
		float earliestHitTime = 100000000000;
		hasCollided = false;
		if(objects.size() > 1){
			for(int i = 0; i < objects.size() - 1; i++){
				for(int j = i+1; j < objects.size(); j++){
					Circle& u = objects[i];
					Circle&	v = objects[j]; // v is static in reference frame
					fvec2 u_vel = u.getVel() - v.getVel();
					Line u_movement(u.getPos(), u.getPos() + u_vel);
					fvec2 project_v = closestPointOnLine(u_movement, v.getPos());
					
					// a_sq + b_sq = r_sq
					float a_sq = arma::accu(arma::pow(v.getPos() - project_v, 2)); 
					float b_sq = pow(v.getRadius() + u.getRadius(), 2) - a_sq;
					float b = sqrt(b_sq);
					fvec2 u_vel_unit = arma::normalise(u_vel);
					fvec2 intersectionPoint = project_v - b*u_vel_unit;
					float intersectionDist = arma::norm(u.getPos() - intersectionPoint);
					float a_dist = arma::norm(u.getPos() - project_v);
					//op::drawPoint(intersectionPoint, window);
					//op::drawPoint(u.getPos(), window);
					//op::drawPoint(project_v, window);
					bool isHitting = intersectionDist < a_dist; // when false, u is moving away from v
					if(isHitting){
						float hitTime = arma::norm(intersectionPoint - u.getPos())/arma::norm(u_vel);
						if(hitTime  < timeLeft){
							if(hitTime < earliestHitTime){
								circleCircle = true;
								circleID = i;
								circle2ID = j;
								earliestHitTime = hitTime;
							}
						}
					}
				}
			}
		}
		int i = 0;
		for(auto& object : objects){
			int j = 0;
			for(auto line : lines){
				fvec2 direction = line.getDirection();
				fvec2 vel_paralel = op::getParalel(object.getVel(), direction);
				fvec2 vel_perpendicular = object.getVel() - vel_paralel;
				fvec2 p = object.getPos();
				fvec2 project_p = closestPointOnLine(line, p);
				float distance = arma::norm(p-project_p) - object.getRadius();
				float speed_perpendicular = arma::norm(vel_perpendicular);
				float hitTime = distance / speed_perpendicular;
				fvec2 intersectDir = project_p - p;
				float dir = arma::dot(arma::normalise(intersectDir), arma::normalise(object.getVel()));
				if(dir > 0){
					if(hitTime < timeLeft){
						if(hitTime < earliestHitTime){
							if(once){
								once = false;
							}
							circleCircle = false;
							lineID = j;
							circleID = i;
							earliestHitTime = hitTime;
						}
					}
				}
				j++;
			}
			i++;
		}
		if(earliestHitTime < 100000){ // there is a collision this frame
			hasCollided = true;
			if(circleCircle){
				auto& u = objects[circleID];
				auto& v = objects[circle2ID];
				travelAll(earliestHitTime);
				timeLeft -= earliestHitTime;
				fvec2 perpendicular = u.getPos() - v.getPos();
				fvec2 u_perp = op::getParalel(u.getVel(), perpendicular);
				fvec2 u_paralel = u.getVel() - u_perp;
				fvec2 v_perp = op::getParalel(v.getVel(), perpendicular);
				fvec2 v_paralel = v.getVel() - v_perp;
				fvec2 u_new_vel = (u_perp*(u.getMass() - v.getMass()) + 2*v.getMass()*v_perp)/(u.getMass() + v.getMass()) + u_paralel;
				fvec2 v_new_vel = (v_perp*(v.getMass() - u.getMass()) + 2*u.getMass()*u_perp)/(u.getMass() + v.getMass()) + v_paralel;

				u.setVel(u_new_vel);
				v.setVel(v_new_vel);
			}
			else{
				auto& object = objects[circleID];
				auto& line = lines[lineID];
				fvec2 direction = line.getDirection();
				fvec2 vel_paralel = op::getParalel(object.getVel(), direction);
				fvec2 vel_perpendicular = object.getVel() - vel_paralel;
				fvec2 p = object.getPos();
				fvec2 project_p = closestPointOnLine(line, p);
				float distance = arma::norm(p-project_p) - object.getRadius();
				float speed_perpendicular = arma::norm(vel_perpendicular);
				float hitTime = distance / speed_perpendicular;
				travelAll(earliestHitTime);
				timeLeft -= earliestHitTime;
				fvec2 newVel = vel_paralel - vel_perpendicular;
				object.setVel(newVel);
			}
		}
		count++;
	}

	float e_k = 0;
	for(auto object : objects){ // calculate total kinetic energy
		e_k += 0.5 * object.getMass() * pow(arma::norm(object.getVel()), 2);
	}
	// cout << "kinetic engergy: " << e_k << endl;

	travelAll(timeLeft);
}

void Physics::mouseDrag(float deltaTime){
	static arma::fvec2 startPos, currPos;
	static bool hasStarted = false;
	if(hasStarted){
		if(!sf::Mouse::isButtonPressed(sf::Mouse::Left)){
			hasStarted = false;
		}
		else{
			currPos = op::toArma( sf::Mouse::getPosition(window) );
			fvec2 deltaPos = currPos - startPos;
			cout << "deltaPos: " << ( currPos - startPos ) << endl;
			objects[0].applyForce(deltaPos, deltaTime);
		}
	}
	if(hasStarted == false && sf::Mouse::isButtonPressed(sf::Mouse::Left)){
		hasStarted = true;
		startPos = op::toArma( sf::Mouse::getPosition(window) );
	}
}

void Physics::draw(float deltaTime){
	mouseDrag(deltaTime);
	update(deltaTime);
	for(int i = 0; i < objects.size(); i++){
		sf::CircleShape circle;
		circle.setPosition(op::toSf(objects[i].getPos()));
		circle.setRadius(op::toSf(objects[i].getRadius()));
		circle.setOrigin(circle.getRadius(), circle.getRadius());
		circle.setFillColor(sf::Color(0, 0, 255, 120));
		window.draw(circle);
	}
	lines_array.clear();
	for(int i = 4; i < lines.size(); i++){
		lines_array.append(op::toSf(lines[i].getStart()));
		lines_array.append(op::toSf(lines[i].getEnd()));
	}
	window.draw(lines_array);
}

void Physics::travelAll(float time){
	for(auto& object : objects){
		object.travel(time);
	}
}

arma::fvec2 Physics::closestPointOnLine(Line line, arma::fvec2 p){
	// line start = A and line end = B
	fvec2 AP = p - line.getStart();
	fvec2 AB = line.getEnd() - line.getStart();

	float magnitudeAB = arma::accu(arma::pow(AB, 2));
	float dotProduct = arma::dot(AP, AB);
	float distance = dotProduct / magnitudeAB; // not an actual distance

	return line.getStart() + AB * distance;

    //float a = line.getA();
	//float b = line.getB();
	//float c = line.getC();
	//float h = b*p[1]-(b*b)/a * p[0];
	//float j = (c-h)/(pow(b, 2)/a + a);
	//float k = (-a/b)*j + c/b; 
	//return arma::fvec2{j,k};
}
