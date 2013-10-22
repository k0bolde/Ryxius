#ifndef __STANDALONE_H__
#define __STANDALONE_H__

#include "orx.h" // We want to interface with ORX, so including it is helpful! :)
#include <string> // Our 'GetObjectByName' and 'Update' functions both use strings for the comparison tests.
 

class StandAlone
{
public:
	// Instance deals with ensuring we only have one copy of our StandAlone class.
	static StandAlone* Instance();
 
	// Init function sets up our default/first scene.
	static orxSTATUS orxFASTCALL Init();
 
	// Run and Exit are both empty for now.
	static orxSTATUS orxFASTCALL Run();
	static void orxFASTCALL Exit();
 
	// GetObjectByName compares the names of the objects loaded in memory, with a string we pass in
	//   either returning the first matching object, or a null pointer.
	static orxOBJECT* orxFASTCALL GetObjectByName( std::string objectName );
 
	// Update is called on each 'tick' of a clock.
	static void orxFASTCALL Update( const orxCLOCK_INFO* clockInfo, void* context );
 
	// We currently use EventHandler to simply log when some of events happen, so we know they're working.
	static orxSTATUS orxFASTCALL EventHandler( const orxEVENT* currentEvent );
// GetMouseWorldPosition gets the mouse 'screen' position, converts that to 'world'
	//   coordinates, and returns the vector.
	static orxVECTOR orxFASTCALL GetMouseWorldPosition();
 
protected:
	StandAlone();
	StandAlone(const StandAlone&);
	StandAlone& operator= (const StandAlone&);
 
private:
	static StandAlone* m_Instance;
};
 
#endif // __STANDALONE_H__