#include <d3d11.h>
#include "camera.h"
#include <math.h>

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>

Camera::Camera(int x, int y, int z, int camera_rotation)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_camera_rotation = camera_rotation;



	//m_dx = sin(camera_rotation);
	//m_dz = cos(camera_rotation);
}

void Camera::Rotate(float degrees)
{

	m_camera_rotation = m_camera_rotation + degrees;
	//m_dx = sin(m_camera_rotation);
	//m_dz = cos(m_camera_rotation);

}

void Camera::Forward(float step)
{

	m_x += step * m_dx;
	m_z += step * m_dz;
}

void Camera::Up(float step)
{

	m_y += step + m_y;
}

XMMATRIX Camera::GetViewMatrix()
{
	m_dx = sin(m_camera_rotation * (XM_PI / 180));
	m_dz = cos(m_camera_rotation * (XM_PI / 180));

	XMVECTOR position = XMVectorSet(m_x, m_y, m_z, 0.0f);
	XMVECTOR lookat = XMVectorSet(m_x + m_dx, m_y, m_z + m_dz, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(position, lookat, up);

	return view;
}