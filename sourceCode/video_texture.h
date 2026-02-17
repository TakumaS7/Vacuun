#pragma once

#include "texture.h"
#include "main.h"
struct VideoTexture {

  static bool createAPI();
  static void destroyAPI();

  struct InternalData;
  InternalData* internal_data = nullptr;

  bool create(const char* filename);
  void destroy();
  bool update(float dt);

  void pause();
  void resume();
  bool hasFinished();
  TextureForVideo* getTexture();
  float getAspectRatio() const;
  void setLoop(bool loop);
};

