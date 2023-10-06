#include "ShadowMap.h"

ShadowMap::ShadowMap()
{
	FBO = 0;
	shadowMap = 0;
}

bool ShadowMap::init(GLuint width, GLuint height)
{
	// Set the shadow map width and height
	shadowWidth = width; 
	shadowHeight = height;

	// Generate framebuffer and shadow map texture
	glGenFramebuffers(1, &FBO);
	glGenTextures(1, &shadowMap);
	
	// Bind the shadow map texture and specify its properties
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	// Set texture wrapping and filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float bColor[] = { 1.0f, 1.0f , 1.0f , 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bColor);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind the framebuffer and attach the shadow map texture to it as a depth attachment
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

	// Set draw and read buffers to none, indicating that this framebuffer will not be used for rendering to the screen
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// Check the framebuffer's completeness status
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebuffer Error: %i/n", status);
		return false;
	}

	// Unbind the framebuffer to prevent accidental rendering into it
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Return true if the initialization is successful
	return true;
}

void ShadowMap::Write()
{
	//Write to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void ShadowMap::Read(GLenum textureUnit)
{
	// Activate the specified texture unit
	glActiveTexture(textureUnit);
	// Bind the shadow map texture to the specified texture unit
	glBindTexture(GL_TEXTURE_2D, shadowMap);
}

ShadowMap::~ShadowMap()
{
	//Delete the FBO Frame buffer
	if (FBO) {
		glDeleteFramebuffers(1, &FBO);
	}

	//Delete the shadowMap texture
	if (shadowMap) {
		glDeleteTextures(1, &shadowMap);
	}
}
