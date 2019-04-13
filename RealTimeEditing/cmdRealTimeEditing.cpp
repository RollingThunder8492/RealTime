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

class CRhinoGetTranslationPoint : public CRhinoGetXform
{
public:
	CRhinoGetTranslationPoint() = default;
	~CRhinoGetTranslationPoint() = default;
	BOOL CalculateTransform(CRhinoViewport& vp, const ON_3dPoint& pt, ON_Xform& xform);
	ON_3dVector GetDir();

private:
	ON_3dVector dir;
};

BOOL CRhinoGetTranslationPoint::CalculateTransform(CRhinoViewport& vp, const ON_3dPoint& pt, ON_Xform& xform)
{
	UNREFERENCED_PARAMETER(vp);
	dir = pt - m_basepoint;
	if (dir.IsTiny())
		xform = ON_Xform::IdentityTransformation;
	else
		xform = ON_Xform::TranslationTransformation(dir);
	return (xform.IsValid()) ? TRUE : FALSE;
}

ON_3dVector CRhinoGetTranslationPoint::GetDir()
{
	return dir;
}

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

	//select point on mesh that we want to update
	CRhinoGetObject gv;
	gv.SetCommandPrompt(L"Select Mesh Vertex to Move");
	gv.SetGeometryFilter(CRhinoObject::meshvertex_filter);
	gv.GetObjects(1, 1);
	if (gv.CommandResult() != CRhinoCommand::success)
		return gv.CommandResult();

	//Extract vertex and mesh data
	CRhinoObjRef obj_ref = gv.Object(0);
	const CRhinoObject* obj = obj_ref.Object();
	const ON_Mesh* mesh = obj_ref.Mesh();
	int vertex_index = obj_ref.GeometryComponentIndex().m_index;
	if (obj == 0 | mesh == 0)
		return CRhinoCommand::failure;

	//Following gets ask for the translation
	CRhinoGetPoint gp;
	gp.SetCommandPrompt(L"Point to move from");
	gp.GetPoint();
	if (gp.CommandResult() != CRhinoCommand::success)
		return gp.CommandResult();

	CRhinoGetTranslationPoint gt;
	gt.SetCommandPrompt(L"Point to move to");
	gt.SetBasePoint(gp.Point());
	gt.DrawLineFromPoint(gp.Point(), TRUE);
	gt.AppendObjects(gv);
	gt.GetXform();
	if (gt.CommandResult() != CRhinoCommand::success)
		return gp.CommandResult();

	CRhinoView* view = gt.View();
	if (0 == view)
		return CRhinoCommand::failure;

	ON_Xform xform;
	ON_Mesh copied_mesh(*mesh);
	if (gt.CalculateTransform(view->ActiveViewport(), gt.Point(), xform))
	{
		copied_mesh.SetVertex(vertex_index, gt.GetDir() + mesh->m_V[vertex_index]);

		//Need to invalidate things so they may be recalculated
		copied_mesh.InvalidateVertexBoundingBox();
		copied_mesh.InvalidateVertexNormalBoundingBox();
		copied_mesh.InvalidateCurvatureStats();
		copied_mesh.m_FN.SetCount(0);
		copied_mesh.m_N.SetCount(0);
		copied_mesh.ComputeFaceNormals();
		copied_mesh.ComputeVertexNormals();
		copied_mesh.SetClosed(-1);

		if (copied_mesh.IsValid())
		{
			context.m_doc.ReplaceObject(CRhinoObjRef(obj), copied_mesh);
			context.m_doc.Redraw();
		}
	}

	/*
	if (gt.CalculateTransform(view->ActiveViewport(), gt.Point(), xform))
	{
		for (int i = 0; i < gv.ObjectCount(); i++)
		{
			CRhinoObjRef obj_ref = gv.Object(i);
			context.m_doc.TransformObject(obj_ref, xform);
		}
		context.m_doc.Redraw();
	}
	*/

	

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
