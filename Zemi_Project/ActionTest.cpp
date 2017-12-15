/******************************************
  DirectX�摜�\���v���O����
*******************************************/

//�w�b�_�[�t�@�C���̃C���N���[�h
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>

//���C�u�����t�@�C���̃��[�h
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")

//�萔�ƃ}�N���̒�`
#define FVF_CUSTOM ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define INITGUID

#define WND_WIDTH 640   //�E�B���h�E�̉���
#define WND_HEIGHT 480  //�E�B���h�E�̏c��

#define CHARA_WIDTH 75   //�L�����̉���
#define CHARA_HEIGHT 75  //�L�����̏c��

#define TAMA_WIDTH 37    //�e�̉���
#define TAMA_HEIGHT 22   //�e�̏c��

#define EMY_WIDTH 71     //�G�̉���
#define EMY_HEIGHT 63    //�G�̏c��

#define COIN_WIDTH 39    //�R�C���̉���
#define COIN_HEIGHT 67   //�R�C���̏c��

#define MAP_SIZE 32    //�}�b�v�`�b�v�T�C�Y
#define MAP_CHIP 10    //�}�b�v�`�b�v�̐�



//�O���[�o���ϐ�
LPDIRECT3D9 pD3d;             //Direct3D�I�u�W�F�N�g�|�C���^
LPDIRECT3DDEVICE9 pD3dDev;    //Direct3D�f�o�C�X�|�C���^
LPD3DXSPRITE pSprite;         //Direct3D�X�v���C�g�|�C���^
LPDIRECT3DTEXTURE9 pTexture;     //Direct3D�e�N�X�`���|�C���^(�v���C���[)
LPDIRECT3DTEXTURE9 pMapTexture;  //�}�b�v�`�b�v
LPDIRECT3DTEXTURE9 pTamaTexture; //�e
LPDIRECT3DTEXTURE9 pEmyTexture;  //�G
LPDIRECT3DTEXTURE9 pCoinTexture; //�R�C��

LPDIRECTINPUT8 pDinput;       //DirectInput�I�u�W�F�N�g�|�C���^
LPDIRECTINPUTDEVICE8 pKeyDev; //DirectInput�f�o�C�X�|�C���^

LPD3DXFONT pFont;  //�t�H���g�|�C���^

static float posX = 290, posY = 0;    //�v���C���[�̍��W
static float emyX = 520, emyY = 318;  //�G�̍��W
static float tamaX, tamaY;            //�e�̍��W
static float coinX = 80, coinY = 318; //�R�C���̍��W
static float mapX, mapY;              //�}�b�v�`�b�v�̍��W

int hp = 1000;  //�g�o
int score = 0;  //�X�R�A
int coin = 0;   //�R�C���擾����

bool tamaFlg = FALSE;  //�e�̔��˃t���O
bool charaHori = 0;    //�v���C���[�̌���(0:�� 1:�E)

float ReverseH = 1.0;  //�摜���]�p�ϐ�

//�v���g�^�C�v�錾
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT InitD3d(HWND);
void InputKey();    //�L�[�{�[�h����
void DrawRender();  //�摜�̕`�揈��
void BulletSet();   //�e�̔��ˏ���
void HitTest();     //�����蔻�菈��
void FreeDx();      //�I�u�W�F�N�g�̊J������


