/***************************************************************/
//
//
//		DirectX	[ debugmanager.cpp ]
//
//											Author	kazuki tanaka
//											Date	2017 1/7
/*---------------------------------------------------------------
��Update : 2017/1/7
			debugmanager.cpp�̍쐬

/*---------------------------------------------------------------
	�C���N���[�h�t�@�C��
---------------------------------------------------------------*/

//#include <Windows.h>
//#include <stdio.h>
//#include <d3dx9.h>

//#include <WinBase.h>
#include "main.h"
#include "d_manager.h"
#include "d_console.h"
#include "d_log.h"
#include "d_font.h"
//#include "debugfont.h"

// ���[�U�[�C���v�b�g
#include "mouse.h"

// �v�Z�R���|�[�l���g
#include "game_equation.h"

#pragma comment ( lib, "Kernel32.lib" )
//#pragma comment (lib,"d3d9.lib")
//#pragma comment (lib,"d3dx9.lib")
//#pragma comment (lib,"dxguid.lib")

using namespace std;      								// �W�����C�u�������O��Ԏw��

/*---------------------------------------------------------------
	�}�N����`
---------------------------------------------------------------*/

#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p)=NULL; } }
#define RADIAN( theta ) ((( theta ) * D3DX_PI ) / 180 )	// ���W�A���p�ւ̕ϊ�

// �A�t�B���ϊ�
#define AFFINE_TRANSFORM_X( radiusX, radiusY, theta ) \
(( radiusX ) * cosf( RADIAN( theta )) - ( radiusY ) * sinf( RADIAN( theta )))

#define AFFINE_TRANSFORM_Y( radiusX, radiusY, theta ) \
(( radiusX ) * sinf( RADIAN( theta )) + ( radiusY ) * cosf( RADIAN( theta )))


/*---------------------------------------------------------------
	�萔��`
---------------------------------------------------------------*/

#define NUM_VERTEX  ( 4 )								// 1�`��Ŏg�p���钸�_��
#define NUM_POLYGON ( 2 )								// �`�悷��|���S����

#define TEXTURE_NAME "data/TEXTURE/background.jpg"		// �e�N�X�`���p�X

/*----------------------------------------------------------------
	�X�^�e�B�b�N�����o�ϐ��̏�����
----------------------------------------------------------------*/

vector<CDebugManager*> CDebugManager::pManager;
int                    CDebugManager::counter = 0;
LPDIRECT3DDEVICE9      CDebugManager::pDevice = nullptr;



// �f�o�b�O�}�l�[�W���[�̃R���X�g���N�^
CDebugManager::CDebugManager( )
{
	
	// �f�o�b�O�}�l�[�W���̕ϐ�������
	pDebugConsole = nullptr;
	pDebugLog     = nullptr;
	pDebugFont    = nullptr;
	pLine         = nullptr;

	// �v���ϐ��̏�����
	//ZeroMemory( &point, sizeof( D3DXVECTOR3 * PROCESS_COUNT ) );
	measureNumber = 0;

	// �}�C�i���o�[������
	myNumber = 0;	

}

// �f�o�b�O�}�l�[�W���[�̃f�X�g���N�^
CDebugManager::~CDebugManager( )
{

	// �����Ȃ�

}

