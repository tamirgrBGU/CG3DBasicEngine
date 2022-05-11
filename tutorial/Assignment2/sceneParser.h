#include <fstream>
#include <iostream>
#include <errno.h>
#include <vector>
#include <string>
#include "../../external/eigen/Eigen/Core"

struct SceneData
{
	Eigen::Vector4f eye;         //position + shine
	Eigen::Vector4f ambient;
	Eigen::Vector4i sizes;				// total object num, lights num, reflecting object num, transmitting object num
	std::vector<Eigen::Vector4f> objects;      //center coordinates + radius / normal + d
	std::vector<Eigen::Vector4f> lights;        //position + cos(angle)
	std::vector<Eigen::Vector4f> directions;     //direction +  is directional 0.0/1.0
	std::vector<Eigen::Vector4f> colors;
	std::vector<Eigen::Vector4f> intensities;		   //light intensity
};

class SceneParser
{
private:
	
	static inline unsigned int FindNextChar(unsigned int start, const char* str, unsigned int length, char token)
	{
		unsigned int result = start;
		while(result < length)
		{
			result++;
			if(str[result] == token)
				break;
		}
    
		return result;
	}

	static inline unsigned int ParseIndexValue(const std::string& token, unsigned int start, unsigned int end)
	{
		return atoi(token.substr(start, end - start).c_str()) - 1;
	}

	static inline float ParseFloatValue(const std::string& token, unsigned int start, unsigned int end)
	{
		return atof(token.substr(start, end - start).c_str());
	}
	Eigen::Vector4f parseVec4(const std::string& line);
public:
	SceneParser(const std::string& fileName, SceneData* data);
};