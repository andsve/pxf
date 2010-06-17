#include <Pxf/Game/Sprite.h>
#include <Pxf/Graphics/Texture.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Util/String.h>
#include <stdio.h>

using namespace Pxf;
using namespace Game;

unsigned Sprite::m_SpriteCounter = 0;

Sprite::Sprite(Graphics::Device* _pDevice, const char* _ID, const char* _Filepath, int _CellWidth, int _CellHeight, int _Frequency, int _ZIndex, int* _CustomSequence)
{
	Sprite(_pDevice,_ID,_pDevice->CreateTexture(_Filepath),_CellWidth,_CellHeight,_Frequency,_ZIndex,_CustomSequence);

}

Sprite::Sprite(Graphics::Device* _pDevice, const char* _ID, Graphics::Texture* _Texture, int _CellWidth, int _CellHeight, int _Frequency,int _ZIndex, int* _CustomSequence)
	: m_Device(_pDevice),
	  m_ID(_ID),
	  m_Texture(_Texture),
	  m_CellSize(_CellWidth,_CellHeight),
	  m_Frequency(_Frequency),
	  m_CurrentFrame(0),
	  m_ZIndex(_ZIndex),
	  m_MaxFrames(0),
	  m_Ready(true),
	  m_CustomSequence(_CustomSequence),
	  m_UseCustomSequence(false)
{
	if(!m_ID)
	{
		char _NewName[32];
		sprintf(_NewName,"Sprite%i",m_SpriteCounter);

		std::string * _StrName = new std::string(_NewName);
		m_ID = _StrName->c_str();
	}

	m_SpriteCounter++;

	if(!m_Device)
	{
		m_Ready = false;
		printf("Sprite %s: Invalid Device\n", m_ID);
	}

	if(!m_Texture)
	{
		m_Ready = false;
		printf("Sprite %s: Invalid Texture\n", m_ID);
	}

	if(_CustomSequence)
		m_UseCustomSequence = true;

	if(m_Ready)
		printf("Sprite %s: Ready\n", m_ID);
}

Sprite::~Sprite()
{
	// do we need to delete device / texture? they might be used by other objects
	m_Device = 0;
	m_Texture = 0;
	
	
	delete m_CustomSequence;
	delete m_ID;
	
}


