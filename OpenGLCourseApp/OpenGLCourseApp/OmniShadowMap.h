#pragma once
#include "ShadowMap.h"


class OmniShadowMap :
    public ShadowMap
{
public:
	OmniShadowMap();

	virtual bool init(GLuint width, GLuint height);

	virtual void Write();

	virtual void Read(GLenum textureUnit);

	~OmniShadowMap();
};

