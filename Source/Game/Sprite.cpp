#include <Pxf/Game/Sprite.h>
#include <Pxf/Graphics/Texture.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Util/String.h>
#include <stdio.h>
#include <vector.h>

using namespace Pxf;
using namespace Game;

unsigned Sprite::m_SpriteCounter = 0;

// TODO: The sprite class should take care of creating sequencing
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

sprite_sequence::~sprite_sequence()
{
	delete[] sequence;
	sequence = 0;
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
	{
		m_UseCustomSequence = true;
		m_MaxFrames			= m_CustomSequence->size;
	}
	else
	{
		// calculate max cells
		// TODO: what do we do about uneven cell numbers? exessive space is truncated atm
		int _ImgWidth = m_Texture->GetWidth();
		int _ImgHeight = m_Texture->GetHeight();
		
		int _WAmount = (_ImgWidth / _CellWidth);
		int _HAmount = (_ImgHeight / _CellHeight);
		
		m_MaxFrames = _WAmount * _HAmount;
	}
		

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

void Sprite::Update()
{
	// check if we need to update
	
	switch(m_SpriteState)
	{
		case Paused:
			break;
		case Stopped:
			break;
		case Running:
			break;
		default:
			break;
	}
	
	/*
	 const float time_step = 60 / frequency;
	 
	 switch_frame += dt;
	 if(switch_frame >= time_step)
	 {
		NextFrame();
		
		// reset
		switch_frame = 0.0f;
	 }
	*/
}

void Sprite::Draw()
{
	// sprite process:
	
	// bind texture
	m_Device->BindTexture(m_Texture);
	
	// draw sprite
}

/* 
 
 Sprite controllers
 
 */

void Sprite::Reset()
{
	m_CurrentFrame = 0;
	m_SpriteState = Running;
	
}

void Sprite::Pause()
{
	m_SpriteState = Paused;
}

void Sprite::Stop()
{
	m_SpriteState = Stopped;
}

void Sprite::Start()
{
	m_SpriteState = Running;
}

void Sprite::NextFrame()
{
	if(m_CurrentFrame >= m_MaxFrames)
		m_CurrentFrame = 0;
	else
		m_CurrentFrame++;
}