#include "App.h"

#include <stdlib.h>

enum MainStates
{
	MAIN_CREATION,
	MAIN_AWAKE,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_CLEAN,
	MAIN_FAIL,
	MAIN_EXIT
};

int main(int argc, char* argv[])
{
	MainStates state = MAIN_CREATION;
	int result = EXIT_FAILURE;

	while (state != MAIN_EXIT)
	{
		switch (state)
		{
		case MAIN_CREATION:
			state = MAIN_AWAKE;
			break;

		case MAIN_AWAKE:
			state = MAIN_START;
			break;

		case MAIN_START:
			state = MAIN_UPDATE;
			break;

		case MAIN_UPDATE:
			state = MAIN_CLEAN;
			break;

		case MAIN_CLEAN:
			state = MAIN_EXIT;
			break;

		case MAIN_FAIL:
			state = MAIN_EXIT;
			break;

		default:
			break;
		}
	}
	return 0;
}