/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <windows.h>

#include "winmidi.h"

void 
winmidiport_config(void);

/******************************************************************************/
/*                                                                            */
/* Midi Monitor                                                               */
/*                                                                            */
/******************************************************************************/

static HINSTANCE winmidi_instance = NULL;
static HWND winmidi_wnd = NULL;
static HANDLE winmidi_thread = NULL;

extern DWORD winmidi_in;
extern DWORD winmidi_out;

char* winmidi_tostring(DWORD midi, char* buf, int len);

BOOL WINAPI 
DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
  winmidi_instance = (HINSTANCE) hModule;
  return TRUE;
}

static long FAR PASCAL 
winmidi_wndproc(HWND hWnd, unsigned message, WPARAM wParam, LPARAM lParam)
{
  HDC hdc;
  RECT r1 = { 20, 10, 180, 30 };
  RECT r2 = { 20, 30, 180, 50 };
  char buf[256];

  switch (message) {
  case WM_CREATE:
    return 0;
    break;

  case WM_PAINT: 
    hdc = GetDC(hWnd); 
    DrawText(hdc, winmidi_tostring(winmidi_in, buf, 256), -1, &r1, DT_TOP | DT_LEFT);
    DrawText(hdc, winmidi_tostring(winmidi_out, buf, 256), -1, &r2, DT_TOP | DT_LEFT);
    return 0;
    break;

  case WM_SIZE: 
    return 0;
    break;

  case WM_DESTROY:
    return 0;
    break;

  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
    break;
  } 
  return(0L);
}

static DWORD WINAPI 
winmidi_monitor_run(LPVOID arg)
{
  MSG msg; 
  WNDCLASS myClass;
  int r;

  myClass.hCursor = LoadCursor(NULL, IDC_ARROW );
  myClass.hIcon = LoadIcon(NULL, IDI_APPLICATION); 
  myClass.lpszMenuName = (LPSTR) NULL;
  myClass.lpszClassName = (LPSTR) "winmidi";
  myClass.hbrBackground = GetStockObject(WHITE_BRUSH);                  
  myClass.hInstance = winmidi_instance;
  myClass.style = CS_HREDRAW | CS_VREDRAW;
  myClass.lpfnWndProc = winmidi_wndproc;
  myClass.cbClsExtra = 0;
  myClass.cbWndExtra = 0;

  if (!RegisterClass(&myClass)) {
    fts_log( "[winmidi]: failed to register the window class\n");
    return -1;
  }

  winmidi_wnd = CreateWindow((LPSTR) "winmidi", (LPSTR) "Midi Monitor", WS_OVERLAPPED,
			     CW_USEDEFAULT, CW_USEDEFAULT, 200, 100, (HWND) NULL, (HMENU) NULL, 
			     winmidi_instance, (LPVOID) NULL);  
  if (winmidi_wnd == NULL) {
    fts_log( "[winmidi]: failed to create the monitor window\n");
    return -1;
  }
  
  ShowWindow(winmidi_wnd, SW_SHOWNORMAL); 
  UpdateWindow(winmidi_wnd); 

  while ((r = GetMessage(&msg, winmidi_wnd, 0, 0))) {
    if (r == -1) {
      break;
    }
    TranslateMessage(&msg); 
    DispatchMessage(&msg); 
  } 
  return 0;
}

static void 
winmidi_monitor_start() 
{
  unsigned long thread_id;
  winmidi_thread = CreateThread(NULL, 0, winmidi_monitor_run, (LPVOID) 0, 0, &thread_id);
  if (winmidi_thread) {
    SetThreadPriority(winmidi_thread, THREAD_PRIORITY_BELOW_NORMAL);
  }
}

static void  
winmidi_monitor_stop() 
{
  if (winmidi_wnd) {
    SendMessage(winmidi_wnd, WM_QUIT, 0, 0);
  }
}

/******************************************************************************/
/*                                                                            */
/* Module congiguration                                                         */
/*                                                                            */
/******************************************************************************/

void 
winmidi_config(void)
{
  winmidiport_config();
/*    winmidi_monitor_start(); */
}

