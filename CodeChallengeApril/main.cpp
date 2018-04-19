#include <iostream>
#include "map.h"
#include <sys/stat.h>

enum ErrCode
{
	Success = 0,
	InvalidStart = 1,
	InvalidEnd = 2,
	BlockedStart = 3,
	BlockedEnd = 4,
	InvalidPath = 5,
	InvalidArgCount = 6,
	UnableToFindPath
};

bool ReadMap(const char* path, std::vector<char>& output)
{
	struct stat stat_buf;
	int32_t rc = stat(path, &stat_buf);

	if (rc != 0 || stat_buf.st_size < Map::kMapSize*Map::kMapSize)
		return false;

	FILE* file = fopen(path, "rb");

	if (!file)
		return false;

	output.resize(stat_buf.st_size);

	int32_t readSize = fread(output.data(), 1, output.size(), file);

	fclose(file);

	if (readSize != output.size())
	{
		return false;
	}

	return true;
}

struct ParsedArgs 
{
	std::vector<char> mapFileData;
	vector2 startPoint;
	vector2 endPoint;
};

ErrCode ParseArg(int32_t argc, const char* argv[], ParsedArgs& parsedArgs)
{
	if (argc != 6)
		return InvalidArgCount;

	const char* mapPath = argv[1];

	if (!ReadMap(mapPath, parsedArgs.mapFileData))
		return InvalidPath;

	parsedArgs.startPoint.x = atoi(argv[2]);
	parsedArgs.startPoint.y = atoi(argv[3]);

	parsedArgs.endPoint.x = atoi(argv[4]);
	parsedArgs.endPoint.y = atoi(argv[5]);

	if (parsedArgs.startPoint.x < 0 || parsedArgs.startPoint.x >= Map::kMapSize 
		|| parsedArgs.startPoint.y < 0 || parsedArgs.startPoint.y >= Map::kMapSize)
		return InvalidStart;

    if(parsedArgs.endPoint.x < 0 || parsedArgs.endPoint.x >= Map::kMapSize 
		|| parsedArgs.endPoint.y < 0 || parsedArgs.endPoint.y >= Map::kMapSize)
		return InvalidEnd;

    for(auto i = parsedArgs.mapFileData.begin();i != parsedArgs.mapFileData.end(); ++i)
    {
        if(*i == '\r')
        {
            auto itr = parsedArgs.mapFileData.erase(i);
            if(itr != parsedArgs.mapFileData.end())
                i = itr;

        }
    }

	// +1 to account for whitespace at the end
	if (parsedArgs.mapFileData[parsedArgs.startPoint.x + ((Map::kMapSize+1) * parsedArgs.startPoint.y)] == Map::cellBlocked)
		return BlockedStart;

	if(parsedArgs.mapFileData[parsedArgs.endPoint.x + ((Map::kMapSize+1) * parsedArgs.endPoint.y)] == Map::cellBlocked)
		return BlockedEnd;

	return Success;
}

int32_t main(int32_t argc, const char* argv[])
{
	ParsedArgs args;
	int32_t argResult = ParseArg(argc, argv, args);

	if (argResult != 0)
		return argResult;

	Map map(args.mapFileData, args.startPoint, args.endPoint);
    
	if (!map.ComputePath())
		return UnableToFindPath;

	map.DisplayMap();
    
	return 0;
}