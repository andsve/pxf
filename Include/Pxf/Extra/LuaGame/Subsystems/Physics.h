#ifndef __PXF_EXTRA_LUAGAME_SUBSYSTEMS_PHYSICS_H__
#define __PXF_EXTRA_LUAGAME_SUBSYSTEMS_PHYSICS_H__

#include <Pxf/Extra/LuaGame/Subsystem.h>
#include <Pxf/Math/Vector.h>

// box2D forward declarations
class b2World;
class b2Body;

namespace Pxf
{
    namespace Extra
    {
        namespace LuaGame
        {
            namespace PhysicsSubsystem
            {
                void RegisterClass(lua_State* _L);
                int NewWorld(lua_State* _L);

				// helpers
				int _NewBox2DWorld(lua_State* _L);
				int _NewBullet3DWorld(lua_State* _L);

				class LuaPhysicsBody : public Subsystem
				{
				public:
					//virtual void RegisterClass(lua_State* _L) = 0;
					virtual int GetPosition(lua_State* _L) = 0; 
				private:
				};

				class LuaBox2DPhysicsBody : public LuaPhysicsBody
				{
				public:
					int GetPosition(lua_State* _L);
				private:
					b2Body* m_Body;
				};

				class LuaPhysicsWorld : Subsystem
				{
				public:
					LuaPhysicsWorld(Math::Vec3f _Gravity) { 
						m_Gravity = _Gravity;
					}

					void RegisterClass(lua_State* _L);

					virtual int NewBody(lua_State* _L) = 0;
					virtual int GetType() = 0;

					Math::Vec3f GetGravity() { return m_Gravity; }
				private:
					Math::Vec3f m_Gravity;
				};

				enum LuaPhysicsWorldType {
					WORLD_TYPE_BULLET3D = 1,
					WORLD_TYPE_BOX2D = 2
				};

				class LuaBox2DPhysicsWorld : public LuaPhysicsWorld
				{
				public:
					LuaBox2DPhysicsWorld(Math::Vec3f _Gravity) 
						: LuaPhysicsWorld(_Gravity) { }
					int NewBody(lua_State* _L);
					int GetType() { return WORLD_TYPE_BOX2D; }
				private:
					b2World* m_World;
				};
            } /* PhysicsSubsystem */
        } /* LuaGame */
    } /* Extra */
} /* Pxf */



#endif // __PXF_EXTRA_LUAGAME_SUBSYSTEMS_GRAPHICS_H__
