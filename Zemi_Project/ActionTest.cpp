/******************************************
  DirectX画像表示プログラム
*******************************************/

//ヘッダーファイルのインクルード
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>

//ライブラリファイルのロード
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")

//定数とマクロの定義
#define FVF_CUSTOM ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define INITGUID

#define WND_WIDTH 640   //ウィンドウの横幅
#define WND_HEIGHT 480  //ウィンドウの縦幅

#define CHARA_WIDTH 75   //キャラの横幅
#define CHARA_HEIGHT 75  //キャラの縦幅

#define TAMA_WIDTH 37    //弾の横幅
#define TAMA_HEIGHT 22   //弾の縦幅

#define EMY_WIDTH 71     //敵の横幅
#define EMY_HEIGHT 63    //敵の縦幅

#define COIN_WIDTH 39    //コインの横幅
#define COIN_HEIGHT 67   //コインの縦幅

#define MAP_SIZE 32    //マップチップサイズ
#define MAP_CHIP 10    //マップチップの数



//グローバル変数
LPDIRECT3D9 pD3d;             //Direct3Dオブジェクトポインタ
LPDIRECT3DDEVICE9 pD3dDev;    //Direct3Dデバイスポインタ
LPD3DXSPRITE pSprite;         //Direct3Dスプライトポインタ
LPDIRECT3DTEXTURE9 pTexture;     //Direct3Dテクスチャポインタ(プレイヤー)
LPDIRECT3DTEXTURE9 pMapTexture;  //マップチップ
LPDIRECT3DTEXTURE9 pTamaTexture; //弾
LPDIRECT3DTEXTURE9 pEmyTexture;  //敵
LPDIRECT3DTEXTURE9 pCoinTexture; //コイン

LPDIRECTINPUT8 pDinput;       //DirectInputオブジェクトポインタ
LPDIRECTINPUTDEVICE8 pKeyDev; //DirectInputデバイスポインタ

LPD3DXFONT pFont;  //フォントポインタ

static float posX = 290, posY = 0;    //プレイヤーの座標
static float emyX = 520, emyY = 318;  //敵の座標
static float tamaX, tamaY;            //弾の座標
static float coinX = 80, coinY = 318; //コインの座標
static float mapX, mapY;              //マップチップの座標

int hp = 1000;  //ＨＰ
int score = 0;  //スコア
int coin = 0;   //コイン取得枚数

bool tamaFlg = FALSE;  //弾の発射フラグ
bool charaHori = 0;    //プレイヤーの向き(0:左 1:右)

float ReverseH = 1.0;  //画像反転用変数

//プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT InitD3d(HWND);
void InputKey();    //キーボード処理
void DrawRender();  //画像の描画処理
void BulletSet();   //弾の発射処理
void HitTest();     //当たり判定処理
void FreeDx();      //オブジェクトの開放処理


//-----------------------------------------
//メイン関数(エントリポイント)
//-----------------------------------------
int APIENTRY _tWinMain(HINSTANCE hInst, 
	HINSTANCE hPrevInst, LPTSTR lpCmdLine, 
	int nCmdShow)
{
	// ウィンドウの初期化
	HWND hWnd = NULL;
	MSG msg;

	TCHAR szAppName[] = "ゼミ３期_DirectX";  //ウィンドウタイトル
	WNDCLASSEX  wndclass;  //ウィンドウクラス

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInst;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_ASTERISK);
	RegisterClassEx(&wndclass);

	//ウィンドウ作成
	hWnd = CreateWindow(szAppName, szAppName, 
		WS_OVERLAPPEDWINDOW, 0, 0, WND_WIDTH, WND_HEIGHT,
		NULL, NULL, hInst, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Direct3Dの初期化
	if (FAILED(InitD3d(hWnd)))
	{
		return 0;
	}


	// メッセージループ
	ZeroMemory(&msg, sizeof(msg));

	do
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg); 
		}
		else
		{
			InputKey();
			DrawRender();
		}

		Sleep(1);

	} while (msg.message != WM_QUIT);

	//オブジェクトの開放
	FreeDx();

	//アプリの終了
	return (int)msg.wParam;
}


