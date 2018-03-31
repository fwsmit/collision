#include "constants.h"

namespace op{
	using namespace constants;
	sf::Vector2f toSf(arma::fvec2 vec) { 
		sf::Vector2f result;
		result.x = toSf(vec[0]);
		result.y = toSf(vec[1]);
		return result;
	}

	float toSf(float val){
		return val * scale;
	}

	arma::fvec2 toArma(sf::Vector2f vec){
		return arma::fvec2{toArma(vec.x), toArma(vec.y)};
	}

	arma::fvec2 toArma(sf::Vector2u vec){
		return arma::fvec2{toArma(vec.x), toArma(vec.y)};
	}

	float toArma(float val){
		return val/scale;
	}
	
}