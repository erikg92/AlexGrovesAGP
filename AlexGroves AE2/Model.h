#pragma once
#include "objfilemodel.h"

class Model
{
private:
	ID3D11Device*			m_pD3DDevice;
	ID3D11DeviceContext*	m_pImmediateContext;

	ObjFileModel*			m_pObject;
	ID3D11VertexShader*		m_pVShader;
	ID3D11PixelShader*		m_pPShader;
	ID3D11InputLayout*		m_pInputLayout;
	ID3D11Buffer*			m_pConstantBuffer;

	ID3D11ShaderResourceView* m_pTexture0;
	ID3D11SamplerState* m_pSampler0;

	XMMATRIX transpose;

	XMVECTOR m_directional_light_shines_from;
	XMVECTOR m_directional_light_colour;
	XMVECTOR m_ambient_light_colour;

	float m_x, m_y, m_z;
	float m_xangle, m_yangle, m_zangle;
	float m_scale;
	XMMATRIX world;

public:
	Model(ID3D11Device* device, ID3D11DeviceContext* context);
	~Model();
	int LoadObjModel(char* filename);
	void Draw(XMMATRIX* view, XMMATRIX* projection);
	
	void SetXPos(float);
	void SetYPos(float);
	void SetZPos(float);
	void SetXRotation(float);
	void SetYRotation(float);
	void SetZRotation(float);
	void SetScale(float);

	float GetXPos();
	float GetYPos();
	float GetZPos();
	float GetXRotation();
	float GetYRotation();
	float GetZRotation();
	float GetScale();

	void IncXPos(float);
	void IncYPos(float);
	void IncZPos(float);
	void IncXRotation(float);
	void IncYRotation(float);
	void IncZRotation(float);
	void IncScale(float);
};