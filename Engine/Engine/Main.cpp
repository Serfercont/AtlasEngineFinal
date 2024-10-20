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
	App* app = NULL;

	while (state != MAIN_EXIT)
	{
		switch (state)
		{
		case MAIN_CREATION:
			app = new App(argc, argv);

			if (app != NULL)
				state = MAIN_AWAKE;
			else
				state = MAIN_FAIL;

			break;

		case MAIN_AWAKE:

			if (app->Awake() == true)
				state = MAIN_START;
			else
				state = MAIN_FAIL;

			break;

		case MAIN_START:
			if (app->Start() == true)
				state = MAIN_UPDATE;
			else
				state = MAIN_FAIL;

			break;

		case MAIN_UPDATE:
			if (app->Update() == false)
				state = MAIN_CLEAN;

			break;

		case MAIN_CLEAN:
			if (app->CleanUp() == true)
			{
				result = EXIT_SUCCESS;
				state = MAIN_EXIT;
			}
			else
				state = MAIN_FAIL;

			break;

		case MAIN_FAIL:
			result = EXIT_FAILURE;
			state = MAIN_EXIT;
			break;

		default:
			break;
		}
	}
	return 0;
}