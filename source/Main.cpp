#include "GameSystem.h"
#include <time.h>

int main( int argc, char** argv )
{
	srand(time(NULL));
	orx_Execute( argc, argv, GameSystem::Init, GameSystem::Run, GameSystem::Exit );
 
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	//Hokay guys, this little tidbit is to get rid of that stupid console window.
	//What you do is go to Project->(ProjNameHere) Properties, then go to
	//Configuration Properties->Linker->System and in SubSystem, click the dropdown
	//box and choose WINDOWS. Compile and run, and that pesky console window is gone!
	//
	//To revert back to actually having the console, simply change the SubSystem from
	//WINDOWS to CONSOLE. Some extra checks were put in place to insure that the
	//program completely terminated rather than occasionally allowing the console to hang
	//and in cases where it's hidden, the program couldn't be terminated outside of task manager.
	//-Chris
	orx_WinExecute(GameSystem::Init, GameSystem::Run, GameSystem::Exit);
}