//-----------------------------------------
//���C���֐�(�G���g���|�C���g)
//-----------------------------------------
int APIENTRY _tWinMain(HINSTANCE hInst, 
	HINSTANCE hPrevInst, LPTSTR lpCmdLine, 
	int nCmdShow)
{
	// �E�B���h�E�̏�����
	HWND hWnd = NULL;
	MSG msg;

	TCHAR szAppName[] = "�[�~�R��_DirectX";  //�E�B���h�E�^�C�g��
	WNDCLASSEX  wndclass;  //�E�B���h�E�N���X

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

	//�E�B���h�E�쐬
	hWnd = CreateWindow(szAppName, szAppName, 
		WS_OVERLAPPEDWINDOW, 0, 0, WND_WIDTH, WND_HEIGHT,
		NULL, NULL, hInst, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Direct3D�̏�����
	if (FAILED(InitD3d(hWnd)))
	{
		return 0;
	}


	// ���b�Z�[�W���[�v
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

	//�I�u�W�F�N�g�̊J��
	FreeDx();

	//�A�v���̏I��
	return (int)msg.wParam;
}


//-----------------------------------------
//�E�B���h�E�v���V�[�W��
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
//Direct3D�̏�����
//-----------------------------------------
HRESULT InitD3d(HWND hWnd)
{
	//Direct3D�I�u�W�F�N�g�̐���
	if (NULL == (pD3d = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		MessageBox(0, "Direct3D�̍쐬���s", "", MB_OK);
		return E_FAIL;
	}

	//Direct3D�f�o�C�X�I�u�W�F�N�g�̍쐬
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
			MessageBox(0, "HAL���[�h��Direct3D�f�o�C�X���쐬�ł��܂��� nREF���[�h�ōĎ��s���܂�",
				NULL, MB_OK);

			if (FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING,
				&d3dpp, &pD3dDev)))
			{
				if (FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT,
					D3DDEVTYPE_REF, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
					&d3dpp, &pD3dDev)))
				{
					MessageBox(0, "Direct3D�f�o�C�X���쐬�ł��܂���ł���",
						NULL, MB_OK);

					return E_FAIL;
				}
			}
		}
	}
	
	//�e�N�X�`���I�u�W�F�N�g�̍쐬
	//�v���C���[�摜
	if (FAILED(D3DXCreateTextureFromFileEx(pD3dDev,
		"Material/player.png", CHARA_WIDTH, CHARA_HEIGHT,
		0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_FILTER_NONE,
		D3DX_DEFAULT, 0xff000000, NULL, NULL, &pTexture)))
	{
		MessageBox(0, "�e�N�X�`���̍쐬�Ɏ��s���܂���", "", MB_OK);
		return E_FAIL;
	}

	//�e�摜
	if (FAILED(D3DXCreateTextureFromFileEx(pD3dDev,
		"Material/tama.png", CHARA_WIDTH, CHARA_HEIGHT,
		0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_FILTER_NONE,
		D3DX_DEFAULT, 0xff000000, NULL, NULL, &pTamaTexture)))
	{
		MessageBox(0, "�e�N�X�`���̍쐬�Ɏ��s���܂���", "", MB_OK);
		return E_FAIL;
	}

	//�}�b�v�`�b�v�摜
	if (FAILED(D3DXCreateTextureFromFileEx(pD3dDev,
		"Material/wall.bmp", CHARA_WIDTH, CHARA_HEIGHT,
		0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_FILTER_NONE,
		D3DX_DEFAULT, 0xff000000, NULL, NULL, &pMapTexture)))
	{
		MessageBox(0, "�e�N�X�`���̍쐬�Ɏ��s���܂���", "", MB_OK);
		return E_FAIL;
	}

	//�G�摜
	if (FAILED(D3DXCreateTextureFromFileEx(pD3dDev,
		"Material/enemy.png", EMY_WIDTH, EMY_HEIGHT,
		0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_FILTER_NONE,
		D3DX_DEFAULT, 0xff000000, NULL, NULL, &pEmyTexture)))
	{
		MessageBox(0, "�e�N�X�`���̍쐬�Ɏ��s���܂���", "", MB_OK);
		return E_FAIL;
	}

	//�R�C���摜
	if (FAILED(D3DXCreateTextureFromFileEx(pD3dDev,
		"Material/coin.png", COIN_WIDTH, COIN_HEIGHT,
		0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_FILTER_NONE,
		D3DX_DEFAULT, 0xff000000, NULL, NULL, &pCoinTexture)))
	{
		MessageBox(0, "�e�N�X�`���̍쐬�Ɏ��s���܂���", "", MB_OK);
		return E_FAIL;
	}

	//�X�v���C�g�I�u�W�F�N�g�̍쐬
	if (FAILED(D3DXCreateSprite(pD3dDev, &pSprite)))
	{
		MessageBox(0, "�X�v���C�g�̍쐬�Ɏ��s���܂���", "", MB_OK);
		return E_FAIL;
	}

	//�����񃌃��_�����O�̏�����
	if (FAILED(D3DXCreateFont(pD3dDev, 0, 10, FW_REGULAR, NULL,
		FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
		PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "tahoma", &pFont)))
	{
		return E_FAIL;
	}

	return S_OK;
}

