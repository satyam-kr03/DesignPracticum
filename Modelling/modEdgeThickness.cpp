// Description: This code demonstrates how to change the edge thickness of a shape in OpenCASCADE.

#include "Viewer.h"

#include <BRepTools.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <GC_MakeSegment.hxx>
#include <TopoDS.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepLib.hxx>
#include <Standard_Type.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pnt.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Graphic3d_Camera.hxx>
#include <WNT_Window.hxx>
#include <WNT_WClass.hxx>
#include <chrono>
#include <thread>
#include <Image_PixMap.hxx>
#include <Image_AlienPixMap.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Standard.hxx>
#include <stdio.h>
#include <Image_Format.hxx>
#include <Prs3d_LineAspect.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <Graphic3d_Aspects.hxx>
#include <V3d_DirectionalLight.hxx>
#include <gp_Trsf.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_ColoredDrawer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <Graphic3d_TypeOfShadingModel.hxx>
#include <V3d_TypeOfShadingModel.hxx>

#include <string>
#include <cmath>
#include <Prs3d_LineAspect.hxx>
#include <AIS_Shape.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <TopoDS.hxx>
#include <Graphic3d_Structure.hxx>
#include <iostream>

TopoDS_Shape MakeCube(const Standard_Real size, const Standard_Boolean center = Standard_True, const Standard_Real xSize = 0.0, const Standard_Real ySize = 0.0, const Standard_Real zSize = 0.0)
{
    gp_Pnt origin(0.0, 0.0, 0.0); // Assuming origin is at (0,0,0)
    BRepPrimAPI_MakeBox box(origin, size, size, size);
    box.Build();
    TopoDS_Solid mySolid = box.Solid();
    // Compute offsets based on centering
    if (center)
    {
        if (xSize != 0.0)
        {
            origin.SetX(origin.X() - xSize / 2.0);
        }
        if (ySize != 0.0)
        {
            origin.SetY(origin.Y() - ySize / 2.0);
        }
        if (zSize != 0.0)
        {
            origin.SetZ(origin.Z() - zSize / 2.0);
        }
    }
    return mySolid;
}

TopoDS_Solid MakeSphere(const Standard_Real radius)
{
    gp_Pnt origin(0.0, 0.0, 0.0); // Assuming origin is at (0,0,0)
    BRepPrimAPI_MakeSphere sphere(origin, radius);
    sphere.Build();
    if (!sphere.IsDone())
    {
        // Handle error
        std::cerr << "Failed to create sphere." << std::endl;
        exit(EXIT_FAILURE);
    }
    TopoDS_Solid mySolid = TopoDS::Solid(sphere.Shape());
    return mySolid;
}