// �f�o�b�O�}�l�[�W���[�̏�����
void CDebugManager::Init( void )
{


	{ // �f�o�b�O�@�\�Z�b�g

		// �R���\�[��������
		#ifdef _DEBUG
		CConsole::OpenConsole( );
		CConsole::DebugLog( " -- Start DebugConsole -- \n" );
		#endif // _DEBUG

		// ���O�J�n
		pDebugLog = new CDebugLog;
		pDebugLog ->StartLog( "data\\log.txt");

		// Window�f�o�b�O������
		pDebugFont = CDebugFont::SetDebugFont( ); 
		pDebugFont ->SetColor( &D3DXCOLOR( 0.39f, 1, 1, 1 ) );

	} // -> START DEBUG


	// �O���t�`��p�f�o�C�X�̐ݒ�
	if( !pDevice ){
		// �f�o�C�X�̎擾
		pDevice = GetDevice( );
	}

	// ���C���`��f�o�C�X�̍쐬
	HRESULT hResult = D3DXCreateLine( pDevice, &pLine ); 
	if( FAILED( hResult ) ){
	
		#ifdef _DEBUG
		MessageBox( NULL, "LINE DEVICE DONT CREATED !!", "CREATE LINE ERROR !! ", MB_OK | MB_ICONWARNING );
		#endif // _DEBUG
	}

	// �_���̏�����
	for( int index = 0; index < PROCESS_COUNT; index++ ){
	
		point[ index ] = D3DXVECTOR2( ((SCREEN_WIDTH / PROCESS_COUNT) * index), 0.0f + ( SCREEN_HEIGHT >> 1 ) + (10 * index) );
	}


	// �e�N�X�`���̓ǂݍ��� 
	#ifdef TEXTURE_NAME
	HRESULT hres = D3DXCreateTextureFromFile( pDevice, TEXTURE_NAME, &pBackGround );
	if( FAILED( hres )){
		#ifdef _DEBUG
		MessageBox( NULL, "�e�N�X�`���t�@�C���̓ǂݍ��݂Ɏ��s���܂���!" , "TEXTURE LOAD ERROR!!", MB_OK | MB_ICONWARNING );
		#endif	// _DEBUG
	}
	#endif // TEXTURE_NAME

	// �o�b�N�o�b�t�@�|�C���^�i�[
	pDevice->GetRenderTarget( 0 , &pBackbuf );

	// �e�N�X�`���̍쐬
	pDevice ->CreateTexture(
		SCREEN_WIDTH, SCREEN_HEIGHT,
		1,
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&pTexture,
		NULL
	);	

	// �T�[�t�F�X�̐ݒ�
	pTexture ->GetSurfaceLevel( 0, &pSurface );
	
	// �|���S���ʒu�̐ݒ�
	pos = D3DXVECTOR3( (SCREEN_WIDTH >>1) + (SCREEN_WIDTH >>2), (SCREEN_HEIGHT >> 1) - (SCREEN_HEIGHT >>2), 0.0f );
	
	// ���_�o�b�t�@�̍쐬
	VERTEX_2D *pVtx;									// ���z�A�h���X

	if(FAILED(pDevice->CreateVertexBuffer(
			sizeof(VERTEX_2D) * NUM_VERTEX,	 	 		// �m�ۂ���o�b�t�@�T�C�Y(���_�����m��) �P��:�o�C�g
			D3DUSAGE_WRITEONLY,							// �g�p���@
			FVF_VERTEX_2D,								// �g�p���钸�_�t�H�[�}�b�g(����ȏ��������Ȃ�����Q�Ƃ���Ȃ��̂�0�ł悢)
			D3DPOOL_MANAGED,							// �o�b�t�@�̊Ǘ����@(�o�b�t�@�̒��ɏ������񂾂��̂̊Ǘ����@)
			&pVtxBuf,									// �i�[����|�C���^��
			NULL)))
	{
		#ifdef _DEBUG
		MessageBox(NULL,"�o�b�t�@�ւ̏������݂Ɏ��s���܂���!!","vertex buffer ERROR!!", MB_OK );
		#endif // _DEBUG
		return;
	}

	// ���W�̐ݒ�
	D3DXVECTOR2 start,end;								// �e�N�X�`���ʒu��� �n�_�ƏI�_
	start = D3DXVECTOR2( pos.x - ( SCREEN_WIDTH * 0.15f ), pos.y - ( SCREEN_HEIGHT * 0.15f ) );
	end   = D3DXVECTOR2( pos.x + ( SCREEN_WIDTH * 0.15f ), pos.y + ( SCREEN_HEIGHT * 0.15f ) );

	
	// �o�b�t�@�����b�N�����z�A�h���X���擾����
	pVtxBuf -> Lock(0,0,(void**)&pVtx,0);				// GPU�����VRAM�ւ̑�������b�N����

	pVtx[ 0 ].pos = D3DXVECTOR3( start.x, start.y, 0.0f ); 
	pVtx[ 1 ].pos = D3DXVECTOR3( end.x,   start.y, 0.0f ); 
	pVtx[ 2 ].pos = D3DXVECTOR3( start.x, end.y,   0.0f ); 
	pVtx[ 3 ].pos = D3DXVECTOR3( end.x,   end.y,   0.0f ); 

	pVtx[ 0 ].rhw = 1.0f;
	pVtx[ 1 ].rhw = 1.0f;
	pVtx[ 2 ].rhw = 1.0f;
	pVtx[ 3 ].rhw = 1.0f;

	pVtx[ 0 ].color = D3DCOLOR_RGBA( 192, 192, 244, 100 );
	pVtx[ 1 ].color = D3DCOLOR_RGBA( 192, 192, 244, 100 );
	pVtx[ 2 ].color = D3DCOLOR_RGBA( 192, 192, 244, 100 );
	pVtx[ 3 ].color = D3DCOLOR_RGBA( 192, 192, 244, 100 );

	pVtx[ 0 ].tex = D3DXVECTOR2( 0.0f, 0.0f );
	pVtx[ 1 ].tex = D3DXVECTOR2( 1.0f, 0.0f );
	pVtx[ 2 ].tex = D3DXVECTOR2( 0.0f, 1.0f );
	pVtx[ 3 ].tex = D3DXVECTOR2( 1.0f, 1.0f );

	// �o�b�t�@�̃A�����b�N
	pVtxBuf ->Unlock();

	
	// �v����̍쐬
	LARGE_INTEGER def = {};
	DWORD set = 0;
	nFreq.push_back( def );
	nBefore.push_back( def );
	nAfter.push_back( def );
	dwTime.push_back( set );

	// �v���ϐ��̏�����
	memset(&nFreq[measureNumber],   0x00, sizeof nFreq[measureNumber]);
	memset(&nBefore[measureNumber], 0x00, sizeof nBefore[measureNumber]);
	memset(&nAfter[measureNumber],  0x00, sizeof nAfter[measureNumber]);
	memset(&freq,   0x00, sizeof freq );
	memset(&before, 0x00, sizeof before );
	memset(&after,  0x00, sizeof after );
	dwTime[measureNumber] = 0;

	measureNumber++;

}

