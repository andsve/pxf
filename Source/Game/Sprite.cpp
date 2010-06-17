#include <Pxf/Game/Sprite.h>
#include <Pxf/Graphics/Texture.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Util/String.h>
#include <stdio.h>
#include <vector.h>

using namespace Pxf;
using namespace Game;

unsigned Sprite::m_SpriteCounter = 0;

sprite_sequence::sprite_sequence(int nbr_of_args, ... )
{
	int					new_val,i;
	int*				_array = NULL;
	va_list				vl;
	std::vector<int>	vec;
	
	va_start(vl,nbr_of_args);
	
	for(i = 0; i < nbr_of_args; i++)
	{
		new_val = va_arg(vl,int);
		vec.push_back(new_val);
	}
	
	va_end(vl);
	
	_array = new int [vec.size()];
	memcpy(_array, &vec.front(), vec.size() * sizeof(int));
	
	valid		= true;
	sequence	= _array;
	size		= nbr_of_args;
}

Sprite::Sprite(Graphics::Device* _pDevice, const char* _ID, Graphics::Texture* _Texture, int _CellWidth, int _CellHeight, int _Frequency,int _ZIndex, sprite_sequence* _CustomSequence)
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

void Sprite::Reset()
{
	m_CurrentFrame = 0;
	m_SpriteState = Running;
	
}


