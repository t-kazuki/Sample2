/***************************************************************/
//
//
//		DirectX	[ debugmanager.cpp ]
//
//											Author	kazuki tanaka
//											Date	2017 1/7
/*---------------------------------------------------------------
■Update : 2017/1/7
			debugmanager.cppの作成

/*---------------------------------------------------------------
	インクルードファイル
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

// ユーザーインプット
#include "mouse.h"

// 計算コンポーネント
#include "game_equation.h"

#pragma comment ( lib, "Kernel32.lib" )
//#pragma comment (lib,"d3d9.lib")
//#pragma comment (lib,"d3dx9.lib")
//#pragma comment (lib,"dxguid.lib")

using namespace std;      								// 標準ライブラリ名前空間指定

/*---------------------------------------------------------------
	マクロ定義
---------------------------------------------------------------*/

#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p)=NULL; } }
#define RADIAN( theta ) ((( theta ) * D3DX_PI ) / 180 )	// ラジアン角への変換

// アフィン変換
#define AFFINE_TRANSFORM_X( radiusX, radiusY, theta ) \
(( radiusX ) * cosf( RADIAN( theta )) - ( radiusY ) * sinf( RADIAN( theta )))

#define AFFINE_TRANSFORM_Y( radiusX, radiusY, theta ) \
(( radiusX ) * sinf( RADIAN( theta )) + ( radiusY ) * cosf( RADIAN( theta )))


/*---------------------------------------------------------------
	定数定義
---------------------------------------------------------------*/

#define NUM_VERTEX  ( 4 )								// 1描画で使用する頂点数
#define NUM_POLYGON ( 2 )								// 描画するポリゴン数

#define TEXTURE_NAME "data/TEXTURE/background.jpg"		// テクスチャパス

/*----------------------------------------------------------------
	スタティックメンバ変数の初期化
----------------------------------------------------------------*/

vector<CDebugManager*> CDebugManager::pManager;
int                    CDebugManager::counter = 0;
LPDIRECT3DDEVICE9      CDebugManager::pDevice = nullptr;



// デバッグマネージャーのコンストラクタ
CDebugManager::CDebugManager( )
{
	
	// デバッグマネージャの変数初期化
	pDebugConsole = nullptr;
	pDebugLog     = nullptr;
	pDebugFont    = nullptr;
	pLine         = nullptr;

	// 計測変数の初期化
	//ZeroMemory( &point, sizeof( D3DXVECTOR3 * PROCESS_COUNT ) );
	measureNumber = 0;

	// マイナンバー初期化
	myNumber = 0;	

}

// デバッグマネージャーのデストラクタ
CDebugManager::~CDebugManager( )
{

	// 処理なし

}