// �f�o�b�O�}�l�[�W���[�̏I������
void CDebugManager::Uninit( void )
{


	// ���C���`��f�o�C�X�̍폜
	SAFE_RELEASE( pLine )

	// ���_�o�b�t�@�̔j��
	SAFE_RELEASE( pVtxBuf )

	// �e�N�X�`���T�[�t�F�X�j��
	SAFE_RELEASE( pTexture )
	SAFE_RELEASE( pBackGround )
	SAFE_RELEASE( pSurface )
	SAFE_RELEASE( pBackbuf )

	{ // �f�o�b�O�@�\�I��
	
		// �R���\�[���I��
		CConsole::CloseConsole( );

		// ���O�I��
		pDebugLog ->EndLog( );
		SAFE_DELETE( pDebugLog )

		// 

	} // -> END DEBUG

	// �f�o�b�O�}�l�[�W���[�̔j��
	CDebugManager::Release( );
	
}

// �f�o�b�O�}�l�[�W���[�̍X�V
void CDebugManager::Update( void )
{

	// �O���t�̃f�[�^�X�V
	CDebugManager::UpdateGraph( );
	
	{ // ���_���W�̍X�V
		
		VERTEX_2D *pVtx;									// ���z�A�h���X

		// �o�b�t�@�����b�N�����z�A�h���X���擾����
		pVtxBuf -> Lock(0,0,(void**)&pVtx,0);		// GPU�����VRAM�ւ̑�������b�N����

		// ���W�̐ݒ�
		D3DXVECTOR2 start,end;								// �e�N�X�`���ʒu��� �n�_�ƏI�_
		start = D3DXVECTOR2( pos.x - ( SCREEN_WIDTH * 0.15f ), pos.y - ( SCREEN_HEIGHT * 0.15f ) );
		end   = D3DXVECTOR2( pos.x + ( SCREEN_WIDTH * 0.15f ), pos.y + ( SCREEN_HEIGHT * 0.15f ) );

		pVtx[ 0 ].pos = D3DXVECTOR3( start.x, start.y, 0.0f ); 
		pVtx[ 1 ].pos = D3DXVECTOR3( end.x,   start.y, 0.0f ); 
		pVtx[ 2 ].pos = D3DXVECTOR3( start.x, end.y,   0.0f ); 
		pVtx[ 3 ].pos = D3DXVECTOR3( end.x,   end.y,   0.0f ); 

		// �o�b�t�@�̃A�����b�N
		pVtxBuf ->Unlock();
		
	}

}

