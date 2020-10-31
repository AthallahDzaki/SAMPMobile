#include "button.h"
#include "game/game.h"

extern CGUI *pGUI;

struct stButtonRect
{
	int x1;	// left
	int y1;	// top
	int x2;	// right
	int y2;	// bottom
};

struct stfButtonRect
{
	float x1;
	float y1;
	float x2;
	float y2;
};

CButton::CButton()
{
    m_pButtonTexture = nullptr;
    m_bActive = false;
    m_bToggle = false;
}

CButton::~CButton()
{
    if(m_pButtonTexture) delete m_pButtonTexture;
    m_bActive = false;
    m_bToggle = false;
}

void CButton::LoadButtonTexture()
{
    m_pButtonTexture = (RwTexture*)LoadTextureFromDB("samp", "passbutton");
}

void CButton::Toggle()
{
    m_bToggle = !m_bToggle;
}

void CButton::Draw()
{
    if(!m_bToggle) return;

    static RwIm2DVertex vert[4];
	const RwReal nearScreenZ 	= *(RwReal*)(g_libGTASA+0x9DAA60);	// CSprite2d::NearScreenZ
	const RwReal recipNearClip 	= *(RwReal*)(g_libGTASA+0x9DAA64);	// CSprite2d::RecipNearClip

    stButtonRect rect;
    stfButtonRect *uv = nullptr;
    rect.x1 = pGUI->ScaleX(500.0f);
    rect.y1 = pGUI->ScaleY(628.0f);
    rect.x2 = pGUI->ScaleX(628.0f);
    rect.y2 = pGUI->ScaleY(628.0f);

	RwIm2DVertexSetScreenX(&vert[0], rect.x1);
	RwIm2DVertexSetScreenY(&vert[0], rect.y2);
	RwIm2DVertexSetScreenZ(&vert[0], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[0], recipNearClip);
	RwIm2DVertexSetU(&vert[0], uv ? uv->x1 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[0], uv ? uv->y2 : 0.0f, recipNearClip);

	RwIm2DVertexSetScreenX(&vert[1], rect.x2);
	RwIm2DVertexSetScreenY(&vert[1], rect.y2);
	RwIm2DVertexSetScreenZ(&vert[1], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[1], recipNearClip);
	RwIm2DVertexSetU(&vert[1], uv ? uv->x2 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[1], uv ? uv->y2 : 0.0f, recipNearClip);

	RwIm2DVertexSetScreenX(&vert[2], rect.x1);
	RwIm2DVertexSetScreenY(&vert[2], rect.y1);
	RwIm2DVertexSetScreenZ(&vert[2], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[2], recipNearClip);
	RwIm2DVertexSetU(&vert[2], uv ? uv->x1 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[2], uv ? uv->y1 : 0.0f, recipNearClip);

	RwIm2DVertexSetScreenX(&vert[3], rect.x2);
	RwIm2DVertexSetScreenY(&vert[3], rect.y1);
	RwIm2DVertexSetScreenZ(&vert[3], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[3], recipNearClip);
	RwIm2DVertexSetU(&vert[3], uv ? uv->x2 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[3], uv ? uv->y1 : 0.0f, recipNearClip);

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)m_pButtonTexture->raster);
	RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vert, 4);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)0);
}