TopoDS_Shape MakeBottle(const Standard_Real myWidth, const Standard_Real myHeight,
                        const Standard_Real myThickness)
{
    // Profile : Define Support Points
    gp_Pnt aPnt1(-myWidth / 2., 0, 0);
    gp_Pnt aPnt2(-myWidth / 2., -myThickness / 4., 0);
    gp_Pnt aPnt3(0, -myThickness / 2., 0);
    gp_Pnt aPnt4(myWidth / 2., -myThickness / 4., 0);
    gp_Pnt aPnt5(myWidth / 2., 0, 0);

    // Profile : Define the Geometry
    Handle(Geom_TrimmedCurve) anArcOfCircle = GC_MakeArcOfCircle(aPnt2, aPnt3, aPnt4);
    Handle(Geom_TrimmedCurve) aSegment1 = GC_MakeSegment(aPnt1, aPnt2);
    Handle(Geom_TrimmedCurve) aSegment2 = GC_MakeSegment(aPnt4, aPnt5);

    // Profile : Define the Topology
    TopoDS_Edge anEdge1 = BRepBuilderAPI_MakeEdge(aSegment1);
    TopoDS_Edge anEdge2 = BRepBuilderAPI_MakeEdge(anArcOfCircle);
    TopoDS_Edge anEdge3 = BRepBuilderAPI_MakeEdge(aSegment2);
    TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(anEdge1, anEdge2, anEdge3);

    // Complete Profile
    gp_Ax1 xAxis = gp::OX();
    gp_Trsf aTrsf;

    aTrsf.SetMirror(xAxis);
    BRepBuilderAPI_Transform aBRepTrsf(aWire, aTrsf);
    TopoDS_Shape aMirroredShape = aBRepTrsf.Shape();
    TopoDS_Wire aMirroredWire = TopoDS::Wire(aMirroredShape);

    BRepBuilderAPI_MakeWire mkWire;
    mkWire.Add(aWire);
    mkWire.Add(aMirroredWire);
    TopoDS_Wire myWireProfile = mkWire.Wire();

    // Body : Prism the Profile
    TopoDS_Face myFaceProfile = BRepBuilderAPI_MakeFace(myWireProfile);
    gp_Vec aPrismVec(0, 0, myHeight);
    TopoDS_Shape myBody = BRepPrimAPI_MakePrism(myFaceProfile, aPrismVec);

    // Body : Apply Fillets
    BRepFilletAPI_MakeFillet mkFillet(myBody);
    TopExp_Explorer anEdgeExplorer(myBody, TopAbs_EDGE);
    while (anEdgeExplorer.More())
    {
        TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExplorer.Current());
        // Add edge to fillet algorithm
        mkFillet.Add(myThickness / 12., anEdge);
        anEdgeExplorer.Next();
    }

    myBody = mkFillet.Shape();

    // Body : Add the Neck
    gp_Pnt neckLocation(0, 0, myHeight);
    gp_Dir neckAxis = gp::DZ();
    gp_Ax2 neckAx2(neckLocation, neckAxis);

    Standard_Real myNeckRadius = myThickness / 4.;
    Standard_Real myNeckHeight = myHeight / 10.;

    BRepPrimAPI_MakeCylinder MKCylinder(neckAx2, myNeckRadius, myNeckHeight);
    TopoDS_Shape myNeck = MKCylinder.Shape();

    myBody = BRepAlgoAPI_Fuse(myBody, myNeck);

    // Body : Create a Hollowed Solid
    TopoDS_Face faceToRemove;
    Standard_Real zMax = -1;

    for (TopExp_Explorer aFaceExplorer(myBody, TopAbs_FACE); aFaceExplorer.More(); aFaceExplorer.Next())
    {
        TopoDS_Face aFace = TopoDS::Face(aFaceExplorer.Current());
        // Check if <aFace> is the top face of the bottle's neck
        Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace);
        if (aSurface->DynamicType() == STANDARD_TYPE(Geom_Plane))
        {
            Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aSurface);
            gp_Pnt aPnt = aPlane->Location();
            Standard_Real aZ = aPnt.Z();
            if (aZ > zMax)
            {
                zMax = aZ;
                faceToRemove = aFace;
            }
        }
    }

    TopTools_ListOfShape facesToRemove;
    facesToRemove.Append(faceToRemove);
    BRepOffsetAPI_MakeThickSolid aSolidMaker;
    aSolidMaker.MakeThickSolidByJoin(myBody, facesToRemove, -myThickness / 50, 1.e-3);
    myBody = aSolidMaker.Shape();
    // Threading : Create Surfaces
    Handle(Geom_CylindricalSurface) aCyl1 = new Geom_CylindricalSurface(neckAx2, myNeckRadius * 0.99);
    Handle(Geom_CylindricalSurface) aCyl2 = new Geom_CylindricalSurface(neckAx2, myNeckRadius * 1.05);

    // Threading : Define 2D Curves
    gp_Pnt2d aPnt(2. * M_PI, myNeckHeight / 2.);
    gp_Dir2d aDir(2. * M_PI, myNeckHeight / 4.);
    gp_Ax2d anAx2d(aPnt, aDir);

    Standard_Real aMajor = 2. * M_PI;
    Standard_Real aMinor = myNeckHeight / 10;

    Handle(Geom2d_Ellipse) anEllipse1 = new Geom2d_Ellipse(anAx2d, aMajor, aMinor);
    Handle(Geom2d_Ellipse) anEllipse2 = new Geom2d_Ellipse(anAx2d, aMajor, aMinor / 4);
    Handle(Geom2d_TrimmedCurve) anArc1 = new Geom2d_TrimmedCurve(anEllipse1, 0, M_PI);
    Handle(Geom2d_TrimmedCurve) anArc2 = new Geom2d_TrimmedCurve(anEllipse2, 0, M_PI);
    gp_Pnt2d anEllipsePnt1 = anEllipse1->Value(0);
    gp_Pnt2d anEllipsePnt2 = anEllipse1->Value(M_PI);

    Handle(Geom2d_TrimmedCurve) aSegment = GCE2d_MakeSegment(anEllipsePnt1, anEllipsePnt2);
    // Threading : Build Edges and Wires
    TopoDS_Edge anEdge1OnSurf1 = BRepBuilderAPI_MakeEdge(anArc1, aCyl1);
    TopoDS_Edge anEdge2OnSurf1 = BRepBuilderAPI_MakeEdge(aSegment, aCyl1);
    TopoDS_Edge anEdge1OnSurf2 = BRepBuilderAPI_MakeEdge(anArc2, aCyl2);
    TopoDS_Edge anEdge2OnSurf2 = BRepBuilderAPI_MakeEdge(aSegment, aCyl2);
    TopoDS_Wire threadingWire1 = BRepBuilderAPI_MakeWire(anEdge1OnSurf1, anEdge2OnSurf1);
    TopoDS_Wire threadingWire2 = BRepBuilderAPI_MakeWire(anEdge1OnSurf2, anEdge2OnSurf2);
    BRepLib::BuildCurves3d(threadingWire1);
    BRepLib::BuildCurves3d(threadingWire2);

    // Create Threading
    BRepOffsetAPI_ThruSections aTool(Standard_True);
    aTool.AddWire(threadingWire1);
    aTool.AddWire(threadingWire2);
    aTool.CheckCompatibility(Standard_False);

    TopoDS_Shape myThreading = aTool.Shape();

    // Building the Resulting Compound
    TopoDS_Compound aRes;
    BRep_Builder aBuilder;
    aBuilder.MakeCompound(aRes);
    aBuilder.Add(aRes, myBody);
    aBuilder.Add(aRes, myThreading);

    return aRes;
}