// �f�o�b�O�}�l�[�W���[�̕`��
void CDebugManager::Draw( void )
{
	
	// �������ԃO���t�̕`��
	CDebugManager::DrawGraph( );
	
	{ // �|���S���̕`��
	
		// �X�g���[��
		pDevice->SetStreamSource(0,
		pVtxBuf,											// �ǂ̒��_�o�b�t�@�\�ƌ��Ԃ�...
		0,													// �f�[�^�̃I�t�Z�b�g
		sizeof(VERTEX_2D));									// �X�g���C�h�l(�o�C�g)

		// ���_�t�H�[�}�b�g�̐ݒ�
		pDevice ->SetFVF(FVF_VERTEX_2D);

		// �e�N�X�`���̐ݒ�
		pDevice ->SetTexture( 0, pBackGround );		

		// �|���S���̕`��
		pDevice ->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0 , NUM_POLYGON );

		// �e�N�X�`���̐ݒ�
		pDevice ->SetTexture( 0, pTexture );		

		// �|���S���̕`��
		pDevice ->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0 , NUM_POLYGON );
	}

}

// �������Ԍv���J�n
void CDebugManager::StartMeasureProcess( void )
{

	// ���g���ݒ�
	if( !QueryPerformanceFrequency( &freq ) ){

		#ifdef _DEBUG
		CConsole::DebugLog( " OS���@�\���T�|�[�g���Ă��Ȃ����ߌv�����o���܂���ł����B" );
		#endif // _DEBUG
		return;
	}
	//QueryPerformanceFrequency(&freq);
	//nFreq[0].QuadPart = def.QuadPart;

	// �v���J�n
	QueryPerformanceCounter(&before);

}

// �������Ԍv���I��
void CDebugManager::EndMeasureProcess( void )
{
	

	// �v���I��
	if( !QueryPerformanceCounter(&after) ){

		#ifdef _DEBUG
		CConsole::DebugLog( " OS���@�\���T�|�[�g���Ă��Ȃ����ߌv�����o���܂���ł����B" );
		#endif // _DEBUG
		return;
	}

	// �������Ԏ擾
	dwTime[0] = (DWORD)((after.QuadPart - before.QuadPart) * 1000 / freq.QuadPart);

	// �f�[�^��FIFO�Ń\�[�g
	for( int index = 0; index < PROCESS_COUNT-1; index++ ){
	
		point[ index ].y =  point[ index+1 ].y;
	}

	// �l��ݒ�
	point[ PROCESS_COUNT-1 ].y = (float)dwTime[0] * 10.0f;
	point[ PROCESS_COUNT-1 ].y += (float)( SCREEN_HEIGHT >> 1 );
	#ifdef _DEBUG
	//pDebugLog ->WriteLog( " -- �������� -- \n" );
	#endif // _DEBUG

	for( int index = 0; index < PROCESS_COUNT; index++ ){
	
		point[ index ].x = ((SCREEN_WIDTH / (PROCESS_COUNT-1)) * index);
		#ifdef _DEBUG
		//pDebugLog ->WriteLog( " index : %d \n x : %f  \n y : %f \n ", index, point[index].x, point[index].y );
		#endif // _DEBUG
	}

}

// �������Ԍv���J�n(�w��)
void CDebugManager::StartMeasureProcess( const int index )
{

	// ���g���ݒ�
	if( !QueryPerformanceFrequency( &freq ) ){

		#ifdef _DEBUG
		CConsole::DebugLog( " OS���@�\���T�|�[�g���Ă��Ȃ����ߌv�����o���܂���ł����B" );
		#endif // _DEBUG
		return;
	}

	// �v���J�n
	QueryPerformanceCounter(&before);


}

