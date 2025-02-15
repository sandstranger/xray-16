#include "StdAfx.h"
#include "UIZoneMap.h"

#include "InfoPortion.h"
#include "PDA.h"

#include "Grenade.h"
#include "Level.h"
#include "game_cl_base.h"

#include "Actor.h"
#include "ai_space.h"
#include "xrAICore/Navigation/game_graph.h"

#include "ui/UIMap.h"
#include "ui/UIXmlInit.h"
#include "ui/UIHelper.h"
#include "ui/UIInventoryUtilities.h"
//////////////////////////////////////////////////////////////////////////

void CUIZoneMap::Init(bool motionIconAttached)
{
    ZoneScoped;

    CUIXml uiXml;
    uiXml.Load(CONFIG_PATH, UI_PATH, UI_PATH_DEFAULT, "zone_map.xml");

    CUIXmlInit::InitStatic(uiXml, "minimap:background", 0, &m_background);
    CUIXmlInit::InitStatic(uiXml, "minimap:level_frame", 0, &m_clipFrame);
    CUIXmlInit::InitStatic(uiXml, "minimap:center", 0, &m_center);

    m_clock_wnd = UIHelper::CreateStatic(uiXml, "minimap:clock_wnd", &m_background, false);

    if (IsGameTypeSingle())
    {
        m_pointerDistanceText = UIHelper::CreateStatic(uiXml, "minimap:background:dist_text", &m_background, false);
    }

    m_activeMap = xr_new<CUIMiniMap>();
    m_clipFrame.AttachChild(m_activeMap);
    m_activeMap->SetAutoDelete(true);
    m_activeMap->EnableHeading(true);

    m_activeMap->SetRounded(motionIconAttached);

    CUIXmlInit::InitStatic(uiXml, "minimap:compass", 0, &m_compass);
    m_background.AttachChild(&m_compass);

    m_clipFrame.AttachChild(&m_center);

    m_Counter_text.SetText("");
    visible = true;

    Fvector2 temp;
    const float k = UI().get_current_kx();

    if (motionIconAttached)
    {
        temp = m_clipFrame.GetWndSize();
        temp.y *= UI_BASE_HEIGHT * k;
        temp.x = temp.y / k;
        m_clipFrame.SetWndSize(temp);

        temp = m_clipFrame.GetWndPos();
        m_clipFrame.SetWndPos(temp.mul(UI_BASE_HEIGHT));
    }

    if (motionIconAttached)
    {
        m_background.SetHeight(m_background.GetHeight() * UI_BASE_HEIGHT);
        m_background.SetWidth(m_background.GetHeight() * k);

        m_clipFrame.GetWndRect().getcenter(temp);
        m_background.SetWndPos(temp);
    }

    temp = m_clipFrame.GetWndSize();
    m_center.SetWndPos(temp.div(2.0f));

    if (motionIconAttached)
    {
        temp = m_compass.GetWndPos();
        temp.mul(m_background.GetWndSize());
        m_compass.SetWndPos(temp);
    }

    if (m_clock_wnd && motionIconAttached)
    {
        temp = m_clock_wnd->GetWndPos();
        temp.mul(m_background.GetWndSize());
        m_clock_wnd->SetWndPos(temp);
    }

    if (IsGameTypeSingle())
    {
        CUIXmlInit::InitStatic(uiXml, "minimap:static_counter", 0, &m_Counter);
        m_background.AttachChild(&m_Counter);
        CUIXmlInit::InitStatic(uiXml, "minimap:static_counter:text_static", 0, &m_Counter_text);
        m_Counter.AttachChild(&m_Counter_text);

        if (motionIconAttached)
        {
            temp = m_Counter.GetWndPos();
            temp.mul(m_background.GetWndSize());
            m_Counter.SetWndPos(temp);
        }
    }
}

void CUIZoneMap::Render()
{
    if (!visible)
        return;

    m_clipFrame.Draw();
    m_background.Draw();
}

