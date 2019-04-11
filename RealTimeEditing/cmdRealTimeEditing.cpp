// cmdRealTimeEditing.cpp : command file
//

#include "StdAfx.h"
#include "RealTimeEditingPlugIn.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN RealTimeEditing command
//

#pragma region RealTimeEditing command

// Do NOT put the definition of class CCommandRealTimeEditing in a header
// file. There is only ONE instance of a CCommandRealTimeEditing class
// and that instance is the static theRealTimeEditingCommand that appears
// immediately below the class definition.

class CCommandRealTimeEditing : public CRhinoCommand
{
public:
  // The one and only instance of CCommandRealTimeEditing is created below.
  // No copy constructor or operator= is required.
  // Values of member variables persist for the duration of the application.

  // CCommandRealTimeEditing::CCommandRealTimeEditing()
  // is called exactly once when static theRealTimeEditingCommand is created.
  CCommandRealTimeEditing() = default;

  // CCommandRealTimeEditing::~CCommandRealTimeEditing()
  // is called exactly once when static theRealTimeEditingCommand is destroyed.
  // The destructor should not make any calls to the Rhino SDK. 
  // If your command has persistent settings, then override 
  // CRhinoCommand::SaveProfile and CRhinoCommand::LoadProfile.
  ~CCommandRealTimeEditing() = default;

  // Returns a unique UUID for this command.
  // If you try to use an id that is already being used, then
  // your command will not work. Use GUIDGEN.EXE to make unique UUID.
  UUID CommandUUID() override
  {
    // {423F9DD4-2940-4315-A407-994F09A1C234}
    static const GUID RealTimeEditingCommand_UUID =
    { 0x423F9DD4, 0x2940, 0x4315, { 0xA4, 0x7, 0x99, 0x4F, 0x9, 0xA1, 0xC2, 0x34 } };
    return RealTimeEditingCommand_UUID;
  }

  // Returns the English command name.
  // If you want to provide a localized command name, then override 
  // CRhinoCommand::LocalCommandName.
  const wchar_t* EnglishCommandName() override { return L"RealTimeEditing"; }

  // Rhino calls RunCommand to run the command.
  CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override;
};

// The one and only CCommandRealTimeEditing object
// Do NOT create any other instance of a CCommandRealTimeEditing class.
static class CCommandRealTimeEditing theRealTimeEditingCommand;

CRhinoCommand::result CCommandRealTimeEditing::RunCommand(const CRhinoCommandContext& context)
{
  // CCommandRealTimeEditing::RunCommand() is called when the user
  // runs the "RealTimeEditing".

  // TODO: Add command code here.

  // Rhino command that display a dialog box interface should also support
  // a command-line, or scriptable interface.

  ON_wString str;
  str.Format(L"The \"%s\" command is under construction.\n", EnglishCommandName());
  const wchar_t* pszStr = static_cast<const wchar_t*>(str);
  if (context.IsInteractive())
    RhinoMessageBox(pszStr, RealTimeEditingPlugIn().PlugInName(), MB_OK);
  else
    RhinoApp().Print(pszStr);

  // TODO: Return one of the following values:
  //   CRhinoCommand::success:  The command worked.
  //   CRhinoCommand::failure:  The command failed because of invalid input, inability
  //                            to compute the desired result, or some other reason
  //                            computation reason.
  //   CRhinoCommand::cancel:   The user interactively canceled the command 
  //                            (by pressing ESCAPE, clicking a CANCEL button, etc.)
  //                            in a Get operation, dialog, time consuming computation, etc.

  return CRhinoCommand::success;
}

#pragma endregion

//
// END RealTimeEditing command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
