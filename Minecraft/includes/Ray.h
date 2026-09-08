#pragma once

#include <glm/glm.hpp>



class Ray
{

	public:
		Ray(const glm::vec3& start);

		// unsure of return type
		// may return block or boolean

		bool cast(const glm::vec3& direction , float max_dist);


	private:

		glm::vec3 m_start_position;


};
