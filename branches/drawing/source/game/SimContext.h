//--------------------------------------------------------
// OpenNero : SimContext
//  the context of a simulated world
//--------------------------------------------------------

#ifndef _GAME_SIMCONTEXT_H_
#define _GAME_SIMCONTEXT_H_

#include "core/Common.h"
#include "core/BoostCommon.h"
#include "core/IrrUtil.h"
#include "game/objects/PropertyMap.h"
#include "game/Kernel.h"
#include "game/Simulation.h"
#include "input/IOMapping.h"
#include "render/SceneObject.h"

namespace OpenNero
{
    /// @cond
    BOOST_PTR_DECL( Simulation );
    BOOST_PTR_DECL( SimContext );
    BOOST_PTR_DECL( SimFactory );
    BOOST_PTR_DECL( ObjectTemplate );
    BOOST_PTR_DECL( Camera );
    BOOST_PTR_DECL( GuiManager );
    /// @endcond

    /**
     * The SimContext is the entire game state. It stores all the objects in 
     * in the simulation.
     */
    class SimContext : public enable_shared_from_this<SimContext>
    {
    public:

        /// Who is currently the controller of input
        enum InputReceiver
        {
            kIR_Game,   ///< Game related input
            kIR_Gui     ///< Gui system related input
        };

    public:

        SimContext();
        ~SimContext();

        // -- For IEventReceiver ---

        /// handle an event
        bool HandleEvent( const irr::SEvent& event );

        // -- For SimContext ---

        /// Initialize a simcontext given a device
        bool Initialize(IrrlichtDevice_IPtr device);

        bool onPush(int argc, char** argv);
        bool onPop();

    public:

        /// return the font
		irr::gui::IGUIFont* GetFont();

        /// retun a pointer to the scene manager
		const ISceneManager_IPtr GetSceneManager();

        /// add an object from the specified template with the specified position, rotation and velocity
        SimId AddObject(const std::string& templateName,
                        const Vector3f& pos,
                        const Vector3f& rot = Vector3f(0,0,0),
                        const Vector3f& scale = Vector3f(1,1,1),
                        const std::string& label = std::string(),
                        uint32_t type = 0);

        /// Remove an object by it's ID
        bool RemoveObject( SimId id );

        /// add rgb X-Y-Z axes
        void AddAxes();

        /// set the fog mode
        void SetFog();

        /// Get the SimEntity under the user specified cursor position on screen
        SimEntityPtr GetEntityUnderMouse(const int32_t& x, const int32_t& y) const;

        /// Get the Id of the SimEntity under the user specified cursor position on screen
        SimId GetEntityIdUnderMouse(const int32_t& x, const int32_t& y) const;

        /// Get the vector from camera origin to the specified cursor position on screen
        Line3f GetRayUnderMouse(const int32_t& x, const int32_t& y) const;

#if NERO_BUILD_PHYSICS
        /// Find all objects in a sphere
        SimDataVector FindInSphere( const Vector3f& origin, F32 radius ) const;
#endif // NERO_BUILD_PHYSICS

		/// Find the first object that intersects the specified ray
		boost::python::tuple FindInRay( const Vector3f& origin, const Vector3f& target, const uint32_t& type = 0, const bool val = false) const;

        /// Get (approximate) 3d position of the click
        Vector3f GetClickPosition(const int32_t& x, const int32_t& y) const;

        /// Find the collision point of the specified ray and the object
        bool GetCollisionPoint(const Vector3f& start, const Vector3f& end, SimId id, Vector3f& outCollisionPoint) const;

        /// Get the point of intersection of the vector from camera origin to the specified cursor position on screen
        Vector3f GetPointUnderMouse(const int32_t& x, const int32_t& y) const;

        /// Get the current position of the mouse on the screen
        Pos2i GetMousePosition() const;

        /// These methods call the corresponding methods of SimEntity specified by the id
        /// @{
        void SetObjectPosition( SimId id, const Vector3f& pos );
        void SetObjectRotation( SimId id, const Vector3f& rot );
        void SetObjectScale( SimId id, const Vector3f& scale );
        void SetObjectLabel( SimId id, const std::string& label );
        void SetObjectColor( SimId id, const SColor& color );
		bool SetObjectAnimation( SimId id, const std::string& animation_type );
        /// @}

        /// Get the position of the SimEntity specified by the id
        Vector3f GetObjectPosition( SimId id ) const;

