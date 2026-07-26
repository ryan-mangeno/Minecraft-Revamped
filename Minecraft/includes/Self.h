#pragma once

#include "PlayerObj.h"
#include <glm/glm.hpp>
#include "Camera.h"

class Self : public PlayerObj
{
	public:
		Self( const glm::vec3 pos );
		
		void OnUpdate(float deltaTime);

				

	private:
		
		// camera is singleton ... might change
		//Camera m_Camera;
};