class MyViewer : public AIS_ViewController
{
public:
    //! Main constructor.
    MyViewer()
    {
        // graphic driver setup
        Handle(Aspect_DisplayConnection) aDisplay = new Aspect_DisplayConnection();
        Handle(Graphic3d_GraphicDriver) aDriver = new OpenGl_GraphicDriver(aDisplay);

        // viewer setup
        Handle(V3d_Viewer) aViewer = new V3d_Viewer(aDriver);
        aViewer->SetDefaultLights();
        aViewer->SetLightOn();

        // view setup
        myView = new V3d_View(aViewer);

        const TCollection_AsciiString aClassName("MyWinClass");
        Handle(WNT_WClass) aWinClass =
            new WNT_WClass(aClassName.ToCString(), &windowProcWrapper, 0);
        Handle(WNT_Window) aWindow = new WNT_Window("OCCT Viewer", aWinClass,
                                                    WS_OVERLAPPEDWINDOW,
                                                    100, 100, 512, 512, Quantity_NOC_BLACK);
        ::SetWindowLongPtrW((HWND)aWindow->NativeHandle(), GWLP_USERDATA, (LONG_PTR)this);
        myView->SetImmediateUpdate(false);
        myView->SetWindow(aWindow);
        myView->SetBackgroundColor(Quantity_NOC_GRAY50);
        myView->Camera()->SetProjectionType(Graphic3d_Camera::Projection_Orthographic);
        myView->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_WHITE, 0.1);
        myView->ChangeRenderingParams().RenderResolutionScale = 2.0f;

        // interactive context and demo scene
        myContext = new AIS_InteractiveContext(aViewer);

        // Modify here to display your own TopoDS_Shape object
        TopoDS_Shape aShape = MakeCube(5.0);
        // TopoDS_Shape aShape = MakeBottle(50, 70, 30);
        // TopoDS_Solid aShape = MakeSphere(50);

