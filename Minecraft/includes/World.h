#pragma once

#include <glm/glm.hpp>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include "Atmosphere.h"
#include "Chunk.h"
#include "Hash.h"
#include "Model.h"
#include "Shader.h"
#include "util.h"

using cTuple = std::tuple<int, int, int>;

class World {

public:
  World();
  ~World();

  std::vector<unsigned int> &GetChunkData(int chunkX, int chunkY, int chunkZ);
  void Update(glm::vec3 camPos, Shader *shader);
  void Render(Shader *terrainShader, Shader *modelShader);

  inline void SetDirtyChunk(int chunkX, int chunkY, int chunkZ) {
    cTuple chunkTuple{chunkX, chunkY, chunkZ};
    if (m_Chunks.find(chunkTuple) != m_Chunks.end()) {
      m_Chunks[chunkTuple].SetDirty(true);
    }
  }

  inline Chunk *GetChunk(int chunkX, int chunkY, int chunkZ) {
    cTuple chunkTuple{chunkX, chunkY, chunkZ};
    if (m_Chunks.find(chunkTuple) != m_Chunks.end()) {
      return &m_Chunks[chunkTuple];
    } else {
      return nullptr;
    }
  }

  static World &GetWorld() {
    static World w;
    return w;
  }

  void MarkNeighbors(int lx, int ly, int lz, int cx, int cy, int cz);

private:
  // for chunk generation
  Thread::ThreadPool m_ThreadPool;

  std::unordered_map<cTuple, Chunk> m_Chunks;
  std::queue<glm::vec3> m_Queue;

  int m_RenderDistance = 12;
  int m_RenderHeight = 1;

  unsigned int m_ChunksLoading;
  unsigned int m_NumChunks;
  unsigned int m_NumChunksRendered;

  int m_PrevCamX;
  int m_PrevCamY;
  int m_PrevCamZ;

  Atmosphere m_Atmosphere;
  ModelLoader::Model m_TorchModel;
};
