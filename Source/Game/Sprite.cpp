#include <Pxf/Base/Debug.h>
#include <Pxf/Game/Sprite.h>
#include <Pxf/Graphics/Texture.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/VertexBuffer.h>
#include <Pxf/Util/String.h>
#include <stdio.h>
#include <vector.h>

#define LOCAL_MSG "Sprite"

using namespace Pxf;
using namespace Game;
using namespace Math;

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

Sprite::Sprite(Graphics::Device* _pDevice, Math::Vector2D<float> _Position, const char* _ID, Graphics::Texture* _Texture, int _CellWidth, int _CellHeight, int _Frequency,int _ZIndex)
	: GameObject(Math::Vector3D<float>(_Position.x,_Position.y,0.0f),_ID) 
	, m_Device(_pDevice),
	  m_Texture(_Texture),
	  m_Frequency(_Frequency),
	  m_CurrentFrame(0),
	  m_ZIndex(_ZIndex),
	  m_MaxFrames(0),
	  m_Ready(true),
	  m_DrawBuffer(0)
	, m_UseCustomSequence(false)
{	
	if(!m_Device)
	{
		m_Ready = false;
		Pxf::Message(LOCAL_MSG,"trying to create %s with Invalid Device", m_ID);
	}
	else
	{
		// Create vertex buffer
		m_DrawBuffer = m_Device->CreateVertexBuffer(Graphics::VB_LOCATION_GPU,Graphics::VB_USAGE_STATIC_DRAW);
		m_DrawBuffer->CreateNewBuffer(4,sizeof(Math::Vector3D<float>) + sizeof(Math::Vector2D<float>));
		m_DrawBuffer->SetData(Graphics::VB_VERTEX_DATA,0,3);
		m_DrawBuffer->SetData(Graphics::VB_TEXCOORD_DATA,sizeof(Math::Vector3D<float>),2);
		m_DrawBuffer->SetPrimitive(Graphics::VB_PRIMITIVE_TRIANGLE_STRIP);
	
		SpriteDrawData _Data[4];
		/*
		_Data[0] = SpriteDrawData(Vector3D<float>(-0.5f,-0.5f,0.5f),Vector2D<float>(0.0f,1.0f));
		_Data[1] = SpriteDrawData(Vector3D<float>(0.5f,-0.5f,0.5f),Vector2D<float>(1.0f,1.0f));
		_Data[2] = SpriteDrawData(Vector3D<float>(-0.5f,0.5f,0.5f),Vector2D<float>(0.0f,0.0f));
		_Data[3] = SpriteDrawData(Vector3D<float>(0.5f,0.5f,0.5f),Vector2D<float>(1.0f,0.0f)); */
		
		_Data[0] = SpriteDrawData(Vec3f(0.f,0.f,0.f),Vector2D<float>(0.0f,1.0f));
		_Data[1] = SpriteDrawData(Vec3f(1.0f,0.f,0.f),Vector2D<float>(1.0f,1.0f));
		_Data[2] = SpriteDrawData(Vec3f(0.f,1.0f,0.f),Vector2D<float>(0.0f,0.0f));
		_Data[3] = SpriteDrawData(Vec3f(1.f,1.f,0.f),Vector2D<float>(1.0f,0.0f));

		m_DrawBuffer->UpdateData(_Data,sizeof(_Data),0);
		m_MappedData = (SpriteDrawData*)m_DrawBuffer->MapData(Graphics::VB_ACCESS_WRITE_ONLY);
	}
											  
	if(!m_Texture)
	{
		m_Ready = false;
		Pxf::Message(LOCAL_MSG,"creating %s with Invalid Texture", m_ID);
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
		Pxf::Message(LOCAL_MSG,"%s Ready", m_ID);
	
	_CalculateUV();
	_SetCurrentUV();
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
		NextFrame();
		
		// reset
		switch_frame = 0.0f;
	 }
	*/
}

void Sprite::Draw()
{	
	// bind texture
	Vec3f _Position = GetPosition();
	m_Device->Translate(_Position);
	m_Device->BindTexture(m_Texture);
	
	NextFrame();
	
	// draw sprite
	m_Device->DrawBuffer(m_DrawBuffer);
	m_Device->Translate(-(_Position));
}

void Sprite::_SetCurrentUV()
{	
	// 0,1
	m_MappedData[0].tex_coords.x = m_UVValues[m_CurrentFrame][0];
	m_MappedData[0].tex_coords.y = m_UVValues[m_CurrentFrame][3];
	
	// 1,1
	m_MappedData[1].tex_coords.x = m_UVValues[m_CurrentFrame][2];
	m_MappedData[1].tex_coords.y = m_UVValues[m_CurrentFrame][3];	

	// 0,0
	m_MappedData[2].tex_coords.x = m_UVValues[m_CurrentFrame][0];
	m_MappedData[2].tex_coords.y = m_UVValues[m_CurrentFrame][1];
	
	// 1,0
	m_MappedData[3].tex_coords.x = m_UVValues[m_CurrentFrame][2];
	m_MappedData[3].tex_coords.y = m_UVValues[m_CurrentFrame][1];
}	

// Calculate every UV ONCE and store in an array
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
		/*
		if(m_CurrentFrame >= m_CustomSequence->size)
			m_CurrentFrame = 0;
		else
			m_CurrentFrame++;
		 */
	}
	else
	{
		// Wrap Around in linear mode
		if(m_CurrentFrame >= (m_MaxFrames-1))
			m_CurrentFrame = 0;
		else
			m_CurrentFrame++;
	}
	
	_SetCurrentUV();
}