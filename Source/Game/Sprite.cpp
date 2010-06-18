#include <Pxf/Game/Sprite.h>
#include <Pxf/Graphics/Texture.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Util/String.h>
#include <stdio.h>
#include <vector.h>

using namespace Pxf;
using namespace Game;

unsigned Sprite::m_SpriteCounter = 0;

sprite_sequence::~sprite_sequence()
{
	delete[] sequence;
	sequence = 0;
}

void Sprite::AddSequence(int _SequenceLength, ...)
{
	sprite_sequence new_sequence;
	
	int					new_val,i;
	int*				_array = NULL;
	va_list				vl;
	std::vector<int>	vec;
	
	va_start(vl,_SequenceLength);
	
	for(i = 0; i < _SequenceLength; i++)
	{
		new_val = va_arg(vl,int);
		vec.push_back(new_val);
	}
	
	va_end(vl);
	
	_array = new int [vec.size()];
	memcpy(_array, &vec.front(), vec.size() * sizeof(int));
	
	new_sequence.valid		= true;
	new_sequence.sequence	= _array;
	new_sequence.size		= _SequenceLength;
	
	m_SequenceList.push_back(new_sequence);
}

Sprite::Sprite(Graphics::Device* _pDevice, const char* _ID, Graphics::Texture* _Texture, int _CellWidth, int _CellHeight, int _Frequency,int _ZIndex)
	: m_Device(_pDevice),
	  m_ID(_ID),
	  m_Texture(_Texture),
	  m_Frequency(_Frequency),
	  m_CurrentFrame(0),
	  m_ZIndex(_ZIndex),
	  m_MaxFrames(0),
	  m_Ready(true),
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
	
	m_CellSize[0] = _CellWidth;
	m_CellSize[1] = _CellHeight;

	// calculate max cells
	// TODO: what do we do about uneven cell numbers? exessive space is truncated atm
	int _ImgWidth = m_Texture->GetWidth();
	int _ImgHeight = m_Texture->GetHeight();
	
	int _WAmount = (_ImgWidth / _CellWidth);
	int _HAmount = (_ImgHeight / _CellHeight);
	
	m_UVStep[0] = 1.0f / _WAmount;
	m_UVStep[1] = 1.0f / _HAmount;
	
	m_MaxFrames = _WAmount * _HAmount;
	
	m_UVValues = new float[m_MaxFrames][4];

	if(m_Ready)
		printf("Sprite %s: Ready\n", m_ID);
	
	_CalculateUV();
		
}

Sprite::~Sprite()
{
	// do we need to delete device / texture?
	m_Device = 0;
	m_Texture = 0;
	
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
		_CalculateUV();
		NextFrame();
		
		// reset
		switch_frame = 0.0f;
	 }
	*/
}

void Sprite::Draw()
{	
	// bind texture
	m_Device->BindTexture(m_Texture);
	
	// draw sprite
}


/* 
 NOTE: is it better to fill a 2-dimensional grid with UV values when loading the sprite,
	   or is it ok to calculate these values every time we switch image?
 */

void Sprite::_CalculateUV()
{	
	// calculate cell position in image
	int _CellsX = (float) (m_Texture->GetWidth() / m_CellSize[0]);
	int _CellsY = (float) (m_Texture->GetHeight() / m_CellSize[1]);
	
	int _X,_Y;
	
	for(int i = 0; i < m_MaxFrames; i++)
	{
		_X = i % _CellsX;
		_Y = i / _CellsY;
		
		m_UVValues[i][0] = _X * m_UVStep[0];
		m_UVValues[i][1] = _Y * m_UVStep[1];
		m_UVValues[i][2] = m_UVValues[i][0] + m_UVStep[0];
		m_UVValues[i][3] = m_UVValues[i][1] + m_UVStep[1];
	}
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
	m_CurrentFrame = 0;
	m_SpriteState = Stopped;
}

void Sprite::Start()
{
	m_SpriteState = Running;
}

void Sprite::NextFrame()
{
	if(m_UseCustomSequence)
	{
		// TODO: remove custom sequence and use a stack or something else instead
		/*
		if(m_CurrentFrame >= m_CustomSequence->size)
			m_CurrentFrame = 0;
		else
			m_CurrentFrame++;
		 */
	}
	else
	{
		if(m_CurrentFrame >= m_MaxFrames)
			m_CurrentFrame = 0;
		else
			m_CurrentFrame++;
	}
}