//-----------------------------------------
//�L�[�{�[�h����
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
//�`�揈��
//-----------------------------------------
void DrawRender()
{
	int mset = 0;  //�}�b�v�`�b�v�̊Ԋu

	// �`��O�ɉ�ʂ��N���A, �w�i�F�����œh��Ԃ�
	pD3dDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	// �`��
	if (SUCCEEDED(pD3dDev->BeginScene()))
	{
		//�e�I�u�W�F�N�g�̕`��ݒ�
		RECT rect = { 0, 0, CHARA_WIDTH, CHARA_HEIGHT }; //�L�����̉摜�T�C�Y
		D3DXVECTOR3 v3Center(CHARA_WIDTH / 2, CHARA_HEIGHT / 2, 0); //���S���W
		D3DXVECTOR3 v3Position(posX, posY, 0);  //�\���ʒu

		RECT mRect = { 0, 0, MAP_SIZE, MAP_SIZE };  //�u���b�N�̉摜�T�C�Y
		D3DXVECTOR3 v3MCenter(0, 0, 0);

		RECT tRect = { 0, 0, TAMA_WIDTH, TAMA_HEIGHT };  //�e�̉摜�T�C�Y
		D3DXVECTOR3 v3TCenter(TAMA_WIDTH / 2, TAMA_HEIGHT / 2, 0);
		D3DXVECTOR3 v3TPosition(tamaX, tamaY, 0);

		RECT eRect = { 0,0,EMY_WIDTH, EMY_HEIGHT };  //�G�̉摜�T�C�Y
		D3DXVECTOR3 v3ECenter(EMY_WIDTH / 2, EMY_HEIGHT / 2, 0);
		D3DXVECTOR3 v3EPosition(emyX, emyY, 0);

		RECT cRect = { 0,0,COIN_WIDTH, COIN_HEIGHT };  //�R�C���̉摜�T�C�Y
		D3DXVECTOR3 v3CCenter(COIN_WIDTH / 2, COIN_HEIGHT / 2, 0);
		D3DXVECTOR3 v3CPosition(coinX, coinY, 0);

		//�`��J�n
		pSprite->Begin(D3DXSPRITE_ALPHABLEND);

		//�L�����̕`��
		pSprite->Draw(pTexture, &rect, &v3Center,
			&v3Position, D3DCOLOR_ARGB(255, 255, 255, 255));

		//�G�̕`��
		pSprite->Draw(pEmyTexture, &eRect, &v3ECenter,
			&v3EPosition, D3DCOLOR_ARGB(255, 255, 255, 255));


		//�}�b�v�̕`��
		D3DXVECTOR3 v3MapPos[16]; //�`�悷��u���b�N��

		for (int i = 0; i < 16; i++)
		{
			v3MapPos[i].x = (float)50 + mset;
			v3MapPos[i].y = 350;
			v3MapPos[i].z = 0;

			pSprite->Draw(pMapTexture, &mRect, &v3MCenter,
				&v3MapPos[i], D3DCOLOR_ARGB(255, 255, 255, 255));

			mset += MAP_SIZE;
		}

		//�e�̕`��
		if (tamaFlg == TRUE)
		{
			pSprite->Draw(pTamaTexture, &tRect, &v3TCenter,
				&v3TPosition, D3DCOLOR_ARGB(255, 255, 255, 255));

			tamaX += 5;
		}

		//�R�C���̕`��
		pSprite->Draw(pCoinTexture, &cRect, &v3CCenter,
			&v3CPosition, D3DCOLOR_ARGB(255, 255, 255, 255));

		//������̕`��
		TCHAR hStr[100];
		sprintf_s(hStr, "HP : %d", hp);
		RECT textHp = { 50, 50, 0 };
		pFont->DrawText(NULL, hStr, -1, &textHp, DT_CALCRECT, NULL); //�T�C�Y�v�Z
		pFont->DrawText(NULL, hStr, -1, &textHp, DT_LEFT | DT_BOTTOM, 0xff00ff00); //�����_�����O

		TCHAR sStr[100];
		sprintf_s(sStr, "SCORE : %d", score);
		RECT textScore = { 50, 20, 0 };
		pFont->DrawText(NULL, sStr, -1, &textScore, DT_CALCRECT, NULL); //�T�C�Y�v�Z
		pFont->DrawText(NULL, sStr, -1, &textScore, DT_LEFT | DT_BOTTOM, 0xff00ff00); //�����_�����O

		TCHAR cStr[100];
		sprintf_s(cStr, "COIN : %d", coin);
		RECT textCoin = { 50, 80, 0 };
		pFont->DrawText(NULL, cStr, -1, &textCoin, DT_CALCRECT, NULL); //�T�C�Y�v�Z
		pFont->DrawText(NULL, cStr, -1, &textCoin, DT_LEFT | DT_BOTTOM, 0xff00ff00); //�����_�����O
		

		pSprite->End();       //�`��I��
		pD3dDev->EndScene();

		pD3dDev->Present(NULL, NULL, NULL, NULL);
	}
}


//-----------------------------------------
//�e�̔���
//-----------------------------------------
void BulletSet()
{
	tamaFlg = TRUE;

	//�e�̍��W�l�Z�b�g
	tamaX = posX + 50;
	tamaY = posY - 7;
}


//-----------------------------------------
//�I�u�W�F�N�g�̓����蔻��
//-----------------------------------------
void HitTest()
{
	//����
	posY += 4;

	//�L�����̈ړ�����
	if (posX < 0)   //��
	{
		posX = 0;
	}

	if (posX > 590) //�E
	{
		posX = 590;
	}


	//�L�����ƃu���b�N�̓����蔻��
	if (posY > 318 && posY < 350 
		&& posX > 50 && posX < 590 )
	{
		posY = 318;
	}

	//�G�ƒe�̓����蔻��
	if (tamaX + (TAMA_WIDTH / 2) > emyX - (EMY_WIDTH / 2) &&
		tamaX - (TAMA_WIDTH / 2) < emyX + (EMY_WIDTH / 2) &&
		tamaY + (TAMA_HEIGHT / 2) > emyY - (CHARA_HEIGHT / 2) &&
		tamaY - (TAMA_HEIGHT / 2) < emyY + (CHARA_HEIGHT / 2))
	{
		hp -= 10;      //hp����
		score += 100;  //�X�R�A���Z

		tamaX = 650;

		if (hp < 0)
		{
			hp = 0;
		}

	}

	//�e����ʊO�ɍs�����Ƃ�
	if (tamaX > 650)
	{
		tamaFlg = FALSE;
	}

	//�v���C���[�ƃR�C���̕ӂ蔻��
	//�G�ƒe�̓����蔻��
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
//�쐬�����I�u�W�F�N�g�̊J��
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