        // Create AIS_Shape object and display it
        Handle(AIS_Shape) aShapePrs = new AIS_Shape(aShape);
        myContext->Display(aShapePrs, AIS_Shaded, 0, false);

        myView->FitAll(0.01, false);

        aWindow->Map();
        myView->Redraw();
    }

    void SetShading(Graphic3d_TypeOfShadingModel shading)
    {
        myView->SetShadingModel(shading);
    }

    void SetShapeMaterial(Graphic3d_MaterialAspect aMaterialAspect, Standard_Real aTransparency)
    {
        // Retrieve the displayed shape
        AIS_ListOfInteractive aList;
        myContext->DisplayedObjects(aList);

        // Iterate through displayed objects and set material for each shape
        for (AIS_ListIteratorOfListOfInteractive it(aList); it.More(); it.Next())
        {
            Handle(AIS_InteractiveObject) anObj = it.Value();
            if (anObj->IsKind(STANDARD_TYPE(AIS_Shape)))
            {
                Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(anObj);
                if (!aShape.IsNull())
                {
                    // Set material aspect
                    aShape->SetMaterial(aMaterialAspect);

                    // Set transparency
                    aShape->SetTransparency(aTransparency);
                }
            }
        }

        // Redraw the view
        myView->Redraw();
    }

    void rotateAfterDelay(int delay)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        myView->Rotate(V3d_Z, M_PI / 2.0, Standard_True);
    }

    void SaveSnapshot(std::string filename)
    {
        if (myView.IsNull())
            return;

        // Provide dimensions for the image
        int width = 1000;
        int height = 1000;

        myView->Dump(filename.c_str(), Graphic3d_BT_RGB);
    }

    void SetBackgroundColor(const Quantity_Color &color)
    {
        if (!myView.IsNull())
        {
            myView->SetBackgroundColor(color);
            myView->Redraw();
        }
    }

    void SetObjectColor(const Quantity_Color &color)
    {
        if (!myContext.IsNull())
        {
            AIS_ListOfInteractive aList;
            myContext->DisplayedObjects(aList);

            TopoDS_Compound newShape;
            BRep_Builder builder;

            builder.MakeCompound(newShape);

            for (AIS_ListIteratorOfListOfInteractive anIt(aList); anIt.More(); anIt.Next())
            {
                Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(anIt.Value());
                if (!aShape.IsNull())
                {
                    TopoDS_Shape myShape = aShape->Shape();
                    TopTools_IndexedMapOfShape faces;
                    TopExp::MapShapes(myShape, TopAbs_FACE, faces);
                    for (int i = 1; i <= faces.Extent() - 1; i++)
                    {
                        TopoDS_Face face = TopoDS::Face(faces(i));
                        Handle(AIS_Shape) faceShape = new AIS_Shape(face);
                        faceShape->SetColor(color);  // Set color for the face
                        builder.Add(newShape, face); // Add the face to the new shape
                    }
                    myContext->Remove(aShape, Standard_True); // Remove the original shape
                }
            }

            Handle(AIS_Shape) newAIS = new AIS_Shape(newShape);
            myContext->Display(newAIS, Standard_True); // Display the new shape with colored faces
            myView->Redraw();
        }
    }

    void SetBgGradientStyle(const Aspect_GradientFillMethod theMethod = Aspect_GradientFillMethod_Horizontal,
                            const Standard_Boolean theToUpdate = Standard_False)
    {
        if (!myView.IsNull())
        {
            myView->SetBgGradientStyle(theMethod, theToUpdate);
            myView->Redraw();
        }
    }

    void SetEdgeThickness(const Standard_Real thickness)
    {
        if (!myContext.IsNull())
        {
            AIS_ListOfInteractive aList;
            myContext->DisplayedObjects(aList);

            for (AIS_ListIteratorOfListOfInteractive anIt(aList); anIt.More(); anIt.Next())
            {
                Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(anIt.Value());
                if (!aShape.IsNull())
                {
                    Handle(Prs3d_Drawer) drawer = aShape->Attributes();
                    Handle(Prs3d_LineAspect) lineAspect = drawer->LineAspect();
                    drawer->UnFreeBoundaryAspect()->SetWidth(thickness);
                }
            }
            myView->Redraw();
        }
    }

    void IncreaseEdgeThickness()
    {
        AIS_ListOfInteractive aList;
        myContext->DisplayedObjects(aList);

        for (AIS_ListIteratorOfListOfInteractive anIt(aList); anIt.More(); anIt.Next())
        {
            Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(anIt.Value());
            if (!aShape.IsNull())
            {
                Handle(Prs3d_Drawer) drawer = aShape->Attributes();
                Handle(Prs3d_LineAspect) lineAspect = drawer->LineAspect();

                for (int i = 1; i <= 12; i++)
                {
                    drawer->UnFreeBoundaryAspect()->SetWidth(i * 3);
                    aShape->Redisplay(Standard_True);
                }
            }
        }

        myView->Redraw();
    }

    void SetTransparency(Standard_Real transparency)
    {
        if (!myContext.IsNull())
        {
            AIS_ListOfInteractive aList;
            myContext->DisplayedObjects(aList);

            for (AIS_ListIteratorOfListOfInteractive anIt(aList); anIt.More(); anIt.Next())
            {
                Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(anIt.Value());
                if (!aShape.IsNull())
                {
                    Handle(Prs3d_Drawer) aDrawer = aShape->Attributes();
                    aDrawer->SetTransparency(transparency);
                    aShape->SetAttributes(aDrawer);
                }
            }
            myView->Redraw();
        }
    }

    void SetDirectionalLight(const Quantity_Color &color, const Graphic3d_Vec3 &direction)
    {
        if (!myView.IsNull())
        {
            myView->SetLightOn(); // Ensure lighting is enabled

            // Create directional light
            Handle(V3d_DirectionalLight) directionalLight = new V3d_DirectionalLight();
            directionalLight->SetColor(color);
            // directionalLight->SetDirection(direction);

            // Set directional light on the viewer
            myView->Viewer()->SetLightOn(directionalLight);
            myView->Redraw();
        }
    }

