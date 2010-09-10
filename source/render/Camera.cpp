//--------------------------------------------------------
// OpenNero : Camera
//  A camera wrapper
//--------------------------------------------------------

#include "core/Common.h"
#include "Camera.h"
#include "game/SimContext.h"
#include "game/Simulation.h"
#include "game/SimEntity.h"
#include "scripting/scriptIncludes.h"

namespace OpenNero 
{
    /**
     * CTOR
     * @param handles irrlicht handles to aid in the construction
     * @param rotateSpeed how fast the camera will pitch & yaw
     * @param moveSpeed how fast the camera will translate
     * @param zoomSpeed how fast the camera will zoom
    */
    Camera::Camera( const IrrHandles& handles, float32_t rotateSpeed, float32_t moveSpeed, float32_t zoomSpeed ) : 
        mIrr(handles),
        mRotateSpeed(rotateSpeed),
        mMoveSpeed(moveSpeed),
        mZoomSpeed(zoomSpeed),
        mCamera()        
    {
        Assert( handles.mpSceneManager ); 

        // create our functionality cameras
        mFuncCameras[ kFunc_Maya ] = ICameraSceneNode_IPtr( handles.mpSceneManager->addCameraSceneNodeMaya( 0, rotateSpeed, zoomSpeed, moveSpeed ) );
        mFuncCameras[ kFunc_Nero ] = ICameraSceneNode_IPtr( handles.mpSceneManager->addCameraSceneNodeNero( 0, true, 0.05f, rotateSpeed, moveSpeed, zoomSpeed ) );

        // start out with the NERO camera
        mCamera = mFuncCameras[ kFunc_Nero ];

        Assert( mCamera );
        Assert( mFuncCameras[ kFunc_Maya ] );
        Assert( mFuncCameras[ kFunc_Nero ] );
    }    

    /// Set the position of the camera
    /// @param pos the position to go to
    void Camera::setPosition( const Vector3f& pos )
    {
        Assert( mCamera );
        mCamera->setPosition( ConvertNeroToIrrlichtPosition(pos) );
    }

    /// Retrieve the position of the camera
    Vector3f Camera::getPosition() const
    {
        Assert( mCamera );
        return ConvertIrrlichtToNeroPosition(mCamera->getAbsolutePosition());
    }

    Vector3f Camera::getTarget() const
    {
        Assert( mCamera );
        return ConvertIrrlichtToNeroPosition(mCamera->getTarget());
    }

    Vector3f Camera::getUp() const
    {
        Assert( mCamera );
        return ConvertIrrlichtToNeroPosition(mCamera->getUpVector());
    }

    /// Set whether or not we should edge scroll
    /// @param enable new edge scroll status
    void Camera::setEdgeScroll( bool enable )
    {
        Assert( mCamera );
    }
     
    /// Set what the camera is looking at
    /// @param target the point the camera should look at
    void Camera::setTarget( const Vector3f& target )
    {
        Assert( mCamera );
        mCamera->setTarget( ConvertNeroToIrrlichtPosition(target) );
    }

    /// Set the rotation of the camera
    /// @param rotation the Euler rotation angle (in right-handed degrees)
    void Camera::setRotation( const Vector3f& rotation )
    {
        Assert( mCamera );
        mCamera->setRotation( ConvertNeroToIrrlichtRotation(rotation) );
    }
     
    /// Set the far clipping plane for the projection matrix
    /// @param farPlane the distance from the near plane that the far clipping plane should be placed
    void Camera::setFarPlane( F32 farPlane )
    {
        Assert( mCamera );
        mCamera->setFarValue(farPlane);        
    }     

    /// Gets the current functionality of the camera
    /// @return the current functionality
    Camera::Functionality Camera::getFunctionality() const
    {
        if( mCamera == mFuncCameras[ kFunc_Maya ] )
            return kFunc_Maya;
        else if  (mCamera == mFuncCameras[ kFunc_Nero ])
            return kFunc_Nero;
        else
            return kFunc_FPS;
    }

    /// Set the camera to a new functionality
    /// @param f the new functionality
    void Camera::setFunctionality( Functionality f )
    {
        if( f != getFunctionality() )
        {   
            Assert( mIrr.mpSceneManager );            

            mFuncCameras[f]->setPosition(   mCamera->getAbsolutePosition() );
            mFuncCameras[f]->setTarget(     mCamera->getTarget() );
            mFuncCameras[f]->setFarValue(   mCamera->getFarValue() );
            mFuncCameras[f]->setNearValue(  mCamera->getNearValue() );
            mCamera = mFuncCameras[f];

            mIrr.mpSceneManager->setActiveCamera( mCamera.get() );            
        }
    }

    bool Camera::attach( SimEntityPtr entity )
    {
        Assert(entity);
        SceneObjectPtr scene_object = entity->GetSceneObject();
        if (scene_object && scene_object->mSceneNode)
        {
            mCamera = ICameraSceneNode_IPtr( mIrr.mpSceneManager->addCameraSceneNodeNeroFP(scene_object->mSceneNode));
            scene_object->attachCamera(shared_from_this());
        }
        else
        {
            LOG_F_ERROR("render", "Unable to attach to simulation entity: " << entity);
            return false;
        }
        return true;
    }

	void Camera::setNearPlane( F32 nearPlane )
	{
        Assert(mCamera);
		mCamera->setNearValue(nearPlane);
	}

    OpenNero::Vector3f Camera::getRotation() const
    {
        Assert(mCamera);
        return ConvertIrrlichtToNeroRotation(mCamera->getRotation());
    }

    F32 Camera::getNearPlane() const
    {
        Assert(mCamera);
        return mCamera->getNearValue();
    }

    F32 Camera::getFarPlane() const
    {
        Assert(mCamera);
        return mCamera->getFarValue();
    }


    /// export camera API to script
    PYTHON_BINDER(Camera)
    {
        class_<Camera,CameraPtr>("Camera", "A camera in a sim context", no_init)
            .def("setPosition",     &Camera::setPosition, "Set the current position of the camera" )
            .def("setRotation",     &Camera::setRotation, "Set the euler rotation angles of the camera" )
            .def("setTarget",       &Camera::setTarget, "Set the point the camera is looking at" )
            .def("setFarPlane",     &Camera::setFarPlane, "Set the far plane of the perspective projection of the camera" )
            .def("setEdgeScroll",   &Camera::setEdgeScroll, "Set whether or not the camera edge scrolls" )
            .def("attach",          &Camera::attach, "Attach to sim object by id" )
            ;
    }

    std::ostream& operator<<( std::ostream& output, const CameraPtr camera )
    {
        switch (camera->getFunctionality())
        {
        case Camera::kFunc_FPS:
            output << "<first-person-camera ";
            break;
        case Camera::kFunc_Maya:
            output << "<maya-camera ";
            break;
        case Camera::kFunc_Nero:
            output << "<nero-camera ";
            break;
        default:
            output << "<camera ";
        }
        output << " position=\"" << camera->getPosition();
        output << "\" rotation=\"" << camera->getRotation();
        output << "\" target=\"" << camera->getTarget();
        output << "\" nearPlane=\"" << camera->getNearPlane();
        output << "\" farPlane=\"" << camera->getFarPlane();
        output << "\" moveSpeed=\"" << camera->mMoveSpeed;
        output << "\" rotateSpeed=\"" << camera->mRotateSpeed;
        output << "\" zoomSpeed=\"" << camera->mZoomSpeed;
        output << "\" />";
        return output;
    }
} //end OpenNero
