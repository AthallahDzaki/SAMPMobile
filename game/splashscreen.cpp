#include "../main.h"
#include "RW/RenderWare.h"
#include "game.h"
#include "gui/gui.h"
#include "audiostream.h"

extern CGUI *pGUI;
extern CAudioStream *pAudioStream;

RwTexture *splashTexture = nullptr;
void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
void ImGui_ImplRenderWare_NewFrame();

#define MAX_SCHEMAS 4
uint32_t colors[MAX_SCHEMAS][2] = {
	{ COLOR_ROSE,	COLOR_BRED },
	{ COLOR_BLACK, 	COLOR_ORANGE },
	{ COLOR_CYAN,	COLOR_BLUE },
	{ COLOR_1,		COLOR_2 },
};
unsigned int color_scheme = 0;


struct stRect
{
	int x1; // left
	int y1; // top
	int x2; // right
	int y2; // bottom
};

struct stfRect
{
	float x1;
	float y1;
	float x2;
	float y2;
};

void LoadSplashTexture()
{
	Log("Loading splash texture..");
	splashTexture =(RwTexture*) LoadTextureFromDB("samp", "mylogo");
	color_scheme = 1;
}

void Draw(stRect *rect, uint32_t color, RwRaster *raster = nullptr, stfRect *uv = nullptr)
{
	static RwIm2DVertex vert[4];
	const RwReal nearScreenZ 	= *(RwReal*)(g_libGTASA+0x9DAA60);	// CSprite2d::NearScreenZ
	const RwReal recipNearClip 	= *(RwReal*)(g_libGTASA+0x9DAA64);	// CSprite2d::RecipNearClip

	RwIm2DVertexSetScreenX(&vert[0], rect->x1);
	RwIm2DVertexSetScreenY(&vert[0], rect->y2);
	RwIm2DVertexSetScreenZ(&vert[0], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[0], recipNearClip);
	vert[0].emissiveColor = color;
	RwIm2DVertexSetU(&vert[0], uv ? uv->x1 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[0], uv ? uv->y2 : 0.0f, recipNearClip);

	RwIm2DVertexSetScreenX(&vert[1], rect->x2);
	RwIm2DVertexSetScreenY(&vert[1], rect->y2);
	RwIm2DVertexSetScreenZ(&vert[1], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[1], recipNearClip);
	vert[1].emissiveColor = color;
	RwIm2DVertexSetU(&vert[1], uv ? uv->x2 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[1], uv ? uv->y2 : 0.0f, recipNearClip);

	RwIm2DVertexSetScreenX(&vert[2], rect->x1);
	RwIm2DVertexSetScreenY(&vert[2], rect->y1);
	RwIm2DVertexSetScreenZ(&vert[2], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[2], recipNearClip);
	vert[2].emissiveColor = color;
	RwIm2DVertexSetU(&vert[2], uv ? uv->x1 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[2], uv ? uv->y1 : 0.0f, recipNearClip);

	RwIm2DVertexSetScreenX(&vert[3], rect->x2);
	RwIm2DVertexSetScreenY(&vert[3], rect->y1);
	RwIm2DVertexSetScreenZ(&vert[3], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[3], recipNearClip);
	vert[3].emissiveColor = color;
	RwIm2DVertexSetU(&vert[3], uv ? uv->x2 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[3], uv ? uv->y1 : 0.0f, recipNearClip);

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)raster);
	RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vert, 4);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)0);
}

void RenderSplash()
{
	if (!splashTexture) return;

	stRect rect;
	stfRect uv;
	stfRect sRect;

	// background
	rect.x1 = 0;
	rect.y1 = 0;
	rect.x2 = RsGlobal->maximumWidth;
	rect.y2 = RsGlobal->maximumHeight;
	Draw(&rect, colors[color_scheme][0]);

	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);

	// texture размер
	uv.x1 = 0.0f;
	uv.y1 = 0.0f;
	uv.x2 = 1.0;
	uv.y2 = 1.0;
	rect.x1 = RsGlobal->maximumWidth * 0.05;
	rect.y1 = RsGlobal->maximumHeight * 0.25;
	rect.x2 = RsGlobal->maximumWidth - rect.x1;
	rect.y2 = RsGlobal->maximumHeight - rect.y1;
	Draw(&rect, COLOR_WHITE, splashTexture->raster, &uv);

	const float percent = *(float*)(g_libGTASA + 0x8F08C0);
	if (percent <= 0.0f) return;
	if (!pGUI) return;

	float mult = percent / 100.0f;
	// offset
	float newX = pGUI->ScaleX(120.0f) + (mult * 0.9 * RsGlobal->maximumWidth) + pGUI->ScaleX(140.0f);

	ImGui_ImplRenderWare_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO();
	ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(pGUI->ScaleX(120.0f), io.DisplaySize.y * (1 - 0.025)), ImVec2(io.DisplaySize.x - pGUI->ScaleX(120.0f), io.DisplaySize.y - pGUI->ScaleY(60.0f)), ImColor(0x4A, 0x4A, 0x4A, 0xFF - 30)); // серый цвет полоска

	if (newX >= io.DisplaySize.x - pGUI->ScaleX(120.0f)) newX = io.DisplaySize.x - pGUI->ScaleX(120.0f);
	// нижняя полоса
	ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(pGUI->ScaleX(120.0f), io.DisplaySize.y * (1 - 0.025)), ImVec2(newX, io.DisplaySize.y - pGUI->ScaleY(60.0f)), ImColor(COLOR_2));

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());
}

void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
void ImGui_ImplRenderWare_NewFrame();

void RenderSplashScreen()
{
	RenderSplash();

	if(!pGUI) return;

	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplRenderWare_NewFrame();
	ImGui::NewFrame();

	ImGui::GetBackgroundDrawList()->AddText(ImVec2(5, 5), COLOR_2, FLINRP_COPYRIGHT);
	/*
	#ifndef BREAKER
	ImGui::GetOverlayDrawList()->AddText(ImVec2(0, pGUI->ScaleY(1010)), COLOR_2,
		"\n\t"
		u8"protected Flin Mobile\n\t");
	#endif
	*/
    //if(pAudioStream) pAudioStream->Play("https://grggamer.000webhostapp.com/gor2.mp3", 0.0, 0.0, 0.0, 0.0, 0);
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());
}