private:
    //! Window message handler.
    static LRESULT WINAPI windowProcWrapper(HWND theWnd, UINT theMsg,
                                            WPARAM theParamW, LPARAM theParamL)
    {
        MyViewer *aThis = (MyViewer *)::GetWindowLongPtrW(theWnd, GWLP_USERDATA);
        return aThis != NULL
                   ? aThis->windowProc(theWnd, theMsg, theParamW, theParamL)
                   : ::DefWindowProcW(theWnd, theMsg, theParamW, theParamL);
    }

    //! Window message handler.
    LRESULT WINAPI windowProc(HWND theWnd, UINT theMsg,
                              WPARAM theParamW, LPARAM theParamL)
    {
        switch (theMsg)
        {
        case WM_CLOSE:
        {
            exit(0);
            return 0;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT aPaint;
            ::BeginPaint(theWnd, &aPaint);
            ::EndPaint(theWnd, &aPaint);
            myView->Redraw();
            break;
        }
        case WM_SIZE:
        {
            myView->MustBeResized();
            AIS_ViewController::FlushViewEvents(myContext, myView, true);
            break;
        }
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        {
            const Graphic3d_Vec2i aPos(LOWORD(theParamL), HIWORD(theParamL));
            const Aspect_VKeyFlags aFlags = WNT_Window::MouseKeyFlagsFromEvent(theParamW);
            Aspect_VKeyMouse aButton = Aspect_VKeyMouse_NONE;
            switch (theMsg)
            {
            case WM_LBUTTONUP:
            case WM_LBUTTONDOWN:
                aButton = Aspect_VKeyMouse_LeftButton;
                break;
            case WM_MBUTTONUP:
            case WM_MBUTTONDOWN:
                aButton = Aspect_VKeyMouse_MiddleButton;
                break;
            case WM_RBUTTONUP:
            case WM_RBUTTONDOWN:
                aButton = Aspect_VKeyMouse_RightButton;
                break;
            }
            if (theMsg == WM_LBUTTONDOWN || theMsg == WM_MBUTTONDOWN || theMsg == WM_RBUTTONDOWN)
            {
                ::SetFocus(theWnd);
                ::SetCapture(theWnd);
                AIS_ViewController::PressMouseButton(aPos, aButton, aFlags, false);
            }
            else
            {
                ::ReleaseCapture();
                AIS_ViewController::ReleaseMouseButton(aPos, aButton, aFlags, false);
            }
            AIS_ViewController::FlushViewEvents(myContext, myView, true);
            break;
        }
        case WM_MOUSEMOVE:
        {
            Graphic3d_Vec2i aPos(LOWORD(theParamL), HIWORD(theParamL));
            Aspect_VKeyMouse aButtons = WNT_Window::MouseButtonsFromEvent(theParamW);
            Aspect_VKeyFlags aFlags = WNT_Window::MouseKeyFlagsFromEvent(theParamW);
            CURSORINFO aCursor;
            aCursor.cbSize = sizeof(aCursor);
            if (::GetCursorInfo(&aCursor) != FALSE)
            {
                POINT aCursorPnt = {aCursor.ptScreenPos.x, aCursor.ptScreenPos.y};
                if (::ScreenToClient(theWnd, &aCursorPnt))
                {
                    aPos.SetValues(aCursorPnt.x, aCursorPnt.y);
                    aButtons = WNT_Window::MouseButtonsAsync();
                    aFlags = WNT_Window::MouseKeyFlagsAsync();
                }
            }

            AIS_ViewController::UpdateMousePosition(aPos, aButtons, aFlags, false);
            AIS_ViewController::FlushViewEvents(myContext, myView, true);
            break;
        }
        case WM_MOUSEWHEEL:
        {
            const int aDelta = GET_WHEEL_DELTA_WPARAM(theParamW);
            const double aDeltaF = double(aDelta) / double(WHEEL_DELTA);
            const Aspect_VKeyFlags aFlags = WNT_Window::MouseKeyFlagsFromEvent(theParamW);
            Graphic3d_Vec2i aPos(int(short(LOWORD(theParamL))),
                                 int(short(HIWORD(theParamL))));
            POINT aCursorPnt = {aPos.x(), aPos.y()};
            if (::ScreenToClient(theWnd, &aCursorPnt))
            {
                aPos.SetValues(aCursorPnt.x, aCursorPnt.y);
            }

            AIS_ViewController::UpdateMouseScroll(Aspect_ScrollDelta(aPos, aDeltaF,
                                                                     aFlags));
            AIS_ViewController::FlushViewEvents(myContext, myView, true);
            break;
        }
        default:
        {
            return ::DefWindowProcW(theWnd, theMsg, theParamW, theParamL);
        }
        }
        return 0;
    }