// デバッグマネージャーの初期化
void CDebugManager::Init( void )
{


	{ // デバッグ機能セット

		// コンソール初期化
		#ifdef _DEBUG
		CConsole::OpenConsole( );
		CConsole::DebugLog( " -- Start DebugConsole -- \n" );
		#endif // _DEBUG

		// ログ開始
		pDebugLog = new CDebugLog;
		pDebugLog ->StartLog( "data\\log.txt");

		// Windowデバッグ初期化
		pDebugFont = CDebugFont::SetDebugFont( ); 
		pDebugFont ->SetColor( &D3DXCOLOR( 0.39f, 1, 1, 1 ) );

	} // -> START DEBUG


	// グラフ描画用デバイスの設定
	if( !pDevice ){
		// デバイスの取得
		pDevice = GetDevice( );
	}

	// ライン描画デバイスの作成
	HRESULT hResult = D3DXCreateLine( pDevice, &pLine ); 
	if( FAILED( hResult ) ){
	
		#ifdef _DEBUG
		MessageBox( NULL, "LINE DEVICE DONT CREATED !!", "CREATE LINE ERROR !! ", MB_OK | MB_ICONWARNING );
		#endif // _DEBUG
	}

	// 点情報の初期化
	for( int index = 0; index < PROCESS_COUNT; index++ ){
	
		point[ index ] = D3DXVECTOR2( ((SCREEN_WIDTH / PROCESS_COUNT) * index), 0.0f + ( SCREEN_HEIGHT >> 1 ) + (10 * index) );
	}


	// テクスチャの読み込み 
	#ifdef TEXTURE_NAME
	HRESULT hres = D3DXCreateTextureFromFile( pDevice, TEXTURE_NAME, &pBackGround );
	if( FAILED( hres )){
		#ifdef _DEBUG
		MessageBox( NULL, "テクスチャファイルの読み込みに失敗しました!" , "TEXTURE LOAD ERROR!!", MB_OK | MB_ICONWARNING );
		#endif	// _DEBUG
	}
	#endif // TEXTURE_NAME

	// バックバッファポインタ格納
	pDevice->GetRenderTarget( 0 , &pBackbuf );

	// テクスチャの作成
	pDevice ->CreateTexture(
		SCREEN_WIDTH, SCREEN_HEIGHT,
		1,
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&pTexture,
		NULL
	);	

	// サーフェスの設定
	pTexture ->GetSurfaceLevel( 0, &pSurface );
	
	// ポリゴン位置の設定
	pos = D3DXVECTOR3( (SCREEN_WIDTH >>1) + (SCREEN_WIDTH >>2), (SCREEN_HEIGHT >> 1) - (SCREEN_HEIGHT >>2), 0.0f );
	
	// 頂点バッファの作成
	VERTEX_2D *pVtx;									// 仮想アドレス

	if(FAILED(pDevice->CreateVertexBuffer(
			sizeof(VERTEX_2D) * NUM_VERTEX,	 	 		// 確保するバッファサイズ(頂点数分確保) 単位:バイト
			D3DUSAGE_WRITEONLY,							// 使用方法
			FVF_VERTEX_2D,								// 使用する頂点フォーマット(特殊な処理をしない限り参照されないので0でよい)
			D3DPOOL_MANAGED,							// バッファの管理方法(バッファの中に書き込んだものの管理方法)
			&pVtxBuf,									// 格納するポインタ先
			NULL)))
	{
		#ifdef _DEBUG
		MessageBox(NULL,"バッファへの書き込みに失敗しました!!","vertex buffer ERROR!!", MB_OK );
		#endif // _DEBUG
		return;
	}

	// 座標の設定
	D3DXVECTOR2 start,end;								// テクスチャ位置情報 始点と終点
	start = D3DXVECTOR2( pos.x - ( SCREEN_WIDTH * 0.15f ), pos.y - ( SCREEN_HEIGHT * 0.15f ) );
	end   = D3DXVECTOR2( pos.x + ( SCREEN_WIDTH * 0.15f ), pos.y + ( SCREEN_HEIGHT * 0.15f ) );

	
	// バッファをロックし仮想アドレスを取得する
	pVtxBuf -> Lock(0,0,(void**)&pVtx,0);				// GPUからのVRAMへの操作をロックする

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

	// バッファのアンロック
	pVtxBuf ->Unlock();

	
	// 計測器の作成
	LARGE_INTEGER def = {};
	DWORD set = 0;
	nFreq.push_back( def );
	nBefore.push_back( def );
	nAfter.push_back( def );
	dwTime.push_back( set );

	// 計測変数の初期化
	memset(&nFreq[measureNumber],   0x00, sizeof nFreq[measureNumber]);
	memset(&nBefore[measureNumber], 0x00, sizeof nBefore[measureNumber]);
	memset(&nAfter[measureNumber],  0x00, sizeof nAfter[measureNumber]);
	memset(&freq,   0x00, sizeof freq );
	memset(&before, 0x00, sizeof before );
	memset(&after,  0x00, sizeof after );
	dwTime[measureNumber] = 0;

	measureNumber++;

}

// デバッグマネージャーの終了処理
void CDebugManager::Uninit( void )
{


	// ライン描画デバイスの削除
	SAFE_RELEASE( pLine )

	// 頂点バッファの破棄
	SAFE_RELEASE( pVtxBuf )

	// テクスチャサーフェス破棄
	SAFE_RELEASE( pTexture )
	SAFE_RELEASE( pBackGround )
	SAFE_RELEASE( pSurface )
	SAFE_RELEASE( pBackbuf )

	{ // デバッグ機能終了
	
		// コンソール終了
		CConsole::CloseConsole( );

		// ログ終了
		pDebugLog ->EndLog( );
		SAFE_DELETE( pDebugLog )

		// 

	} // -> END DEBUG

	// デバッグマネージャーの破棄
	CDebugManager::Release( );
	
}

// デバッグマネージャーの更新
void CDebugManager::Update( void )
{

	// グラフのデータ更新
	CDebugManager::UpdateGraph( );
	
	{ // 頂点座標の更新
		
		VERTEX_2D *pVtx;									// 仮想アドレス

		// バッファをロックし仮想アドレスを取得する
		pVtxBuf -> Lock(0,0,(void**)&pVtx,0);		// GPUからのVRAMへの操作をロックする

		// 座標の設定
		D3DXVECTOR2 start,end;								// テクスチャ位置情報 始点と終点
		start = D3DXVECTOR2( pos.x - ( SCREEN_WIDTH * 0.15f ), pos.y - ( SCREEN_HEIGHT * 0.15f ) );
		end   = D3DXVECTOR2( pos.x + ( SCREEN_WIDTH * 0.15f ), pos.y + ( SCREEN_HEIGHT * 0.15f ) );

		pVtx[ 0 ].pos = D3DXVECTOR3( start.x, start.y, 0.0f ); 
		pVtx[ 1 ].pos = D3DXVECTOR3( end.x,   start.y, 0.0f ); 
		pVtx[ 2 ].pos = D3DXVECTOR3( start.x, end.y,   0.0f ); 
		pVtx[ 3 ].pos = D3DXVECTOR3( end.x,   end.y,   0.0f ); 

		// バッファのアンロック
		pVtxBuf ->Unlock();
		
	}

}

