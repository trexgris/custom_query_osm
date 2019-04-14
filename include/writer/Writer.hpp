#pragma once 
namespace osm {
namespace query {
namespace writer {
std::size_t Callback(char* ptr, std::size_t size, std::size_t nmemb, void *f)
{
	FILE *file = (FILE *)f;
	return fwrite(ptr, size, nmemb, file);
};
}
}
}