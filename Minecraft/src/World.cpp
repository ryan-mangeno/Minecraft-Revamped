#include "World.h"
#include "FMath.h"
#include "constants.h"

#include <iostream>

World::World()
    : m_ThreadPool(4), m_RenderHeight(1), m_ChunksLoading(0), m_NumChunks(0),
      m_NumChunksRendered(0), m_PrevCamX(0), m_PrevCamY(20), m_PrevCamZ(0),
      m_TorchModel(
          (getResourcePath() / "assets/minecraft_torch.glb").string()) {
  m_Atmosphere.add_light({0.0f, 30.0f, 0.0f});

  // The OpenGL context exists before World is first requested in
  // Minecraft::Run.
  m_TorchModel.Init();
}

World::~World() {}

void World::Update(glm::vec3 camPos, Shader *shader) {

  // we want to floor if we are negative, since int divsion will round up
  int camChunkX =
      camPos.x < 0 ? ftoi_fast(camPos.x / CHUNK_SIZE) : camPos.x / CHUNK_SIZE;
  int camChunkY =
      camPos.y < 0 ? ftoi_fast(camPos.y / CHUNK_SIZE) : camPos.y / CHUNK_SIZE;
  int camChunkZ =
      camPos.z < 0 ? ftoi_fast(camPos.z / CHUNK_SIZE) : camPos.z / CHUNK_SIZE;

  // Check if camera moved to new chunk
  if (camChunkX != m_PrevCamX || camChunkY != m_PrevCamY ||
      camChunkZ != m_PrevCamZ) {

    m_PrevCamX = camChunkX;
    m_PrevCamY = camChunkY;
    m_PrevCamZ = camChunkZ;

    // chunk we are at
    /*

    we want to prioritize the chunk we are at currently as soon as we can in a
    queue if we dont and we rapidly move to a new area we would be generating
    chunks possibly out of render distance

    */
    m_Queue = {};
    if (m_Chunks.find({camChunkX, camChunkY, camChunkZ}) == m_Chunks.end())
      m_Queue.push({camChunkX, camChunkY, camChunkZ});

    auto enqueueSymmetric = [&](int x, int y, int z) -> void {
      m_Queue.push({camChunkX + x, y, camChunkZ + z});

      // to avoid duplicates
      if (y != 0)
        m_Queue.push({camChunkX + x, -y, camChunkZ + z});
    };

    // for each render distance 'step', iterate over the shells of a cube in xyz
    // plane
    // ... where max(|x|, |z|) == step, and enqueue symmetric chunk positions
    // across Y this replaces manual handling of cardinal directions, corners,
    // and edges I had previously

    for (int step = 0; step < m_RenderDistance; ++step) {
      for (int x = -step; x <= step; ++x) {
        for (int z = -step; z <= step; ++z) {
          // skiping inner cube faces, we only want the shell
          if (std::max(std::abs(x), std::abs(z)) == step) {
            for (int y = 0; y <= m_RenderHeight; ++y) {
              enqueueSymmetric(x, y, z);
            }
          }
        }
      }
    }
  } else if (m_ChunksLoading == 0 && !m_Queue.empty()) {
    // proccess next item since queue no empty
    glm::vec3 next = m_Queue.front();
    m_Queue.pop();

    cTuple chunkTuple{next.x, next.y, next.z};

    if (m_Chunks.find(chunkTuple) == m_Chunks.end()) {
      m_Chunks.try_emplace(chunkTuple, next, m_ThreadPool);
    }
  }

  m_ChunksLoading = 0;
  m_NumChunks = 0;
  m_NumChunksRendered = 0;
  for (auto it = m_Chunks.begin(); it != m_Chunks.end();) {
    m_NumChunks++;

    bool chunkReady = it->second.GetReadyStatus();

    if (!chunkReady)
      m_ChunksLoading++;

    glm::vec3 &cPos = it->second.GetPos();

    int chunkX = cPos.x;
    int chunkY = cPos.y;
    int chunkZ = cPos.z;

    if (chunkReady && (abs(chunkX - camChunkX) > m_RenderDistance ||
                       abs(chunkY - camChunkY) > m_RenderDistance ||
                       abs(chunkZ - camChunkZ) > m_RenderDistance)) {
      it->second.SetRender(false);
      m_NumChunksRendered--;
      ++it;
    } else {
      it->second.OnUpdate();
      it->second.SetRender(true);
      m_NumChunksRendered++;
      ++it;
    }
  }

  // update sun pos here as well but sun pos is static for now
  m_Atmosphere.update(shader);
}

void World::Render(Shader *terrainShader, Shader *modelShader) {

  // set sun pos
  terrainShader->Bind();
  terrainShader->SetUniformVec3f("uSunPos", m_Atmosphere.get_sun_pos());

  for (auto it = m_Chunks.begin(); it != m_Chunks.end(); it++) {
    it->second.TryRender(terrainShader);
  }

  modelShader->Bind();
  for (const glm::vec3 &lightPosition : m_Atmosphere.get_point_lights()) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, lightPosition);
    model = glm::scale(model, glm::vec3(0.1f));
    modelShader->SetUniformMat4f("model", model);
    m_TorchModel.Render(modelShader);
  }
}

std::vector<unsigned int> &World::GetChunkData(int chunkX, int chunkY,
                                               int chunkZ) {
  cTuple chunkTuple{chunkX, chunkY, chunkZ};
  if (m_Chunks.find(chunkTuple) == m_Chunks.end()) {
    static std::vector<unsigned int> emptyVector;
    return emptyVector;
  } else {
    return m_Chunks[chunkTuple].GetData();
  }
}

void World::MarkNeighbors(int lx, int ly, int lz, int cx, int cy, int cz) {
  const int max = CHUNK_SIZE - 1;

  auto markChunk = [&](std::vector<glm::ivec3> &dvecs) -> void {
    for (glm::ivec3 &dvec : dvecs) {
      Chunk *neighbor = GetChunk(cx + dvec.x, cy + dvec.y, cz + dvec.z);

      if (neighbor) {
        neighbor->SetDirty(true);
        neighbor->OnUpdate(); // Update duplicated face in the neighbor
      }
    }
  };

  // takes in local indices for lx, ly, lz

  auto getEdgeDirections = [](int x, int y, int z) -> std::vector<glm::ivec3> {
    std::vector<glm::ivec3> dirs;

    if (x == 0)
      dirs.push_back({-1, 0, 0}); // -X
    else if (x == CHUNK_SIZE - 1)
      dirs.push_back({1, 0, 0}); // +X

    if (y == 0)
      dirs.push_back({0, -1, 0}); // -Y
    else if (y == CHUNK_SIZE - 1)
      dirs.push_back({0, 1, 0}); // +Y

    if (z == 0)
      dirs.push_back({0, 0, -1}); // -Z
    else if (z == CHUNK_SIZE - 1)
      dirs.push_back({0, 0, 1}); // +Z

    return dirs;
  };

  // Check if the block is on the edge of the chunk
  std::vector<glm::ivec3> dirs = getEdgeDirections(lx, ly, lz);
  markChunk(dirs);
}