//-----------------------------------------
//ウィンドウプロシージャ
//-----------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT mes, WPARAM wParam, LPARAM lParam)
{
	switch (mes)
	{
	    case WM_DESTROY:
			PostQuitMessage(0);
		    break;
	}

	return DefWindowProc(hWnd, mes, wParam, lParam);
}


//-----------------------------------------
//Direct3Dの初期化
//-----------------------------------------
HRESULT InitD3d(HWND hWnd)
{
	//Direct3Dオブジェクトの生成
	if (NULL == (pD3d = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		MessageBox(0, "Direct3Dの作成失敗", "", MB_OK);
		return E_FAIL;
	}

	//Direct3Dデバイスオブジェクトの作成
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.BackBufferCount = 1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.Windowed = TRUE;

	if (FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &pD3dDev)))
	{
		if (FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &pD3dDev)))
		{
			MessageBox(0, "HALモードでDirect3Dデバイスを作成できません nREFモードで再試行します",
				NULL, MB_OK);

			if (FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING,
				&d3dpp, &pD3dDev)))
			{
				if (FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT,
					D3DDEVTYPE_REF, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
					&d3dpp, &pD3dDev)))
				{
					MessageBox(0, "Direct3Dデバイスを作成できませんでした",
						NULL, MB_OK);

					return E_FAIL;
				}
			}
		}
	}
	
	//テクスチャオブジェクトの作成
	//プレイヤー画像
	if (FAILED(D3DXCreateTextureFromFileEx(pD3dDev,
		"Material/player.png", CHARA_WIDTH, CHARA_HEIGHT,
		0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_FILTER_NONE,
		D3DX_DEFAULT, 0xff000000, NULL, NULL, &pTexture)))
	{
		MessageBox(0, "テクスチャの作成に失敗しました", "", MB_OK);
		return E_FAIL;
	}

	//弾画像
	if (FAILED(D3DXCreateTextureFromFileEx(pD3dDev,
		"Material/tama.png", CHARA_WIDTH, CHARA_HEIGHT,
		0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_FILTER_NONE,
		D3DX_DEFAULT, 0xff000000, NULL, NULL, &pTamaTexture)))
	{
		MessageBox(0, "テクスチャの作成に失敗しました", "", MB_OK);
		return E_FAIL;
	}

	//マップチップ画像
	if (FAILED(D3DXCreateTextureFromFileEx(pD3dDev,
		"Material/wall.bmp", CHARA_WIDTH, CHARA_HEIGHT,
		0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_FILTER_NONE,
		D3DX_DEFAULT, 0xff000000, NULL, NULL, &pMapTexture)))
	{
		MessageBox(0, "テクスチャの作成に失敗しました", "", MB_OK);
		return E_FAIL;
	}

	//敵画像
	if (FAILED(D3DXCreateTextureFromFileEx(pD3dDev,
		"Material/enemy.png", EMY_WIDTH, EMY_HEIGHT,
		0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_FILTER_NONE,
		D3DX_DEFAULT, 0xff000000, NULL, NULL, &pEmyTexture)))
	{
		MessageBox(0, "テクスチャの作成に失敗しました", "", MB_OK);
		return E_FAIL;
	}

	//コイン画像
	if (FAILED(D3DXCreateTextureFromFileEx(pD3dDev,
		"Material/coin.png", COIN_WIDTH, COIN_HEIGHT,
		0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_FILTER_NONE,
		D3DX_DEFAULT, 0xff000000, NULL, NULL, &pCoinTexture)))
	{
		MessageBox(0, "テクスチャの作成に失敗しました", "", MB_OK);
		return E_FAIL;
	}

	//スプライトオブジェクトの作成
	if (FAILED(D3DXCreateSprite(pD3dDev, &pSprite)))
	{
		MessageBox(0, "スプライトの作成に失敗しました", "", MB_OK);
		return E_FAIL;
	}

	//文字列レンダリングの初期化
	if (FAILED(D3DXCreateFont(pD3dDev, 0, 10, FW_REGULAR, NULL,
		FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
		PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "tahoma", &pFont)))
	{
		return E_FAIL;
	}

	return S_OK;
}

