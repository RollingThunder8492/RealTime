////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN RTRotateCenter command
//

#include "stdafx.h"

#pragma region RTRotateCenter command

class CCommandRTRotateCenter : public CRhinoCommand
{
public:
	CCommandRTRotateCenter() { m_angle = 0.0; }
	~CCommandRTRotateCenter() = default;
	UUID CommandUUID() override
	{
		// {B0A2E991-15FF-45E1-B6F2-947C168FCB5E}
		static const GUID RTRotateCenterCommand_UUID =
		{ 0xB0A2E991, 0x15FF, 0x45E1, { 0xB6, 0xF2, 0x94, 0x7C, 0x16, 0x8F, 0xCB, 0x5E } };
		return RTRotateCenterCommand_UUID;
	}
	const wchar_t* EnglishCommandName() override { return L"RTRotateCenter"; }
	CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override;

private:
	double m_angle;
};

// The one and only CCommandRTRotateCenter object
static class CCommandRTRotateCenter theRTRotateCenterCommand;

CRhinoCommand::result CCommandRTRotateCenter::RunCommand(const CRhinoCommandContext& context)
{
	// Select objects to rotate
	CRhinoGetObject go;
	go.SetCommandPrompt(L"Select objects to rotate");
	go.GetObjects(1, 0);
	if (go.CommandResult() != CRhinoCommand::success)
		return go.CommandResult();

	// Rotation angle (in degrees)
	CRhinoGetNumber gn;
	gn.SetCommandPrompt(L"Rotation angle");
	gn.SetDefaultNumber(m_angle);
	gn.GetNumber();
	if (gn.CommandResult() != CRhinoCommand::success)
		return gn.CommandResult();

	// Validate input
	double angle = gn.Number();
	if (angle == 0)
		return CRhinoCommand::nothing;

	m_angle = angle;

	// Get the active view's construction plane
	ON_Plane plane = RhinoActiveCPlane();

	int i;
	for (i = 0; i < go.ObjectCount(); i++)
	{
		// Do not split objects that get kinky 
		// when they are transformed.
		CRhinoKeepKinkySurfaces keep_kinky_srfs;

		// Get an object reference
		const CRhinoObjRef& ref = go.Object(i);

		// Get the real object
		const CRhinoObject* obj = ref.Object();
		if (!obj)
			continue;

		// Get the object's tight bounding box
		ON_BoundingBox bbox;
		if (!obj->GetTightBoundingBox(bbox, false, 0))
			continue;

		// Create transformation matrix
		ON_Xform xform;
		xform.Rotation(m_angle * ON_PI / 180.0, plane.zaxis, bbox.Center());

		// Transform the object
		context.m_doc.TransformObject(obj, xform, true, true, true);
	}

	context.m_doc.Redraw();

	return CRhinoCommand::success;
}

#pragma endregion

//
// END RTRotateCenter command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
