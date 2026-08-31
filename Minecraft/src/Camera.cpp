#include "Camera.h"
#include "Phys.h"
#include "Log.h"


// amplifying gravity * 2
const float GRAVITY = -9.81f * 2;
const float TERMINAL_VELOCITY = -50.0f;

// general up direction for projection stuff
constexpr glm::vec3 g_Up(0.f, 1.f, 0.f);


Camera::Camera(glm::vec3 position, int width, int height, float fov, float nearPlane, float farPlane)
    : m_Width(width),
    m_Height(height),
    m_Position(position),
    m_PositionUpdate(position),
    m_PrevPosition(position),
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
{
    // can do fov or zoom for this
    m_ProjectionMat = glm::perspective(glm::radians(m_Zoom), float(m_Width) / float(m_Height), nearPlane, farPlane);
    updateCameraVectors();
}

glm::mat4 Camera::CalcViewMatrix() const
{
    return glm::lookAt(m_Position, m_Position + m_Orientation, g_Up);
}
void Camera::DispatchKeyboardEvent(Direction dir, float deltaTime)
{

    float velocity = m_Speed * deltaTime;

    glm::vec3 backward = glm::cross(m_Right, g_Up);

    switch (dir) {
        case NORTH:      m_PositionUpdate += -backward * velocity; break;
        case SOUTH:         m_PositionUpdate +=  backward * velocity; break;
        case WEST:         m_PositionUpdate +=       -m_Right * velocity; break;
        case EAST:        m_PositionUpdate +=        m_Right * velocity; break;
        case UP:           m_PositionUpdate += g_Up * velocity; m_Velocity = 5.0f; break;
        case DOWN:         m_PositionUpdate +=          -g_Up * velocity; break;
        case NONE:                                                  break;

        default:                                                    break;
    }
}

// for gravity sim
void Camera::OnUpdate(float deltaTime)
{
    // resolve xz first at current height, then y after gravity
    // a pure side hit can't leak into a vertical push this way

    // narrowphase now does a real box vs box overlap test itself and
    // only returns actual collisions, so no gate check is needed here
    auto queryHits = [this]() -> std::vector<ColliderResult> {
        std::vector<glm::vec3> blocks = BroadPhase(glm::floor(m_PositionUpdate - 2.0f), glm::ceil(m_PositionUpdate + 4.0f));
        glm::vec3 minBoxPos{ m_PositionUpdate.x - PLAYER_WIDTH / 2.f, m_PositionUpdate.y - PLAYER_HEIGHT, m_PositionUpdate.z - PLAYER_WIDTH / 2.0f };
        AABB box(minBoxPos, PLAYER_WIDTH, PLAYER_HEIGHT);
        return NarrowPhase(blocks, m_PositionUpdate, box);
    };

    // horizontal pass
    {
        auto hitBlocks = queryHits();
        glm::vec3 totalCorrectionXZ(0.0f);
        for (const auto& it : hitBlocks) {
            // keep the biggest correction instead of overwriting
            if (it.overlapXZ != 0.0f) {
                glm::vec3 c = it.normal * it.overlapXZ;
                if (glm::length(c) > glm::length(totalCorrectionXZ))
                    totalCorrectionXZ = c;
            }
        }
        if (totalCorrectionXZ.x || totalCorrectionXZ.z) {
            MC_DEBUG("XZ Collision! x: %f, totalCorrectionXZ.z %f", totalCorrectionXZ.x, totalCorrectionXZ.z);
        }
        m_PositionUpdate += totalCorrectionXZ;
    }

    // vertical pass
    m_Velocity += GRAVITY * deltaTime;
    if (m_Velocity < TERMINAL_VELOCITY) m_Velocity = TERMINAL_VELOCITY;
    m_PositionUpdate.y += m_Velocity * deltaTime;

    {
        auto hitBlocks = queryHits();

        glm::vec3 totalCorrectionY(0.0f);
        for (const auto& it : hitBlocks) {
            if (it.overlapY != 0.0f) {
                glm::vec3 c = it.normal * it.overlapY;
                if (glm::length(c) > glm::length(totalCorrectionY))
                    totalCorrectionY = c;
                m_Velocity = 0.0f;
            }
        }

        // this is a distance, not a velocity, dont scale by deltaTime
        m_PositionUpdate += totalCorrectionY;
    }

    m_PrevPosition = m_Position;
    m_Position = m_PositionUpdate;
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

    updateCameraVectors();
}

void Camera::DispatchMouseScrollEvent(float scroll)
{
    m_Speed += scroll;

    // if we go too low
    m_Speed = (m_Speed < 0) ? 0 : m_Speed;
}

// calculates the front vector from the camera's updated euler angles
void Camera::updateCameraVectors()
{
    // Yaw is circular: crossing either boundary should continue from the
    // equivalent angle on the other side instead of stopping rotation.
    if (m_Yaw > 180.0f)
        m_Yaw -= 360.0f;
    else if (m_Yaw < -180.0f)
        m_Yaw += 360.0f;

    // cos m_Pitch dampens x rot when looking very high or low
    m_Orientation.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

    // simply sin for y component
    m_Orientation.y = sin(glm::radians(m_Pitch));

    m_Orientation.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

    m_Orientation = glm::normalize(m_Orientation);

    // ori changed so we need to recalc up and right
    m_Right = glm::normalize(glm::cross(m_Orientation, g_Up));
    m_Up = glm::normalize(glm::cross(m_Right, m_Orientation));
}
