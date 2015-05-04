/*
 *  This file and the code contained within it is (c)2001 Energon Software
 *      please direct any comments/questions to energon@micron.net
 *
 *  Any replications, duplications, uses, copies, etc. of this code must be
 *      accompanied by the original, unedited source for this program.
 *      Any non-compliances with this agreement will be taken under
 *      the extent of all intellectual property copyright laws
 *
 */


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "resource.h"


// constants
const int MILLISECONDS = 10;
const int TIMER_ID     = 1;


// prototypes
LRESULT CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI timing_thread(void* data);


// global variables
HWND   hDlgT;
HANDLE h_timing_thread;
DWORD  timing_thread_id;
int    score = 0;


// WinMain()
int WINAPI WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nShowCmd)
{
    srand((unsigned)time(NULL));

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_REFTEST), NULL, (DLGPROC)DlgProc);

    while(WaitForSingleObject(h_timing_thread, INFINITE) == WAIT_TIMEOUT);
    CloseHandle(h_timing_thread);

    return FALSE;
}


// DlgProc()
LRESULT CALLBACK DlgProc(HWND hDlg,
						 UINT uMsg,
						 WPARAM wParam,
						 LPARAM lParam)
{
    char s_score[32];
    char insult[1024];
    hDlgT = hDlg;

	switch(uMsg)
	{
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case ID_START:
            // don't show the score
            ShowWindow(GetDlgItem(hDlg, IDC_SCORE), SW_HIDE);

            // don't show any insults
            ShowWindow(GetDlgItem(hDlg, IDC_INSULT), SW_HIDE);

            // disable the start button
            EnableWindow(GetDlgItem(hDlg, ID_START), FALSE);

            // start timing thread
            h_timing_thread = CreateThread(NULL, 0, timing_thread, NULL, 0, &timing_thread_id);
            break;
        case ID_STOP:
            // stop the timer
            KillTimer(hDlgT, TIMER_ID);

            // wait for the timing thread to stop
            while(WaitForSingleObject(h_timing_thread, INFINITE) == WAIT_TIMEOUT);
            CloseHandle(h_timing_thread);

            // disable stop button
            EnableWindow(GetDlgItem(hDlg, ID_STOP), FALSE);

            // figure out score and display it
            memset((void*)s_score, 0, sizeof(s_score));
            sprintf(s_score, "Your score: %i", score);
			SetDlgItemText(hDlg, IDC_SCORE, s_score);
            ShowWindow(GetDlgItem(hDlg, IDC_SCORE), SW_SHOW);

            // figure insult and display it
            memset((void*)insult, 0, sizeof(insult));
            if(0 == score)
                sprintf(insult, "Holy shit! You've ass fucked me!");
            else if(score > 0 && score <= 5)
                sprintf(insult, "I thought I saw something move...");
            else if(score > 5 && score <= 10)
                sprintf(insult, "Are you even trying?");
            else if(score > 10 && score <= 20)
                sprintf(insult, "Have trouble pushing the button there?");
            else if(score > 20)
                sprintf(insult, "Did you push the button? I can't tell...");
            else
                sprintf(insult, "Did you fall asleep?");
            score = 0;

            SetDlgItemText(hDlg, IDC_INSULT, insult);
            ShowWindow(GetDlgItem(hDlg, IDC_INSULT), SW_SHOW);

            // enable the start button
            EnableWindow(GetDlgItem(hDlg, ID_START), TRUE);
            break;
        case ID_ABOUT:
            MessageBox(hDlgT, "ReflexTest by Energon Software", "About", MB_OK);
            break;
        case ID_EXIT:
            EndDialog(hDlg, LOWORD(wParam));
            break;
        }
    case WM_TIMER:
        score++;
        break;
    case WM_CLOSE:
    case WM_DESTROY:
        EndDialog(hDlg, LOWORD(wParam));
        break;
	}

	return FALSE;
}


// timing_thread()
DWORD WINAPI timing_thread(void* data)
{
    // generate random sleep time (between 0 and 4000)
    int i=0;
    int r = ((rand()%4) * 1000) + (rand()%10 * 100) + (rand()%10 * 10);

    Sleep(r);
    EnableWindow(GetDlgItem(hDlgT, ID_STOP), TRUE);
    SetTimer(hDlgT, TIMER_ID, MILLISECONDS, NULL);
    
    return (DWORD)data;
}