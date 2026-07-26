#include "Camera.h"
#include <iostream>
#include "Phys.h"



                    //amplifying gravity * 2
const float GRAVITY = -9.81f * 2;
const float TERMINAL_VELOCITY = -50.0f;

//general up direction for projection stuff
constexpr glm::vec3 g_Up(0.f, 1.f, 0.f);


Camera::Camera(glm::vec3 position, int width, int height, float fov, float nearPlane, float farPlane)
    : m_Width(width),
    m_Height(height),
    m_Position(position),
    m_MVP(glm::mat4(1.0f)),
    m_Orientation(0.0f, 0.0f, -1.0f),
    m_Up(glm::vec3(0.0f, 1.0f, 0.0f)),
    m_Right(glm::vec3(1.0f, 0.0f, 0.0f)),
    m_Yaw(-90.f),
    m_Pitch(0.f),
    m_Zoom(75.0f),
	m_Speed(5.0f),
	m_Velocity(5.0f),
    m_Sensitivity(0.5f)
{                                       // can do fov or zoom for this
    m_ProjectionMat = glm::perspective(glm::radians(m_Zoom), float(m_Width) / float(m_Height), nearPlane, farPlane);
    updateCameraVectors();
}

glm::mat4 Camera::CalcViewMatrix() const
{
    return glm::lookAt(m_Position, m_Position + m_Orientation, g_Up);
}
void Camera::DispatchKeyboardEvent(MovementDir dir, float deltaTime)
{

    float velocity = m_Speed * deltaTime;

    glm::vec3 backward = glm::cross(m_Right, g_Up);

    switch (dir) {
        case FORWARD:      m_Position += -backward * velocity; break;
        case BACK:         m_Position +=  backward * velocity; break;
        case LEFT:         m_Position +=       -m_Right * velocity; break;
        case RIGHT:        m_Position +=        m_Right * velocity; break;
        case UP:           m_Position += g_Up * velocity; m_Velocity = 5.0f; break;
        case DOWN:         m_Position +=          -g_Up * velocity; break;
        case NONE:                                                  break;

        default:                                                    break;

    }


}

// for gravity sim
void Camera::OnUpdate(float deltaTime)
{
	float chunkX = static_cast<float>(std::floor(m_Position.x));
	float chunkY = static_cast<float>(std::floor(m_Position.y));
	float chunkZ = static_cast<float>(std::floor(m_Position.z));


   m_Velocity += GRAVITY * deltaTime;
   if (m_Velocity < TERMINAL_VELOCITY) m_Velocity = TERMINAL_VELOCITY;
   m_Position.y += m_Velocity * deltaTime;

    // blocks will now be all the blocks , x,y,z , "near" the player with some dx, dy, and dz

    std::vector<glm::vec3> blocks = BroadPhase(glm::floor(m_Position - 2.0f), glm::ceil(m_Position + 4.0f));

    glm::vec3 minBoxPos{ m_Position.x - PLAYER_WIDTH / 2.f, m_Position.y - PLAYER_HEIGHT, m_Position.z - PLAYER_WIDTH / 2.0f };
    AABB box(minBoxPos, PLAYER_WIDTH, PLAYER_HEIGHT);
    std::vector<ColliderResult> hitBlocks = NarrowPhase(blocks, m_Position, box);

    glm::vec3 totalCorrectionY(0.0f);
    glm::vec3 totalCorrectionXZ(0.0f);

    for (const auto& it : hitBlocks) {
        if (!box.collides(it.contactPoint))
            continue;

        if (it.overlapY != 0.0f) {
            totalCorrectionY = it.normal * it.overlapY;
            m_Velocity = 0.0f;
        }

        if (it.overlapXZ != 0.0f)
            totalCorrectionXZ = it.normal * it.overlapXZ;


    }

    // Apply total corrections to position
    m_Position += totalCorrectionY * deltaTime;
    //m_Position += totalCorrectionXZ * deltaTime;


}

// @param xrot -  x offset/rot from different between cur mouse x and prev mouse x
// @param yrot - y offset/rot from different between cur mouse y and prev mouse y
void Camera::DispatchMouseMoveEvent(float xrot, float yrot)
{

    //amplify rot based on sens
    xrot *= m_Sensitivity;
    yrot *= m_Sensitivity;

    // yaw is rot around y axis, so its x offset
    m_Yaw += xrot;
    m_Pitch += yrot;


    if (!(m_Pitch >= -85.0f && m_Pitch <= 85.0f)) {
        m_Pitch -= yrot;
    }


   // std::cout << "Yaw: " << m_Yaw << "\tPitch: " << m_Pitch << "\n";


    updateCameraVectors();
}

void Camera::DispatchMouseScrollEvent(float scroll)
{
    m_Speed += scroll;

    // if we go too low
    m_Speed = (m_Speed < 0) ? 0 : m_Speed;
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors()
{
    // so we can have a very high or very low yaw, wont keep cumulating, more controlled
    glm::clamp(m_Yaw, -180.f, 180.f);

    // multiply by cos m_Pitch for dampen - when looking very high or very low, x rot slower
    m_Orientation.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

    // simply sin for y component
    m_Orientation.y = sin(glm::radians(m_Pitch));

    m_Orientation.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

    m_Orientation = glm::normalize(m_Orientation);

    // ori changed to we need to recalc up and right
    m_Right = glm::normalize(glm::cross(m_Orientation, g_Up));
    m_Up = glm::normalize(glm::cross(m_Right, m_Orientation));
}