// �������Ԍv���J�n(�w��)
void CDebugManager::EndMeasureProcess( const int index )
{


	// �v���I��
	if( !QueryPerformanceCounter(&after) ){

		#ifdef _DEBUG
		CConsole::DebugLog( " OS���@�\���T�|�[�g���Ă��Ȃ����ߌv�����o���܂���ł����B" );
		#endif // _DEBUG
		return;
	}

	// �������Ԏ擾
	dwTime[0] = (DWORD)((after.QuadPart - before.QuadPart) * 1000 / freq.QuadPart);

	// �f�[�^��FIFO�Ń\�[�g
	for( int index = 0; index < PROCESS_COUNT-1; index++ ){
	
		point[ index ].y =  point[ index+1 ].y;
	}

	// �l��ݒ�
	point[ PROCESS_COUNT-1 ].y = (float)dwTime[0] * 10.0f;
	point[ PROCESS_COUNT-1 ].y += (float)( SCREEN_HEIGHT >> 1 );
	#ifdef _DEBUG
	CConsole::DebugLog( " -- �������� -- \n" );
	#endif // _DEBUG

	for( int index = 0; index < PROCESS_COUNT; index++ ){
	
		point[ index ].x = ((SCREEN_WIDTH / (PROCESS_COUNT-1)) * index);
		#ifdef _DEBUG
		//CConsole::DebugLog( " index : %d \n x : %f  \n y : %f \n ", index, point[index].x, point[index].y );
		#endif // _DEBUG
	}


}

// �������ԃO���t�p�f�[�^�̍X�V
void CDebugManager::UpdateGraph( void )
{

	// �O���t�ʒu�X�V
	#ifdef _GAME_EQUATION_H_
	#ifdef _MOUSE_H_
	D3DXVECTOR2 start = D3DXVECTOR2( pos.x - ( SCREEN_WIDTH * 0.15f ), pos.y - ( SCREEN_HEIGHT * 0.15f ) );
	D3DXVECTOR2 end   = D3DXVECTOR2( pos.x + ( SCREEN_WIDTH * 0.15f ), pos.y + ( SCREEN_HEIGHT * 0.15f ) );
	D3DXVECTOR2 coord = D3DXVECTOR2( CMouse::Access()->GetMouseX(), CMouse::Access()->GetMouseY() );
	if( __ObjectTouchSquare( &start, &end, &coord ) ){
		if( CMouse::Access()->GetKeyPress( CMouse::MOUSE_BUTTON_LEFT ) ){

			// �}�E�X���W�擾
			pos.x = CMouse::Access()->GetMouseX( );
			pos.y = CMouse::Access()->GetMouseY( );
		}
	}
	#endif // _MOUSE_H_
	#endif // _GAME_EQUATION_H_

}

// �������Ԃ���O���t�`��
void CDebugManager::DrawGraph( void )
{


	// �����_�����O�^�[�Q�b�g���e�N�X�`���ɕύX
	pDevice->SetRenderTarget( 0 , pSurface );

	
	// �T�[�t�F�C�X�̃N���A
	pDevice->Clear(0,								// �N���A�����`�̈�̐�
		NULL,										// ��`�̈�
		D3DCLEAR_TARGET								// �����_�����O�^�[�Q�b�g�Ɛ[�x�o�b�t�@���N���A 
		| D3DCLEAR_ZBUFFER,							// �[�x���
		D3DCOLOR_RGBA( 255,255,255,0 ),				// �F�����ɓ����x��100%�ɃN���A
		1.0f,										// Z�o�b�t�@�̃N���A�l
		0											// �X�e���V���o�b�t�@�̃N���A�l
	);
	
	
	// ���̑��� 
	pLine ->SetWidth( 5.0f ); 

	// �`��J�n
	pLine->Begin( );  

	// �A���t�@�u�����f�B���O�̐ݒ� 
	pDevice ->SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE); 
	pDevice ->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA); 
	pDevice ->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA); 

	// �O���t�`��
	pLine ->Draw( 
		point,								// �_���
		PROCESS_COUNT,						// ��������
		D3DCOLOR_ARGB(255,255,0,0));		// �`�悷����̐F
	//�`��I��
	
	pLine->End( ); 
	
	
	// �����_�����O�^�[�Q�b�g���o�b�N�o�b�t�@�ɕύX
	pDevice->SetRenderTarget( 0 , pBackbuf );


}

