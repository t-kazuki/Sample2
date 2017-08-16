//=============================================================================
//
// ���͏��� [input.h]
// Author : AKIRA TANAKA
//
//=============================================================================
#ifndef _INPUTDEVICE_H_
#define _INPUTDEVICE_H_

#include "main.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	NUM_KEY_MAX		(256)	// �L�[�ő吔

//*********************************************************
// ���̓N���X
//*********************************************************
class CInputDevice
{
public:

	CInputDevice( );
	virtual ~CInputDevice( );

	virtual HRESULT Init( HINSTANCE hInst, HWND hWnd );
	virtual void Uninit( void );
	virtual void Update( void ) = 0;		// ---> �������z�֐���


protected:
	static LPDIRECTINPUT8 m_pDInput;		// DirectInput�I�u�W�F�N�g
	LPDIRECTINPUTDEVICE8  m_pDIDevice;		// Device�I�u�W�F�N�g(���͂ɕK�v)
};

typedef int int32b;

#endif