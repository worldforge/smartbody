#include "converter.h"

int main(int argc, char* argv[])
{
	printf("Number of arguments: %d\n", argc);
	for (int i = 0; i < argc; ++i)
	{
		printf("%s\n", argv[i]);
	}
	if (argc != 3)
	{
		printf("Input parameters: <root directory for the files need processing> <location of mapping file from worldbet to SmartBody phoneme set>.\n");
		return 0;
	}
	CSLUConverter::convertFromCSLUToBML(argv[1], argv[2]);
	return 0;
}