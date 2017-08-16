//=============================================================================
//
// ���͏��� [input.cpp]
// Author : AKIRA TANAKA
//
//=============================================================================
#include "inputdevice.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	COUNT_WAIT_REPEAT	(20)	// ���s�[�g�҂�����

//*****************************************************************************
// �ÓI�ϐ�
//*****************************************************************************
LPDIRECTINPUT8	CInputDevice::m_pDInput = NULL;	// DirectInput�I�u�W�F�N�g

//=============================================================================
// CInputDevice�R���X�g���X�^
//=============================================================================
CInputDevice::CInputDevice()
{
	m_pDIDevice = NULL;
}

//=============================================================================
// CInputDevice�f�X�g���X�^
//=============================================================================
CInputDevice::~CInputDevice()
{

}

//=============================================================================
// ���͏����̏���������
//=============================================================================
HRESULT CInputDevice::Init(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr = S_FALSE;

	// DirectInput�I�u�W�F�N�g�̍쐬
	if(!m_pDInput)
	{
		hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION,
									IID_IDirectInput8, (void**)&m_pDInput, NULL);
	}

	return hr;
}

// INPUT_DEVICE�̏���������
HRESULT CInputDevice::InitSingleDevice( HINSTANCE hInstance, HWND hWnd )
{

	HRESULT hr = S_FALSE;

	// DirectInput�I�u�W�F�N�g�̍쐬
	if(!m_pDInput)
	{
		hr = DirectInput8Create( hInstance, DIRECTINPUT_VERSION,
									IID_IDirectInput8, (void**)&m_pDInput, NULL);
	}

	return hr;

}

//=============================================================================
// ���͏����̍X�V����
//=============================================================================
#if 0	// ---> �������z�֐���
HRESULT CInputDevice::Update(void)
{
	return S_OK;
}
#endif

//=============================================================================
// ���͏����̏I������
//=============================================================================
void CInputDevice::Uninit(void)
{
	if(m_pDIDevice)
	{// �f�o�C�X�I�u�W�F�N�g�̊J��
		m_pDIDevice->Unacquire();

		m_pDIDevice->Release();
		m_pDIDevice = NULL;
	}

	if(m_pDInput)
	{// DirectInput�I�u�W�F�N�g�̊J��
		m_pDInput->Release();
		m_pDInput = NULL;
	}
}

// ���͏����̏I������
void CInputDevice::UninitSingleDevice( void )
{
	
	if(m_pDInput)
	{// DirectInput�I�u�W�F�N�g�̊J��
		m_pDInput->Release();
		m_pDInput = NULL;
	}

}

// INPUT_DEVICE�̎擾
LPDIRECTINPUT8 CInputDevice::GetInputDevice( void )
{

	return m_pDInput;

}
