#include "Model.h"

struct MODEL_CONSTANT_BUFFER
{
	XMMATRIX WorldVIewProjection;//64 bytes
	XMVECTOR directional_light_vector;	//16 bytes
	XMVECTOR directional_light_colour;	//16 bytes
	XMVECTOR ambient_light_colour;		//16 bytes
	XMFLOAT4 packing;					//16 bytes
};

Model::Model(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_pD3DDevice = device;
	m_pImmediateContext = context;

	m_x = 0.0f;
	m_y = 0.0f;
	m_z = 0.0f;
	m_xangle = 0.0f;
	m_yangle = 0.0f;
	m_zangle = 0.0f;
	m_scale = 1.0f;

}

Model::~Model()
{
	delete(m_pObject);
	if (m_pTexture0) m_pTexture0->Release();
	if (m_pSampler0) m_pSampler0->Release();
	if (m_pConstantBuffer) m_pConstantBuffer->Release();
	if (m_pInputLayout) m_pInputLayout->Release();
	if (m_pVShader) m_pVShader->Release();
	if (m_pPShader) m_pPShader->Release();
	if (m_pImmediateContext) m_pImmediateContext = NULL;//->Release();
	if (m_pD3DDevice) m_pD3DDevice = NULL;//->Release();

}

 int Model::LoadObjModel(char* filename)
{
	

	 m_pObject = new ObjFileModel(filename, m_pD3DDevice, m_pImmediateContext);

	 HRESULT hr = S_OK;


	 if (m_pObject->filename == "FILE NOT LOADED")
	 {
		 return S_FALSE;
	 }

	 //create constant buffer
	 D3D11_BUFFER_DESC constant_buffer_desc;
	 ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	 constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;	//can use UpdateSubresource() to update
	 constant_buffer_desc.ByteWidth = 128;	//MUST be a multiple of 16, calculate from CB struct
	 constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	//Use as a constant buffer

	 hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer);

	 if (FAILED(hr))
	 {
		 return hr;
	 }

	 D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice, "assets/goku.bmp", NULL, NULL, &m_pTexture0, NULL);

	 D3D11_SAMPLER_DESC sampler_desc;
	 ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	 sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	 sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	 sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	 sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	 sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	 m_pD3DDevice->CreateSamplerState(&sampler_desc, &m_pSampler0);

	 ID3DBlob *VS, *PS, *error;
	 hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelVS", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	 if (error != 0) //check for shader compilation error
	 {
		 OutputDebugStringA((char*)error->GetBufferPointer());
		 error->Release();
		 if (FAILED(hr)) //don't fail if error is just a warning
		 {
			 return hr;
		 };
	 }

	 hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelPS", "ps_4_0", 0, 0, 0, &PS, &error, 0);

	 if (error != 0)// check for shader compilation error
	 {
		 OutputDebugStringA((char*)error->GetBufferPointer());
		 error->Release();
		 if (FAILED(hr))
		 {
			 return hr;
		 };
	 }



	 //Create shader objects
	 hr = m_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_pVShader);

	 if (FAILED(hr))
	 {
		 return hr;
	 }

	 hr = m_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_pPShader);

	 if (FAILED(hr))
	 {
		 return hr;
	 }



	 m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	 m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);

	 //create and set the input layoutobject
	 D3D11_INPUT_ELEMENT_DESC iedesc[] =
	 {
		 { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0 , 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		 { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		 { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	 };

	 hr = m_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_pInputLayout);

	 if (FAILED(hr))
	 {
		 return hr;
	 }

	 m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	 return S_OK;
}

 void Model::Draw(XMMATRIX* view, XMMATRIX* projection)
 {
	
	 MODEL_CONSTANT_BUFFER model_cb_values;

	 m_directional_light_shines_from = XMVectorSet(0.0f, 5.0f, -1.0f, 0.0f);
	 m_directional_light_colour = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	 m_ambient_light_colour = XMVectorSet(0.2f, 0.1f, 0.1f, 1.0f);

	 transpose = XMMatrixTranspose(world);

	 model_cb_values.directional_light_colour = m_directional_light_colour;
	 model_cb_values.ambient_light_colour = m_ambient_light_colour;
	 model_cb_values.directional_light_vector = XMVector3Transform(m_directional_light_shines_from, transpose);
	 model_cb_values.directional_light_vector = XMVector3Normalize(model_cb_values.directional_light_vector);

	 world = XMMatrixRotationX(XMConvertToRadians(m_xangle));
	 world *= XMMatrixRotationY(XMConvertToRadians(m_yangle));
	 world *= XMMatrixRotationZ(XMConvertToRadians(m_zangle));
	 world *= XMMatrixScaling(m_scale, m_scale, m_scale);
	 world *= XMMatrixTranslation(m_x, m_y, m_z);


	
	 model_cb_values.WorldVIewProjection = world * (*view) * (*projection);


	 m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	 m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);
	 m_pImmediateContext->IASetInputLayout(m_pInputLayout);


	 
	 m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	 m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler0);
	 m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture0);

	 m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &model_cb_values, 0, 0);

	 m_pObject->Draw();

 }

 void Model::SetXPos(float x)
 {
	 m_x = x;
 }

 void Model::SetYPos(float y)
 {
	 m_y = y;
 }

 void Model::SetZPos(float z)
 {
	 m_z = z;
 }

 void Model::SetXRotation(float angle)
 {
	 m_xangle = angle;
 }

 void Model::SetYRotation(float angle)
 {
	 m_yangle = angle;
 }

 void Model::SetZRotation(float angle)
 {
	 m_zangle = angle;
 }

 void Model::SetScale(float scale)
 {
	 m_scale = scale;
 }

 float Model::GetXPos()
 {
	 return m_x;
 }

 float Model::GetYPos()
 {
	 return m_y;
 }

 float Model::GetZPos()
 {
	 return m_z;
 }

 float Model::GetXRotation()
 {
	 return m_xangle;
 }

 float Model::GetYRotation()
 {
	 return m_yangle;
 }

 float Model::GetZRotation()
 {
	 return m_zangle;
 }

 float Model::GetScale()
 {
	 return m_scale;
 }

 void Model::IncXPos(float step)
 {
	 m_x = m_x + step;
 }

 void Model::IncYPos(float step)
 {
	 m_y = m_y + step;
 }

 void Model::IncZPos(float step)
 {
	 m_z = m_z + step;
 }

 void Model::IncXRotation(float step)
 {
	 m_xangle = m_xangle + step;
 }

 void Model::IncYRotation(float step)
 {
	 m_yangle = m_yangle + step;
 }

 void Model::IncZRotation(float step)
 {
	 m_zangle = m_zangle + step;
 }

 void Model::IncScale(float step)
 {
	 m_scale = m_scale + step;
 }