// デバッグマネージャーの描画
void CDebugManager::Draw( void )
{
	
	// 処理時間グラフの描画
	CDebugManager::DrawGraph( );
	
	{ // ポリゴンの描画
	
		// ストリーム
		pDevice->SetStreamSource(0,
		pVtxBuf,											// どの頂点バッファ―と結ぶか...
		0,													// データのオフセット
		sizeof(VERTEX_2D));									// ストライド値(バイト)

		// 頂点フォーマットの設定
		pDevice ->SetFVF(FVF_VERTEX_2D);

		// テクスチャの設定
		pDevice ->SetTexture( 0, pBackGround );		

		// ポリゴンの描画
		pDevice ->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0 , NUM_POLYGON );

		// テクスチャの設定
		pDevice ->SetTexture( 0, pTexture );		

		// ポリゴンの描画
		pDevice ->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0 , NUM_POLYGON );
	}

}

// 処理時間計測開始
void CDebugManager::StartMeasureProcess( void )
{

	// 周波数設定
	if( !QueryPerformanceFrequency( &freq ) ){

		#ifdef _DEBUG
		CConsole::DebugLog( " OSが機能をサポートしていないため計測が出来ませんでした。" );
		#endif // _DEBUG
		return;
	}
	//QueryPerformanceFrequency(&freq);
	//nFreq[0].QuadPart = def.QuadPart;

	// 計測開始
	QueryPerformanceCounter(&before);

}

// 処理時間計測終了
void CDebugManager::EndMeasureProcess( void )
{
	

	// 計測終了
	if( !QueryPerformanceCounter(&after) ){

		#ifdef _DEBUG
		CConsole::DebugLog( " OSが機能をサポートしていないため計測が出来ませんでした。" );
		#endif // _DEBUG
		return;
	}

	// 処理時間取得
	dwTime[0] = (DWORD)((after.QuadPart - before.QuadPart) * 1000 / freq.QuadPart);

	// データをFIFOでソート
	for( int index = 0; index < PROCESS_COUNT-1; index++ ){
	
		point[ index ].y =  point[ index+1 ].y;
	}

	// 値を設定
	point[ PROCESS_COUNT-1 ].y = (float)dwTime[0] * 10.0f;
	point[ PROCESS_COUNT-1 ].y += (float)( SCREEN_HEIGHT >> 1 );
	#ifdef _DEBUG
	//pDebugLog ->WriteLog( " -- 処理結果 -- \n" );
	#endif // _DEBUG

	for( int index = 0; index < PROCESS_COUNT; index++ ){
	
		point[ index ].x = ((SCREEN_WIDTH / (PROCESS_COUNT-1)) * index);
		#ifdef _DEBUG
		//pDebugLog ->WriteLog( " index : %d \n x : %f  \n y : %f \n ", index, point[index].x, point[index].y );
		#endif // _DEBUG
	}

}

// 処理時間計測開始(指定)
void CDebugManager::StartMeasureProcess( const int index )
{

	// 周波数設定
	if( !QueryPerformanceFrequency( &freq ) ){

		#ifdef _DEBUG
		CConsole::DebugLog( " OSが機能をサポートしていないため計測が出来ませんでした。" );
		#endif // _DEBUG
		return;
	}

	// 計測開始
	QueryPerformanceCounter(&before);


}

// 処理時間計測開始(指定)
void CDebugManager::EndMeasureProcess( const int index )
{


	// 計測終了
	if( !QueryPerformanceCounter(&after) ){

		#ifdef _DEBUG
		CConsole::DebugLog( " OSが機能をサポートしていないため計測が出来ませんでした。" );
		#endif // _DEBUG
		return;
	}

	// 処理時間取得
	dwTime[0] = (DWORD)((after.QuadPart - before.QuadPart) * 1000 / freq.QuadPart);

	// データをFIFOでソート
	for( int index = 0; index < PROCESS_COUNT-1; index++ ){
	
		point[ index ].y =  point[ index+1 ].y;
	}

	// 値を設定
	point[ PROCESS_COUNT-1 ].y = (float)dwTime[0] * 10.0f;
	point[ PROCESS_COUNT-1 ].y += (float)( SCREEN_HEIGHT >> 1 );
	#ifdef _DEBUG
	CConsole::DebugLog( " -- 処理結果 -- \n" );
	#endif // _DEBUG

	for( int index = 0; index < PROCESS_COUNT; index++ ){
	
		point[ index ].x = ((SCREEN_WIDTH / (PROCESS_COUNT-1)) * index);
		#ifdef _DEBUG
		//CConsole::DebugLog( " index : %d \n x : %f  \n y : %f \n ", index, point[index].x, point[index].y );
		#endif // _DEBUG
	}


}