void CUIZoneMap::Update()
{
    const auto* pActor = smart_cast<CActor*>(Level().CurrentViewEntity());
    if (!pActor)
        return;

    if (!(Device.dwFrame % 20) && IsGameTypeSingle())
    {
        string16 text_str;
        xr_strcpy(text_str, sizeof(text_str), "");

        if (CPda* pda = pActor->GetPDA())
        {
            const u32 cn = pda->ActiveContactsNum();
            if (cn > 0)
            {
                xr_sprintf(text_str, sizeof(text_str), "%d", cn);
            }
        }
        m_Counter_text.SetText(text_str);
    }

    UpdateRadar(Device.vCameraPosition);
    float h, p;
    Device.vCameraDirection.getHP(h, p);
    SetHeading(-h);

    if (m_clock_wnd)
    {
        m_clock_wnd->SetText(GetGameTimeAsString(InventoryUtilities::etpTimeToMinutes).c_str());
    }
}

void CUIZoneMap::SetHeading(float angle)
{
    m_activeMap->SetHeading(angle);
    m_compass.SetHeading(angle);
};

void CUIZoneMap::UpdateRadar(Fvector pos)
{
    m_clipFrame.Update();
    m_background.Update();
    m_activeMap->SetActivePoint(pos);

    if (m_pointerDistanceText)
    {
        if (m_activeMap->GetPointerDistance() > 0.5f)
        {
            string64 str;
            xr_sprintf(str, "%.0f m", m_activeMap->GetPointerDistance());
            m_pointerDistanceText->SetText(str);
        }
        else
        {
            m_pointerDistanceText->SetText("");
        }
    }
}

bool CUIZoneMap::ZoomIn() { return true; }
bool CUIZoneMap::ZoomOut() { return true; }

void CUIZoneMap::SetupCurrentMap()
{
    m_activeMap->Initialize(Level().name(), "hud" DELIMITER "default");

    Frect r;
    m_clipFrame.GetAbsoluteRect(r);
    m_activeMap->WorkingArea().set(r);

    Fvector2 wnd_size;
    float zoom_factor = float(m_clipFrame.GetWidth()) / 100.0f;

    cpcstr ln = Level().name().c_str();
    if (pGameIni->section_exist(ln))
    {
        if (pGameIni->line_exist(ln, "minimap_zoom"))
            zoom_factor *= pGameIni->r_float(ln, "minimap_zoom");
    }
    else if (g_pGameLevel->pLevel->section_exist("minimap_zoom"))
    {
        zoom_factor *= g_pGameLevel->pLevel->r_float("minimap_zoom", "value");
    }
    wnd_size.x = m_activeMap->BoundRect().width() * zoom_factor;
    wnd_size.y = m_activeMap->BoundRect().height() * zoom_factor;
    m_activeMap->SetWndSize(wnd_size);
}

void CUIZoneMap::OnSectorChanged(IRender_Sector::sector_id_t sector)
{
    if (!g_pGameLevel->pLevel->section_exist("sub_level_map"))
        return;
    u8 map_idx = u8(-1);
    string64 s_sector;
    xr_sprintf(s_sector, "%zd", sector);

    if (!g_pGameLevel->pLevel->line_exist("sub_level_map", s_sector))
        return;

    map_idx = g_pGameLevel->pLevel->r_u8("sub_level_map", s_sector);
    if (m_current_map_idx == map_idx)
        return;

    m_current_map_idx = map_idx;

    string_path sub_texture;
    xr_sprintf(sub_texture, "%s#%d", m_activeMap->m_texture.c_str(), m_current_map_idx);

    if (map_idx == u8(-1))
        xr_sprintf(sub_texture, "%s", m_activeMap->m_texture.c_str());

    m_activeMap->InitTextureEx(sub_texture, m_activeMap->m_shader_name.c_str());
}

void CUIZoneMap::Counter_ResetClrAnimation() { m_Counter_text.ResetColorAnimation(); }