        /// Get the rotation of the SimEntity specified by the id
        Vector3f GetObjectRotation( SimId id ) const;

        /// Get the scale of the SimEntity specified by the id
        Vector3f GetObjectScale( SimId id ) const;

        /// Get label of the SimEntity specified by the id
        std::string GetObjectLabel( SimId id ) const;

        /// Get override color of the SimEntity specified by the id
        SColor GetObjectColor( SimId id ) const;

        /// End the game
        void KillGame();

        /// Add a camera to the world
        CameraPtr AddCamera( float32_t rotateSpeed, float32_t moveSpeed, float32_t zoomSpeed );

        /// Add a light source to the scene
        void AddLightSource( const Vector3f& position, float32_t radius, const SColor& color = SColor(0xFF, 0xFF, 0xFF, 0xFF) );

        /// Add a skybox to the world
        void AddSkyBox( const std::string& box_base_name, const std::string& extension = "jpg" );

        /// Return the GUI manager
        GuiManagerPtr GetGuiManager();

        /// Define proper mapping for inputs
        void SetInputMapping( const PyIOMap& ioMap );

        // --- end scripting interface methods

        /// return the active camera
        CameraPtr getActiveCamera() const;

        // get an object template from a file
        template<typename ObjTemp> shared_ptr<ObjTemp> getObjectTemplate( const std::string& templateName) const;

        // move the world forward by time dt
        void ProcessTick(float32_t dt);

        /// return the simulation
        SimulationPtr getSimulation() { return mpSimulation; }
        /// return the simulation while changing nothing
        const SimulationPtr getSimulation() const { return mpSimulation; }

        /// return the next free Id
        SimId GetNextFreeId() const;

    protected:

        // update the various systems

        /// update the audio system
        void UpdateAudioSystem(float32_t dt);
        void UpdateInputSystem(float32_t dt);
        void UpdateRenderSystem(float32_t dt);
        void UpdateScriptingSystem(float32_t dt);
        void UpdateSimulation(float32_t dt);
    private:

        // clear out the context
        void FlushContext();

    protected:

        /// maps strings to object templates
        typedef std::map< std::string, ObjectTemplatePtr >ObjectTemplateMap;

    private:

        IrrHandles mIrr;                                ///< Handles to irrlicht things

        CameraPtr mpCamera;                             ///< Our camera in the scene

        ISceneNode_IPtr mpSkyBox;                       ///< Sky box for background of the scene

        GuiManagerPtr mpGuiManager;                     ///< Manager of our gui pieces

        SimulationPtr mpSimulation;                     ///< The simulation
        SimFactoryPtr mpFactory;                        ///< factory for loading "things"
        mutable ObjectTemplateMap mObjectTemplates;     ///< Object templates in use

        SColor              mClearColor;                ///< Screen Clear Color

        PyIOMap             mIOMap;                     ///< The input map for this context
        InputReceiver       mInputReceiver;             ///< The current input receiver
    };

    /**
     * Get an object template either from our cache or from the given file
     * @param templateName the name of the file to get the template from
     * @return ptr to the loaded object template or NULL if failed to load
     */
    template<typename ObjTemp> shared_ptr<ObjTemp> SimContext::getObjectTemplate(
            const std::string& templateName) const
    {
        // make the mod path
        std::string modTemplateName = Kernel::findResource(templateName);

        // make the lookup path
        std::string lookupPath = ObjTemp::TemplateType() + "_"+ modTemplateName;

        // do we already have this?
        ObjectTemplateMap::const_iterator
                itr = mObjectTemplates.find(lookupPath);
        if (itr != mObjectTemplates.end() )
        {
            return shared_static_cast<ObjTemp, ObjectTemplate>(itr->second);
        }

        LOG_F_MSG( "game", "Loading object template " << modTemplateName );

        // we need to add it
        PropertyMap pmap;
        if (pmap.constructPropertyMap(modTemplateName ) )
        {
            shared_ptr<ObjTemp> temp = ObjTemp::createTemplate( mpFactory, pmap ); // allows some degree of polymorphism
            mObjectTemplates[lookupPath] = temp;
            LOG_F_MSG( "game", "Successfully loaded object template " << modTemplateName );
            return temp;
        }

        // fail
        LOG_F_ERROR("game", "Failed to load object template " << modTemplateName );
        return shared_ptr<ObjTemp>();
    }

} //end OpenNero

#endif // _GAME_SIMCONTEXT_H_