//-----------------------------------------
//キーボード処理
//-----------------------------------------
void InputKey()
{
	if (GetKeyState(VK_LEFT) & 0x80)
	{
		posX -= 4;
		ReverseH = -1.0;
	}

	if (GetKeyState(VK_RIGHT) & 0x80)
	{
		posX += 4;
		ReverseH = 1.0;
	}

	if (GetKeyState(VK_UP) & 0x80)
	{
		posY -= 30;
	}

	if (GetKeyState(0x5A) & 0x80)
	{
		if (tamaFlg == FALSE)
		{
			BulletSet();
		}
	}

	HitTest();
}

//-----------------------------------------
//描画処理
//-----------------------------------------
void DrawRender()
{
	int mset = 0;  //マップチップの間隔

	// 描画前に画面をクリア, 背景色を黒で塗りつぶし
	pD3dDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	// 描画
	if (SUCCEEDED(pD3dDev->BeginScene()))
	{
		//各オブジェクトの描画設定
		RECT rect = { 0, 0, CHARA_WIDTH, CHARA_HEIGHT }; //キャラの画像サイズ
		D3DXVECTOR3 v3Center(CHARA_WIDTH / 2, CHARA_HEIGHT / 2, 0); //中心座標
		D3DXVECTOR3 v3Position(posX, posY, 0);  //表示位置

		RECT mRect = { 0, 0, MAP_SIZE, MAP_SIZE };  //ブロックの画像サイズ
		D3DXVECTOR3 v3MCenter(0, 0, 0);

		RECT tRect = { 0, 0, TAMA_WIDTH, TAMA_HEIGHT };  //弾の画像サイズ
		D3DXVECTOR3 v3TCenter(TAMA_WIDTH / 2, TAMA_HEIGHT / 2, 0);
		D3DXVECTOR3 v3TPosition(tamaX, tamaY, 0);

		RECT eRect = { 0,0,EMY_WIDTH, EMY_HEIGHT };  //敵の画像サイズ
		D3DXVECTOR3 v3ECenter(EMY_WIDTH / 2, EMY_HEIGHT / 2, 0);
		D3DXVECTOR3 v3EPosition(emyX, emyY, 0);

		RECT cRect = { 0,0,COIN_WIDTH, COIN_HEIGHT };  //コインの画像サイズ
		D3DXVECTOR3 v3CCenter(COIN_WIDTH / 2, COIN_HEIGHT / 2, 0);
		D3DXVECTOR3 v3CPosition(coinX, coinY, 0);

		//描画開始
		pSprite->Begin(D3DXSPRITE_ALPHABLEND);

		//キャラの描画
		pSprite->Draw(pTexture, &rect, &v3Center,
			&v3Position, D3DCOLOR_ARGB(255, 255, 255, 255));

		//敵の描画
		pSprite->Draw(pEmyTexture, &eRect, &v3ECenter,
			&v3EPosition, D3DCOLOR_ARGB(255, 255, 255, 255));


		//マップの描画
		D3DXVECTOR3 v3MapPos[16]; //描画するブロック数

		for (int i = 0; i < 16; i++)
		{
			v3MapPos[i].x = (float)50 + mset;
			v3MapPos[i].y = 350;
			v3MapPos[i].z = 0;

			pSprite->Draw(pMapTexture, &mRect, &v3MCenter,
				&v3MapPos[i], D3DCOLOR_ARGB(255, 255, 255, 255));

			mset += MAP_SIZE;
		}

		//弾の描画
		if (tamaFlg == TRUE)
		{
			pSprite->Draw(pTamaTexture, &tRect, &v3TCenter,
				&v3TPosition, D3DCOLOR_ARGB(255, 255, 255, 255));

			tamaX += 5;
		}

		//コインの描画
		pSprite->Draw(pCoinTexture, &cRect, &v3CCenter,
			&v3CPosition, D3DCOLOR_ARGB(255, 255, 255, 255));

		//文字列の描画
		TCHAR hStr[100];
		sprintf_s(hStr, "HP : %d", hp);
		RECT textHp = { 50, 50, 0 };
		pFont->DrawText(NULL, hStr, -1, &textHp, DT_CALCRECT, NULL); //サイズ計算
		pFont->DrawText(NULL, hStr, -1, &textHp, DT_LEFT | DT_BOTTOM, 0xff00ff00); //レンダリング

		TCHAR sStr[100];
		sprintf_s(sStr, "SCORE : %d", score);
		RECT textScore = { 50, 20, 0 };
		pFont->DrawText(NULL, sStr, -1, &textScore, DT_CALCRECT, NULL); //サイズ計算
		pFont->DrawText(NULL, sStr, -1, &textScore, DT_LEFT | DT_BOTTOM, 0xff00ff00); //レンダリング

		TCHAR cStr[100];
		sprintf_s(cStr, "COIN : %d", coin);
		RECT textCoin = { 50, 80, 0 };
		pFont->DrawText(NULL, cStr, -1, &textCoin, DT_CALCRECT, NULL); //サイズ計算
		pFont->DrawText(NULL, cStr, -1, &textCoin, DT_LEFT | DT_BOTTOM, 0xff00ff00); //レンダリング
		

		pSprite->End();       //描画終了
		pD3dDev->EndScene();

		pD3dDev->Present(NULL, NULL, NULL, NULL);
	}
}


