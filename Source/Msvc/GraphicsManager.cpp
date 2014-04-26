#include "GraphicsManager.h"

GraphicsManager::GraphicsManager()
{
	m_pD3D = 0;
	m_pBitmap = 0;
}

GraphicsManager::GraphicsManager(const GraphicsManager& other)
{

}

GraphicsManager::~GraphicsManager()
{

}

bool GraphicsManager::Initialize(HWND hwnd)
{
	bool result;

	m_pD3D = BE_NEW(D3DClass);
	if (!m_pD3D)
	{
		return false;
	}

	if (!m_pD3D->Initialize(SCREEN_WIDTH, SCREEN_HEIGHT, true, hwnd, false, 1000.0f, 0.1f))
	{
		BE_ERROR(L"Could not initialize D3D11");
		return FALSE;
	}

	m_pBitmap = BE_NEW(BitmapClass);
	if (!m_pBitmap)
	{
		return false;
	}

	result = m_pBitmap->Initialize(m_pD3D->GetDevice(), SCREEN_WIDTH, SCREEN_HEIGHT, L"../../Data/test.png", 256, 256);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Bitmap Object!", L"ERROR!", MB_OK);
		return false;
	}

	return true;
}

void GraphicsManager::Shutdown()
{
	if (m_pBitmap)
	{
		m_pBitmap->Shutdown();
		
		SAFE_DELETE(m_pBitmap);
	}

	if (m_pD3D)
	{
		m_pD3D->Shutdown();

		SAFE_DELETE(m_pD3D);
	}

	return;
}

bool GraphicsManager::Frame()
{
	bool result;
	
	//render graphics scene
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool GraphicsManager::Render()
{
	//Render Logic
	return true;
}