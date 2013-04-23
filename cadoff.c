/***
 cadoff.c - Copyright (c) 1999 by Th. Thielen
 disables reset via CTRL-ALT-DEL

 Permission is given by the author to freely redistribute and include
 this code in any program as long as this credit is given where due.
 ***/

#define  INCL_BASE
#define  INCL_PM

#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>

/*** DisplayError ********************************************************/ /*FOLD00*/

VOID DisplayError (PSZ pszTitle, PSZ psz)
{
    WinMessageBox (HWND_DESKTOP, HWND_DESKTOP, psz, pszTitle, 0,
                   MB_OK | MB_ERROR | MB_APPLMODAL);
}

/*** wp ******************************************************************/ /*FOLD00*/

static MRESULT EXPENTRY wp (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    return WinDefWindowProc (hwnd, msg, mp1, mp2); }

/*** ToggleCAD ***********************************************************/ /*FOLD00*/

BOOL ToggleCAD (VOID)
{
    ULONG   ulApiRet;
    HFILE   hFile;
    ULONG   ulAct;

    struct
    {
        USHORT usShiftState;
        BYTE   ucHotKeyMake;
        BYTE   ucHotKeyBreak;
        USHORT usHotKeyID;
    } chParms;

    ULONG   ulParm;
    ULONG   ulData;

    if ((ulApiRet = DosOpen ("KBD$", &hFile, &ulAct, 0L, 0L, 1L, 0x40, 0L)))
    {
        DisplayError ("ERROR", "Could not open KBD$.");
        return FALSE;
    }

    memset (&chParms, 0x00, sizeof (chParms));
    chParms.usHotKeyID = 0xffff;

    ulParm = sizeof (chParms);
    ulData = 0;
    ulApiRet = DosDevIOCtl (hFile, 4, 0x56, &chParms, sizeof (chParms),
                            &ulParm, NULL, 0L, &ulData);
    DosClose (hFile);

    if (ulApiRet)
    {
        DisplayError ("ERROR", "Function call to KBD$ failed");
        return FALSE;
    }

    return TRUE;
}

/*** dpAbout *************************************************************/ /*FOLD00*/

MRESULT EXPENTRY dpAbout (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_COMMAND:
        WinDismissDlg (hwnd, 0);
        break;
    }

    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

/*** AboutBox ************************************************************/ /*FOLD00*/

VOID AboutBox (HWND hwnd) {
    WinDlgBox (HWND_DESKTOP, hwnd, dpAbout, 0, 1L, NULL); }

/*** main ****************************************************************/ /*FOLD00*/

int main (int argc, char *argv[])
{
    HAB     hab;
    HMQ     hmq;
    QMSG    qmsg;
    HWND    hwnd;

    hab = WinInitialize (0);
    hmq = WinCreateMsgQueue (hab, 0);

    if (argc > 1)
    {
        AboutBox (HWND_DESKTOP);
        return 1;
    }
    else
    {
        // pressumably disable CTRL-ALT-DEL
        if (ToggleCAD ())
        {
            WinRegisterClass (hab, "thth.wc.cadoff.object", wp, 0L, 0L);
            if ((hwnd = WinCreateWindow (HWND_OBJECT, "thth.wc.cadoff.object",
                                         NULL, 0L, 0,0,0,0, 0, HWND_BOTTOM,
                                         1L, NULL, NULL)))
            {
                while (WinGetMsg (hab, &qmsg, 0L, 0, 0))
                    WinDispatchMsg (hab, &qmsg);

/*                DosBeep (1000, 200);
                DosBeep (800, 200);
                DosBeep (600, 200);
                DosBeep (400, 200);
                DosBeep (200, 200); */

                WinDestroyWindow (hwnd);
            }
            else
                DisplayError ("ERROR", "Could not open object window.");

            // pressumably enable CTRL-ALT-DEL again
            ToggleCAD ();
        }
    }

    WinDestroyMsgQueue (hmq);
    WinTerminate (hab);

    return NO_ERROR;
}

/*************************************************************************/