// 処理時間グラフ用データの更新
void CDebugManager::UpdateGraph( void )
{

	// グラフ位置更新
	#ifdef _GAME_EQUATION_H_
	#ifdef _MOUSE_H_
	D3DXVECTOR2 start = D3DXVECTOR2( pos.x - ( SCREEN_WIDTH * 0.15f ), pos.y - ( SCREEN_HEIGHT * 0.15f ) );
	D3DXVECTOR2 end   = D3DXVECTOR2( pos.x + ( SCREEN_WIDTH * 0.15f ), pos.y + ( SCREEN_HEIGHT * 0.15f ) );
	D3DXVECTOR2 coord = D3DXVECTOR2( CMouse::Access()->GetMouseX(), CMouse::Access()->GetMouseY() );
	if( __ObjectTouchSquare( &start, &end, &coord ) ){
		if( CMouse::Access()->GetKeyPress( CMouse::MOUSE_BUTTON_LEFT ) ){

			// マウス座標取得
			pos.x = CMouse::Access()->GetMouseX( );
			pos.y = CMouse::Access()->GetMouseY( );
		}
	}
	#endif // _MOUSE_H_
	#endif // _GAME_EQUATION_H_

}

// 処理時間を線グラフ描画
void CDebugManager::DrawGraph( void )
{


	// レンダリングターゲットをテクスチャに変更
	pDevice->SetRenderTarget( 0 , pSurface );

	
	// サーフェイスのクリア
	pDevice->Clear(0,								// クリアする矩形領域の数
		NULL,										// 矩形領域
		D3DCLEAR_TARGET								// レンダリングターゲットと深度バッファをクリア 
		| D3DCLEAR_ZBUFFER,							// 深度情報
		D3DCOLOR_RGBA( 255,255,255,0 ),				// 色を黒に透明度を100%にクリア
		1.0f,										// Zバッファのクリア値
		0											// ステンシルバッファのクリア値
	);
	
	
	// 線の太さ 
	pLine ->SetWidth( 5.0f ); 

	// 描画開始
	pLine->Begin( );  

	// アルファブレンディングの設定 
	pDevice ->SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE); 
	pDevice ->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA); 
	pDevice ->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA); 

	// グラフ描画
	pLine ->Draw( 
		point,								// 点情報
		PROCESS_COUNT,						// 扱う情報量
		D3DCOLOR_ARGB(255,255,0,0));		// 描画する線の色
	//描画終了
	
	pLine->End( ); 
	
	
	// レンダリングターゲットをバックバッファに変更
	pDevice->SetRenderTarget( 0 , pBackbuf );


}

// デバッグログのポインタの取得
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

// デバッグフォントの取得
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

// デバッグマネージャーの作成
CDebugManager* CDebugManager::Create( void )
{

	// デバイスの取得
	if( !pDevice ){
	
		pDevice = GetDevice( );
	}

	// デバッグマネージャーの作成
	CDebugManager* pDebugManager;
	pDebugManager = new CDebugManager;
	pManager.push_back( pDebugManager );
	pManager[counter] ->myNumber = counter;
	counter++;

	return pDebugManager;

}

// デバッグマネージャーのインデックス番号のポインタの取得
CDebugManager* CDebugManager::IndexAccess( const int number )
{

	// エラーチェック
	if( number >= counter ){
	
		#ifdef _DEBUG
		MessageBox( NULL, "参照番号が正しくありません", " ERROR D_MANAGER ", MB_OK | MB_ICONWARNING );
		#endif	// _DEBUG
		return NULL;
	}

	return pManager[ number ];

}

// 全更新処理
void CDebugManager::UpdateAll( void )
{
	
	for( int index = 0; index < counter; index ++ ){

		if( pManager[ index ] != NULL ){
		
			pManager[ index ] ->Update( );
		}
	}


}

// 全描画処理
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

// 全開放処理
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

// 個別解放処理
void CDebugManager::Release( void )
{
	
	if( pManager[ myNumber ] != NULL ){

		int nID;

		// deleteの際にIDも消去されるので保存する
		nID = myNumber;

		// デバッグマネージャーの削除
		pManager.erase( pManager.begin() + nID );

		// 削除と同時に前詰めされるので、インデックス番号も前詰め
		if( counter != nID ){
			for( int index = nID; index < counter-1; index++ ){
			
				pManager[ nID ] ->myNumber = index;
			}
		}
		//pManager[ counter ] = NULL;
		counter--;
	}

}

// デバッグメッセージ出力
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


