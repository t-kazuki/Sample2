/***************************************************************/
//
//
//		DirectX	[ debugmanager.h ]
//
//											Author	kazuki tanaka
//											Date	2017 1/7
/*---------------------------------------------------------------
■Update : 2017/1/7
			debugmanager.hの作成
			CDebugManagerの作成
			CLASS : CDebugManager
				CDebugManager::Initの追加
				CDebugManager::Uninitの追加
				CDebugManager::Updateの追加
				CDebugManager::Drawの追加

■Update : 2016/1/7



/*-------------------------------------------------------------*/
#ifndef _DEBUGMANAGER_H_

#define _DEBUGMANAGER_H_


/*---------------------------------------------------------------
	インクルードファイル
---------------------------------------------------------------*/

#include <vector>
#include <map>

typedef unsigned int MY_NUMBER;

#define PROCESS_COUNT (256)								// 保存しておく処理回数

// デバッグクラス
class CConsole;											// コンソール用デバッグクラス
class CDebugLog;										// ログファイル用デバッグクラス
class CDebugFont;										// Window用デバッグクラス
class CProcessMeasure									// 処理速度計測クラス

/*---------------------------------------------------------------
	CLASS
-----------------------------------------------------------------*/

// デバッグマネージャー
class CDebugManager
{
public:

	CDebugManager();									// マネージャーのコンストラクタ
	~CDebugManager();									// マネージャーのデストラクタ
	
	void Init( void );									// マネージャーの初期化処理
	void Uninit( void );								// マネージャーの終了処理
	void Update( void );								// マネージャーの更新処理
	void Draw( void );									// マネージャーの描画処理

	// 処理計測系
	
	void StartMeasureProcess( void );					// 処理の計測開始
	void EndMeasureProcess( void );						// 処理の計測終了
	void StartMeasureProcess( const int index );		// 処理の計測開始(指定)
	void EndMeasureProcess( const int index );			// 処理の計測開始(指定)
	void UpdateGraph( void );							// データの更新など
	void DrawGraph( void );								// 処理時間グラフの描画

	
	CDebugLog* GetDebugLog( void );						// デバッグログのポインタ取得
	CDebugFont* GetDebugFont( void );					// デバッグフォントのポインタ取得
	
	// スタティックメンバ関数
	static CDebugManager* Create( void );				// マネージャーの作成
	static CDebugManager* IndexAccess(const int number);// インデックス番号のポインタの取得

	static void DebugMsg( char* buf, ... );				// デバッグメッセージボックス 
	
#if _UNICODE
	// デバッグ文字列（UNICODE用）
	//static HRESULT IndexAccess( const wstring name );	// インデックス番号のポインタの取得(UNICODE)
#else
	// デバッグ文字列（ANCI用）
	//static HRESULT IndexAccess( const string name );	// インデックス番号のポインタの取得(ANSI)
#endif
	
	static void UpdateAll( void );						// マネージャー総更新
	static void DrawAll( void );						// マナージャー総描画処理
	static void ReleaseAll( void );						// 全マネージャーの破棄処理	
	
private:
	
	void Release( void );								// マネージャーの解放
	
	MY_NUMBER   myNumber;								// マネージャ番号
	
	CConsole*   pDebugConsole;							// コンソール用デバッグクラスのポインタ
	CDebugLog*  pDebugLog;								// ログファイル用デバッグクラスのポインタ
	CDebugFont* pDebugFont;								// Window用デバッグクラスのポインタ
	
	D3DXVECTOR3 pos;									// テクスチャの位置座標
	LPDIRECT3DVERTEXBUFFER9 pVtxBuf;					// 頂点バッファインターフェースへのポインタ
	LPDIRECT3DTEXTURE9  pTexture;						// テクスチャポインタのアドレス
	LPDIRECT3DTEXTURE9  pBackGround;					// テクスチャポインタのアドレス
	IDirect3DSurface9*  pSurface;						// テクスチャサーフェス
	IDirect3DSurface9*  pBackbuf;						// バックバッファポインタ格納
	
	LPD3DXLINE  pLine;									// グラフ描画用デバイスのポインタ
	D3DXVECTOR2 point[PROCESS_COUNT];					// 処理速度データ格納用
	
	std::vector<LARGE_INTEGER> nFreq;					// 処理周波数
	std::vector<LARGE_INTEGER> nBefore;					// 処理開始時間
	std::vector<LARGE_INTEGER> nAfter;					// 処理終了時間
	
	LARGE_INTEGER freq;
	LARGE_INTEGER before;
	LARGE_INTEGER after;
	
	std::vector<DWORD> dwTime;							// 処理時間格納
	
	std::vector< std::map< std::string, int> > 	name;	// 計測器の製造名(連想配列のvector動的配列)
	int measureNumber;									// 計測器の数
	
	// スタティックメンバ変数
	static std::vector<CDebugManager*> pManager;		// マネージャーアドレス
	static int counter;									// マネージャーの個数
	static LPDIRECT3DDEVICE9 pDevice;					// DirectXデバイスのアドレス
	
};



#endif	//	_DEBUGMANAGER_H_