//-----------------------------------------
//弾の発射
//-----------------------------------------
void BulletSet()
{
	tamaFlg = TRUE;

	//弾の座標値セット
	tamaX = posX + 50;
	tamaY = posY - 7;
}


//-----------------------------------------
//オブジェクトの当たり判定
//-----------------------------------------
void HitTest()
{
	//落下
	posY += 4;

	//キャラの移動制限
	if (posX < 0)   //左
	{
		posX = 0;
	}

	if (posX > 590) //右
	{
		posX = 590;
	}


	//キャラとブロックの当たり判定
	if (posY > 318 && posY < 350 
		&& posX > 50 && posX < 590 )
	{
		posY = 318;
	}

	//敵と弾の当たり判定
	if (tamaX + (TAMA_WIDTH / 2) > emyX - (EMY_WIDTH / 2) &&
		tamaX - (TAMA_WIDTH / 2) < emyX + (EMY_WIDTH / 2) &&
		tamaY + (TAMA_HEIGHT / 2) > emyY - (CHARA_HEIGHT / 2) &&
		tamaY - (TAMA_HEIGHT / 2) < emyY + (CHARA_HEIGHT / 2))
	{
		hp -= 10;      //hp減少
		score += 100;  //スコア加算

		tamaX = 650;

		if (hp < 0)
		{
			hp = 0;
		}

	}

	//弾が画面外に行ったとき
	if (tamaX > 650)
	{
		tamaFlg = FALSE;
	}

	//プレイヤーとコインの辺り判定
	//敵と弾の当たり判定
	if (coinX + (COIN_WIDTH / 2) > posX - (CHARA_WIDTH / 2) &&
		coinX - (COIN_WIDTH / 2) < posX + (CHARA_WIDTH / 2) &&
		coinY + (COIN_HEIGHT / 2) > posY - (CHARA_HEIGHT / 2) &&
		coinY - (COIN_HEIGHT / 2) < posY + (CHARA_HEIGHT / 2))
	{
		coin++;
		coinX = rand() % 500 + 80;
	}
}


//-----------------------------------------
//作成したオブジェクトの開放
//-----------------------------------------
void FreeDx()
{
	SAFE_RELEASE(pFont);
	SAFE_RELEASE(pTexture);
	SAFE_RELEASE(pMapTexture);
	SAFE_RELEASE(pTamaTexture);
	SAFE_RELEASE(pEmyTexture);
	SAFE_RELEASE(pCoinTexture);
	SAFE_RELEASE(pSprite);
	SAFE_RELEASE(pD3dDev);
	SAFE_RELEASE(pD3d);
}