// �f�o�b�O���O�̃|�C���^�̎擾
CDebugLog* CDebugManager::GetDebugLog( void )
{

	#ifdef _DEBUGLOG_H_
		return pManager[ myNumber ] ->pDebugLog;
	#else
		#ifdef _DEBUG
		MessageBox( NULL, "ERROR", "ERROR !!", MB_OK | MB_ICONWARNING );
		#endif // _DEBUG
	#endif // _DEBUGLOG_H_

	return 0;

}

// �f�o�b�O�t�H���g�̎擾
CDebugFont* CDebugManager::GetDebugFont( void )
{

	#ifdef _DEBUGFONT_H_
		return pManager[ myNumber ] ->pDebugFont;
	#else
		#ifdef _DEBUG
		MessageBox( NULL, "ERROR", "ERROR !!", MB_OK | MB_ICONWARNING );
		#endif // _DEBUG
	#endif // _DEBUGFONT_H_

	return 0;

}

// �f�o�b�O�}�l�[�W���[�̍쐬
CDebugManager* CDebugManager::Create( void )
{

	// �f�o�C�X�̎擾
	if( !pDevice ){
	
		pDevice = GetDevice( );
	}

	// �f�o�b�O�}�l�[�W���[�̍쐬
	CDebugManager* pDebugManager;
	pDebugManager = new CDebugManager;
	pManager.push_back( pDebugManager );
	pManager[counter] ->myNumber = counter;
	counter++;

	return pDebugManager;

}

// �f�o�b�O�}�l�[�W���[�̃C���f�b�N�X�ԍ��̃|�C���^�̎擾
CDebugManager* CDebugManager::IndexAccess( const int number )
{

	// �G���[�`�F�b�N
	if( number >= counter ){
	
		#ifdef _DEBUG
		MessageBox( NULL, "�Q�Ɣԍ�������������܂���", " ERROR D_MANAGER ", MB_OK | MB_ICONWARNING );
		#endif	// _DEBUG
		return NULL;
	}

	return pManager[ number ];

}

// �S�X�V����
void CDebugManager::UpdateAll( void )
{
	
	for( int index = 0; index < counter; index ++ ){

		if( pManager[ index ] != NULL ){
		
			pManager[ index ] ->Update( );
		}
	}


}

// �S�`�揈��
void CDebugManager::DrawAll( void )
{

	for( int index = 0; index < counter; index ++ ){

		if( pManager[ index ] != NULL ){
		
			pManager[ index ] ->Draw( );
		}
	}

	// Render Debug Font
	CDebugFont::DrawAll( );

}

// �S�J������
void CDebugManager::ReleaseAll( void )
{

	// CDebug Font Release
	CDebugFont::ReleaseAll( );

	for( int index = 0; index < counter; index ++ ){

		if( pManager[ index ] != NULL ){
		
			pManager[ index ] ->Uninit( );
		}
	}

}

// �ʉ������
void CDebugManager::Release( void )
{
	
	if( pManager[ myNumber ] != NULL ){

		int nID;

		// delete�̍ۂ�ID�����������̂ŕۑ�����
		nID = myNumber;

		// �f�o�b�O�}�l�[�W���[�̍폜
		pManager.erase( pManager.begin() + nID );

		// �폜�Ɠ����ɑO�l�߂����̂ŁA�C���f�b�N�X�ԍ����O�l��
		if( counter != nID ){
			for( int index = nID; index < counter-1; index++ ){
			
				pManager[ nID ] ->myNumber = index;
			}
		}
		//pManager[ counter ] = NULL;
		counter--;
	}

}

// �f�o�b�O���b�Z�[�W�o��
void CDebugManager::DebugMsg( char* buf, ... )
{

	va_list ap;
	char str[ 256 ];
	bool flag = false;
	DWORD word;

	va_start( ap, buf );
	vsprintf( str, buf, ap );
	va_end( ap );

	#ifdef _DEBUG
	MessageBox( NULL, str, WINDOW_NAME, MB_OK | MB_ICONWARNING );
	#endif // _DEBUG


}


