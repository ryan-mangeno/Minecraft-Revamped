#include "PlayerObj.h"
#include "Model.h"

ModelLoader::Model
    PlayerObj::m_PlayerModel("resources/models/steve/scene.gltf");

PlayerObj::PlayerObj(const glm::vec3 &pos)
    : m_Position(pos), m_Orientation(glm::vec3(0.f, 0.f, 1.f)),
      m_Collider(
          m_Position, 0.6f,
          1.8f) // todo: define these as a constexpr var for height & width
// player is 0.6 blocks wide, 1.8 blocks tall
{}
