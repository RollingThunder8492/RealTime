////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN RTMove command
//

#include "stdafx.h"

#pragma region RTMove command

class CRhinoGetTranslationPoint : public CRhinoGetXform
{
public:
	CRhinoGetTranslationPoint() = default;
	~CRhinoGetTranslationPoint() = default;
	BOOL CalculateTransform(CRhinoViewport& vp, const ON_3dPoint& pt, ON_Xform& xform);
};

BOOL CRhinoGetTranslationPoint::CalculateTransform(CRhinoViewport& vp, const ON_3dPoint& pt, ON_Xform& xform)
{
	UNREFERENCED_PARAMETER(vp);
	ON_3dVector dir = pt - m_basepoint;
	if (dir.IsTiny())
		xform = ON_Xform::IdentityTransformation;
	else
		xform.TranslationTransformation(dir);
	return (xform.IsValid()) ? TRUE : FALSE;
}

class CCommandRTMove : public CRhinoCommand
{
public:
	CCommandRTMove() = default;
	UUID CommandUUID() override
	{
		// {928076B4-D50-40BC-B115-0D47B73515B8}
		static const GUID RTMoveCommand_UUID =
		{ 0x928076B4, 0xD50, 0x40BC, { 0xB1, 0x15, 0x0D, 0x47, 0xB7, 0x35, 0x15, 0xB8 } };
		return RTMoveCommand_UUID;
	}
	const wchar_t* EnglishCommandName() override { return L"RTMove"; }
	CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override;
};

// The one and only CCommandRTMove object
static class CCommandRTMove theRTMoveCommand;

CRhinoCommand::result CCommandRTMove::RunCommand(const CRhinoCommandContext& context)
{
	CRhinoGetObject go;
	go.SetCommandPrompt(L"Select objects to move");
	go.GetObjects(1, 0);
	if (go.CommandResult() != CRhinoCommand::success)
		return go.CommandResult();

	CRhinoGetPoint gp;
	gp.SetCommandPrompt(L"Point to move from");
	gp.GetPoint();
	if (gp.CommandResult() != CRhinoCommand::success)
		return gp.CommandResult();

	CRhinoGetTranslationPoint gt;
	gt.SetCommandPrompt(L"Point to move to");
	gt.SetBasePoint(gp.Point());
	gt.DrawLineFromPoint(gp.Point(), TRUE);
	gt.AppendObjects(go);
	gt.GetXform();
	if (gt.CommandResult() != CRhinoCommand::success)
		return gp.CommandResult();

	CRhinoView* view = gt.View();
	if (0 == view)
		return CRhinoCommand::failure;

	ON_Xform xform;
	if (gt.CalculateTransform(view->ActiveViewport(), gt.Point(), xform))
	{
		for (int i = 0; i < go.ObjectCount(); i++)
		{
			CRhinoObjRef obj_ref = go.Object(i);
			context.m_doc.TransformObject(obj_ref, xform);
		}
		context.m_doc.Redraw();
	}

	return CRhinoCommand::success;
}

#pragma endregion

//
// END RTMove command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
