/**********************************************************************

  Audacity: A Digital Audio Editor

  ThemePrefs.h

  James Crook


**********************************************************************/

#pragma once
#include "PrefsPanel.h"

// ThemePrefs is being developed first under Windows.
// In case it breaks Linux or mac builds, we only use it
// on Windows for now.  Adventurous people can
// try it out by defining the symbol on other platforms
// if they so wish.  16-May-2006.
#ifdef __WXMSW__
#define USE_THEME_PREFS
#endif

class ThemePrefs :
   public PrefsPanel
{
public:
   ThemePrefs(wxWindow * parent);
   ~ThemePrefs(void);
   bool Apply();

   void Populate();
   void OnLoad(wxCommandEvent &event);
   void OnSave(wxCommandEvent &event);
   void OnLoadThemeCache(wxCommandEvent &event);
   void OnSaveThemeCache(wxCommandEvent &event);

   DECLARE_EVENT_TABLE();
};
