#include "TextureLoader.h"
#include "../stb_image.h"
#include <glad/glad.h>
#include <iostream>

namespace Core 
{
	TextureLoader::TextureLoader()
	{
	}

	unsigned int TextureLoader::LoadTexture(const std::string &textureName, bool isTransparent)
	{
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char *data = stbi_load(textureName.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			int imageDataType = GL_RGB;
			if (isTransparent)
			{
				imageDataType = GL_RGBA;
			}
			glTexImage2D(GL_TEXTURE_2D, 0, imageDataType, width, height, 0, imageDataType, GL_UNSIGNED_BYTE, data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glGenerateMipmap(GL_TEXTURE_2D);
            // Sets the type of filtering to be used on magnifying and
            // minifying the active texture. These are the nicest available options.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
		return texture;
	}
}