private:
    Handle(AIS_InteractiveContext) myContext;
    Handle(V3d_View) myView;
};

class Timer
{
public:
    Timer() : start_(std::chrono::high_resolution_clock::now()) {}

    void reset()
    {
        start_ = std::chrono::high_resolution_clock::now();
    }

    double elapsed() const
    {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now - start_).count();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

int main()
{
    Graphic3d_TypeOfShadingModel shadings[] = {Graphic3d_TypeOfShadingModel_Phong, Graphic3d_TypeOfShadingModel_Gouraud, Graphic3d_TypeOfShadingModel_Pbr, Graphic3d_TypeOfShadingModel_Unlit};
    Graphic3d_NameOfMaterial materials[] = {Graphic3d_NOM_GLASS, Graphic3d_NOM_PLASTIC, Graphic3d_NOM_SHINY_PLASTIC, Graphic3d_NOM_CHROME, Graphic3d_NOM_TRANSPARENT};

    MyViewer v;
    v.SetShading(Graphic3d_TypeOfShadingModel_Phong);
    v.SetShapeMaterial(Graphic3d_NameOfMaterial_Bronze, 0);
    v.IncreaseEdgeThickness();

    for (;;) // message loop
    {
        MSG aMsg = {};
        if (GetMessageW(&aMsg, NULL, 0, 0) <= 0)
        {
            return 0;
        }
        TranslateMessage(&aMsg);
        DispatchMessageW(&aMsg);
    }
    return 0;
}
