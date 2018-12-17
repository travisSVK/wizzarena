#include <string>
#include <map>

namespace Core
{
	class TextureLoader
	{
	public:
		TextureLoader();
		~TextureLoader() {}

		unsigned int LoadTexture(const std::string &textureName, bool isTransparent = false);
	private:
	};
}