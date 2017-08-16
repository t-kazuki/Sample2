/***************************************************************/
//
//
//		DirectX	[ debugmanager.h ]
//
//											Author	kazuki tanaka
//											Date	2017 1/7
/*---------------------------------------------------------------
��Update : 2017/1/7
			debugmanager.h�̍쐬
			CDebugManager�̍쐬
			CLASS : CDebugManager
				CDebugManager::Init�̒ǉ�
				CDebugManager::Uninit�̒ǉ�
				CDebugManager::Update�̒ǉ�
				CDebugManager::Draw�̒ǉ�

��Update : 2016/1/7



/*-------------------------------------------------------------*/
#ifndef _DEBUGMANAGER_H_

#define _DEBUGMANAGER_H_


/*---------------------------------------------------------------
	�C���N���[�h�t�@�C��
---------------------------------------------------------------*/

#include <vector>
#include <map>

typedef unsigned int MY_NUMBER;

#define PROCESS_COUNT (256)								// �ۑ����Ă���������

// �f�o�b�O�N���X
class CConsole;											// �R���\�[���p�f�o�b�O�N���X
class CDebugLog;										// ���O�t�@�C���p�f�o�b�O�N���X
class CDebugFont;										// Window�p�f�o�b�O�N���X
class CProcessMeasure									// �������x�v���N���X

/*---------------------------------------------------------------
	CLASS
-----------------------------------------------------------------*/

// �f�o�b�O�}�l�[�W���[
class CDebugManager
{
public:

	CDebugManager();									// �}�l�[�W���[�̃R���X�g���N�^
	~CDebugManager();									// �}�l�[�W���[�̃f�X�g���N�^
	
	void Init( void );									// �}�l�[�W���[�̏���������
	void Uninit( void );								// �}�l�[�W���[�̏I������
	void Update( void );								// �}�l�[�W���[�̍X�V����
	void Draw( void );									// �}�l�[�W���[�̕`�揈��

	// �����v���n
	
	void StartMeasureProcess( void );					// �����̌v���J�n
	void EndMeasureProcess( void );						// �����̌v���I��
	void StartMeasureProcess( const int index );		// �����̌v���J�n(�w��)
	void EndMeasureProcess( const int index );			// �����̌v���J�n(�w��)
	void UpdateGraph( void );							// �f�[�^�̍X�V�Ȃ�
	void DrawGraph( void );								// �������ԃO���t�̕`��

	
	CDebugLog* GetDebugLog( void );						// �f�o�b�O���O�̃|�C���^�擾
	CDebugFont* GetDebugFont( void );					// �f�o�b�O�t�H���g�̃|�C���^�擾
	
	// �X�^�e�B�b�N�����o�֐�
	static CDebugManager* Create( void );				// �}�l�[�W���[�̍쐬
	static CDebugManager* IndexAccess(const int number);// �C���f�b�N�X�ԍ��̃|�C���^�̎擾

	static void DebugMsg( char* buf, ... );				// �f�o�b�O���b�Z�[�W�{�b�N�X 
	
#if _UNICODE
	// �f�o�b�O������iUNICODE�p�j
	//static HRESULT IndexAccess( const wstring name );	// �C���f�b�N�X�ԍ��̃|�C���^�̎擾(UNICODE)
#else
	// �f�o�b�O������iANCI�p�j
	//static HRESULT IndexAccess( const string name );	// �C���f�b�N�X�ԍ��̃|�C���^�̎擾(ANSI)
#endif
	
	static void UpdateAll( void );						// �}�l�[�W���[���X�V
	static void DrawAll( void );						// �}�i�[�W���[���`�揈��
	static void ReleaseAll( void );						// �S�}�l�[�W���[�̔j������	
	
private:
	
	void Release( void );								// �}�l�[�W���[�̉��
	
	MY_NUMBER   myNumber;								// �}�l�[�W���ԍ�
	
	CConsole*   pDebugConsole;							// �R���\�[���p�f�o�b�O�N���X�̃|�C���^
	CDebugLog*  pDebugLog;								// ���O�t�@�C���p�f�o�b�O�N���X�̃|�C���^
	CDebugFont* pDebugFont;								// Window�p�f�o�b�O�N���X�̃|�C���^
	
	D3DXVECTOR3 pos;									// �e�N�X�`���̈ʒu���W
	LPDIRECT3DVERTEXBUFFER9 pVtxBuf;					// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
	LPDIRECT3DTEXTURE9  pTexture;						// �e�N�X�`���|�C���^�̃A�h���X
	LPDIRECT3DTEXTURE9  pBackGround;					// �e�N�X�`���|�C���^�̃A�h���X
	IDirect3DSurface9*  pSurface;						// �e�N�X�`���T�[�t�F�X
	IDirect3DSurface9*  pBackbuf;						// �o�b�N�o�b�t�@�|�C���^�i�[
	
	LPD3DXLINE  pLine;									// �O���t�`��p�f�o�C�X�̃|�C���^
	D3DXVECTOR2 point[PROCESS_COUNT];					// �������x�f�[�^�i�[�p
	
	std::vector<LARGE_INTEGER> nFreq;					// �������g��
	std::vector<LARGE_INTEGER> nBefore;					// �����J�n����
	std::vector<LARGE_INTEGER> nAfter;					// �����I������
	
	LARGE_INTEGER freq;
	LARGE_INTEGER before;
	LARGE_INTEGER after;
	
	std::vector<DWORD> dwTime;							// �������Ԋi�[
	
	std::vector< std::map< std::string, int> > 	name;	// �v����̐�����(�A�z�z���vector���I�z��)
	int measureNumber;									// �v����̐�
	
	// �X�^�e�B�b�N�����o�ϐ�
	static std::vector<CDebugManager*> pManager;		// �}�l�[�W���[�A�h���X
	static int counter;									// �}�l�[�W���[�̌�
	static LPDIRECT3DDEVICE9 pDevice;					// DirectX�f�o�C�X�̃A�h���X
	
};



#endif	//	_DEBUGMANAGER_H_


