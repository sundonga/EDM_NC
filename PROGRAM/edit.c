#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include <stdarg.h>
#include <ctype.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <pwd.h>

#include <math.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mywindows.h>
#include <minigui/newfiledlg.h>
#include <minigui/filedlg.h>
#include <minigui/mgext.h>

#include "edit.h"
#include "thread_decode.h"

extern HWND hEEdit,hEProIn;
/***************************�༭ģ��**********************************/
/********************************************************************/
/********************************************************************/

static HWND hEditWnd = HWND_INVALID; //�༭�Ի�����
//PNOTEINFO pNoteInfo = NULL;      // ��ų���༭����Ϣ�Ľṹ��ָ��
inline double Gcode2d(char* s, const char* gcode); /*���ַ���s���ַ���gcode�����ʵ�����ַ���, gcode������һ���ַ�!*/
inline int Gcode2i(char* s, const char* gcode); /*���ַ���s���ַ���gcode������������ַ���, gcode������һ���ַ�!*/
extern int stringnum(char *p, const char* s); /*�����ַ���p�к����ַ���s�ĸ���, s������һ���ַ�*/

static void GetFileAndDirList( HWND hwnd,char* path);
static void instolistview (HWND hwnd,PFILEINFO pfi);

int pp=0;

/*��ʼ��pNoteInfo�ṹ��ָ��*/
void InitpNoteInfo()
{
	char currentpath [PATH_MAX + 1];
	getcwd(currentpath,PATH_MAX);
    
        //if(pNoteInfo == NULL)
	if(!(pNoteInfo = (PNOTEINFO)malloc(sizeof(NOTEINFO)))) return ; //error!!
        pNoteInfo->ischanged = FALSE;
        strcpy(pNoteInfo->fileName , "untitled.txt");
        strcpy(pNoteInfo->filePath , currentpath);
	if (pNoteInfo->filePath [strlen (pNoteInfo->filePath) - 1] != '/')
        	strcat (pNoteInfo->filePath, "/");
        pNoteInfo->fileSize = 0;
        pNoteInfo->Buffer = NULL;
        pNoteInfo->hMLEditWnd = HWND_INVALID;
}

//����༭��ص�����
void mledit_notif_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
	SendMessage(hwnd, EM_SETLFDISPCHAR, 0, ';'); 
	if(nc == EN_CHANGE){
		pNoteInfo->ischanged = TRUE;
	}
}

static char* GetParentDir (char *dir)
{
    int i, nParent = 0;
    
    for (i = 0; i < strlen (dir) -1; i++) 
       if (dir [i] == '/') 
            nParent = i;

    if (nParent == 0)
        dir [nParent + 1] = 0;
    else 
        dir [nParent] = 0;
       
    return dir;
}
/*
//���ñ༭���Ϸ��ľ�̬���ı�(��ʾ�ļ�·�����ļ���)
void SetStaticEditText(hwnd)
{
	char str[90]="Program Input: ";
	strcat(str, pNoteInfo->filePath);
	strcat(str, pNoteInfo->fileName);
	SetWindowText(GetDlgItem(hwnd, IDC_STATICEDIT), str);
}*/

/*�༭�Ի���ص�����*/

/*�����༭�Ի���*/

BOOL NBSave (HWND hMLEditWnd)
{
	char chPath[PATH_MAX+1];
	char chName[NAME_MAX+1];
	char chFullName[PATH_MAX+NAME_MAX+1];
	int choise=0;
	FILE *file;
	char buffer[102400];
	long reallength=0;

      	memset (chFullName, 0, PATH_MAX+NAME_MAX + 1);

	GetWindowText (GetDlgItem(hEEdit, IDC_EPATH), chPath, PATH_MAX);

	GetWindowText (GetDlgItem(hEEdit, IDC_EFNAME), chName, NAME_MAX);

	strcpy(chFullName,chPath);
	strcat(chFullName,"/");
	strcat(chFullName,chName);
//	umask (S_IXGRP | S_IWOTH);
	if ((file = fopen (chFullName, "w+")) == NULL)
        {	
		MessageBox (GetDlgItem(hEEdit, IDC_BTNSAVE),"open file err","Program Input", MB_OK | MB_ICONSTOP);
		
		pNoteInfo->ischanged = FALSE;
		GetFileAndDirList (hEProIn, chPath);
		ShowWindow(hEProIn,SW_SHOW);	
		ShowWindow(GetDlgItem(hEEdit,IDC_MLEDIT),SW_HIDE);
	 	SetWindowText(GetDlgItem(hEEdit, IDC_EFNAME),"");
		return FALSE;
        }
	else {
		reallength = GetWindowTextLength(hMLEditWnd);
		GetWindowText(hMLEditWnd,buffer,102400);
      
	        if (fwrite(buffer, 1, reallength, file) < 0)  {
        		MessageBox (GetDlgItem(hEEdit, IDC_BTNSAVE),"write file err","Program Input", MB_OK | MB_ICONEXCLAMATION);
			pNoteInfo->ischanged = FALSE;
	                fclose (file);
			GetFileAndDirList (hEProIn, chPath);
	        	ShowWindow(hEProIn,SW_SHOW);
		        ShowWindow(GetDlgItem(hEEdit,IDC_MLEDIT),SW_HIDE);
	        	SetWindowText(GetDlgItem(hEEdit, IDC_EFNAME),"");
		}

	        pNoteInfo->ischanged = FALSE;
        	fclose (file);
	}
	GetFileAndDirList (hEProIn, chPath);
	ShowWindow(hEProIn,SW_SHOW);	
	ShowWindow(GetDlgItem(hEEdit,IDC_MLEDIT),SW_HIDE);
 	SetWindowText(GetDlgItem(hEEdit, IDC_EFNAME),"");
	
        return TRUE;
}

void open_notif_proc(HWND hwnd, int id, int nc, DWORD add_data)
{
	if (nc == BN_CLICKED)
	{
		int  nSelItem;
		int  nIsDir;
		LVSUBITEM subItem;
		
		int choise=0, fd;
		long reallength=0;
		char buffer[204800];
		
		nSelItem = SendMessage (hEProIn, LVM_GETSELECTEDITEM, 0, 0);
                if (nSelItem > 0 ) {
			nIsDir = SendMessage (hEProIn, LVM_GETITEMADDDATA, 0, nSelItem);
			memset (&subItem, 0, sizeof (subItem));
			subItem.subItem = 0;
			subItem.pszText = (char *)calloc (NAME_MAX + 1, 1);
			if (subItem.pszText == NULL) 
			return;
			SendMessage (hEProIn, LVM_GETSUBITEMTEXT,  (WPARAM)nSelItem, (LPARAM)&subItem);
			
			if(!OpenFile(buffer,subItem.pszText,hEProIn)) return;
			
			ShowWindow(GetDlgItem(hEEdit,IDC_MLEDIT),SW_SHOW);
			ShowWindow(hEProIn,SW_HIDE);
			SetWindowText (GetDlgItem(hEEdit,IDC_MLEDIT), buffer);
		
			SetWindowText(GetDlgItem(hEEdit,IDC_EFNAME),subItem.pszText);
			if(subItem.pszText!=NULL) free(subItem.pszText);
		}
	}
}

BOOL OpenFile(char *buffer, char *filename, HWND hParent)
{
    int  fd;
    int reallength=0;

       if ( access (filename, F_OK) < 0)
            MessageBox (hParent, "no this file","Program", MB_OK | MB_ICONSTOP);
        else if ( access (filename, R_OK) < 0)
            MessageBox (hParent, "cann't read file","Program", MB_OK | MB_ICONSTOP);
        else 
        {
            if ( access (filename, W_OK) < 0)
                MessageBox (hParent, "cann't write file","Program", MB_OK | MB_ICONEXCLAMATION);
            fd = open(filename, O_RDONLY);
            if (fd <= 0)
            {
                 MessageBox (hParent, "open file fault","Program", MB_OK | MB_ICONSTOP);
                 return FALSE;
            }
            if ((reallength=read(fd, buffer, 204799)) >= 204799) 
                 MessageBox (hParent, "file cutted","Program", MB_OK | MB_ICONEXCLAMATION);
            close (fd);
            buffer[reallength]=0; //���λ�� ' \0 '
            
            return TRUE;
        }
    return FALSE;
}


void new_notif_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
	HWND hCtrlWnd;
	PNEWFILEDLGDATA ppfile;
	int choise=0;
	char str[50];
	char filename[NAME_MAX+1];

	if (nc == BN_CLICKED){
		switch(id) {
		case IDC_BTNNEW:
			if(pNoteInfo->ischanged)
			{
				hCtrlWnd=GetDlgItem(hEEdit,IDC_EFNAME);
				GetWindowText (hCtrlWnd, filename, NAME_MAX);
				strcpy(str,"do you want save file");
				strcat(str,filename);
				strcat(str,"?");

				choise = MessageBox (hwnd,
					str,
					"Save File",
					MB_YESNOCANCEL | MB_ICONQUESTION);
				hCtrlWnd=GetDlgItem(hEEdit,IDC_MLEDIT);
				if(choise=IDYES)
				NBSave(hCtrlWnd);
			}
			hCtrlWnd=GetDlgItem(hEEdit,IDC_MLEDIT);
			ShowWindow(hCtrlWnd,SW_SHOW);
			ShowWindow(hEProIn,SW_HIDE);
			SetWindowText(hCtrlWnd,"");

			hCtrlWnd=GetDlgItem(hEEdit,IDC_EFNAME);
			SetWindowText (hCtrlWnd,"untitled.txt");
									
		break;
		case IDC_BTNSAVE:
			if(pNoteInfo->ischanged)
                        {
                                hCtrlWnd=GetDlgItem(hEEdit,IDC_EFNAME);
                                GetWindowText (hCtrlWnd, filename, NAME_MAX);
                                strcpy(str,"do you want save file ");
                                strcat(str,filename);
                                strcat(str,"?");
                                                                                                                       
                                choise = MessageBox (hwnd,
                                        str,
                                        "Save File",
                                        MB_YESNOCANCEL | MB_ICONQUESTION);
                                hCtrlWnd=GetDlgItem(hEEdit,IDC_MLEDIT);
                                if(choise == IDYES)
                                NBSave(hCtrlWnd);
                        }
		break;
		}
	}
}


static void InitPathCombo(char *path)
{
    HWND hCtrlWnd;
    char chSubPath[PATH_MAX];
    char chPath[PATH_MAX];
    char *pStr;

    if (path == NULL) return;

    strcpy (chPath, path);
    strtrimall (chPath);
    if ( strlen (chPath) == 0 ) return;

    if (strcmp (chPath, "/") != 0 &&  chPath[strlen(chPath)-1] == '/') {
        chPath [strlen (chPath) - 1] = 0;
    }

    hCtrlWnd = GetDlgItem (hEEdit, IDC_EPATH);
    SendMessage (hCtrlWnd, CB_RESETCONTENT, 0, 0);
    SendMessage (hCtrlWnd, CB_SETITEMHEIGHT, 0, (LPARAM)GetSysCharHeight()+2);
    
    strcpy(chSubPath, "/");
    SendMessage (hCtrlWnd, CB_ADDSTRING, 0,(LPARAM)chSubPath);
    
    pStr = strchr(chPath + 1, '/');
    while (pStr != NULL){
        memset (chSubPath, 0, sizeof (chSubPath));
        strncpy (chSubPath, chPath, pStr -chPath);
        SendMessage (hCtrlWnd, CB_INSERTSTRING, 0,(LPARAM)chSubPath);
        pStr = strchr (chPath + (pStr -chPath +1), '/');
    }
    
    if (strcmp (chPath, "/") != 0 ){
        SendMessage (hCtrlWnd, CB_INSERTSTRING, 0,(LPARAM)chPath);
    }

    SetWindowText (hCtrlWnd, chPath);            
}

//*****************************************************************************************************************************
char * caption [] =
{
#ifdef _LANG_ZHCN
    "filename", "size", "datemodified"
#else
    "Name","Size", "DateModified"
#endif
};
void listviewproc(HWND hwnd)
{
	int i, j;
        LVITEM item;
        LVCOLUMN lvcol;
        GHANDLE hitem;

	PNEWFILEDLGDATA ppfile;

	getcwd (ppfile->filepath, PATH_MAX);
	InitPathCombo ( ppfile->filepath); 
		
        for (i = 0; i <3; i++) {
            lvcol.nCols = i;
            lvcol.pszHeadText = caption[i];
            lvcol.width = 160;
            lvcol.pfnCompare = NULL;
            lvcol.colFlags = 0;
            SendMessage (hwnd, LVM_ADDCOLUMN, 0, (LPARAM) &lvcol);
        }
	item.nItemHeight=30;
	GetFileAndDirList(hwnd,ppfile->filepath);
	
}
static void GetFileAndDirList( HWND hwnd,char* path)
{
	SendMessage (hwnd, LVM_DELALLITEM, 0, 0);

	struct   dirent* pDirEnt;
	DIR*     dir;
	struct   stat ftype;
	char fullpath [PATH_MAX + 1];
	char     filefullname[PATH_MAX+NAME_MAX+1];
	 FILEINFO fileinfo;


	dir = opendir (path);
	while ((pDirEnt = readdir ( dir )) != NULL ) 
	{
	memset (&fileinfo, 0, sizeof (fileinfo));
	strncpy (fullpath, path, PATH_MAX);
        strcat (fullpath, "/");
        strcat (fullpath, pDirEnt->d_name);
	if ( strcmp (pDirEnt->d_name, ".") == 0 || strcmp (pDirEnt->d_name, "..") == 0)
            continue;
	if (pDirEnt->d_name [0] == '.') 
                continue;
	if (stat (fullpath, &ftype) < 0 )
           continue;

	if (S_ISDIR (ftype.st_mode)){
            fileinfo.IsDir = TRUE;
            fileinfo.filesize = ftype.st_size;
        }
        else if (S_ISREG (ftype.st_mode)) {
                       
            fileinfo.IsDir = FALSE;
            fileinfo.filesize = ftype.st_size;
        }
        
      strcpy (fileinfo.filename, pDirEnt->d_name);
	fileinfo.modifytime = ftype.st_mtime;
	
       instolistview (hwnd,&fileinfo);
	}

	closedir (dir);

    SendMessage (hwnd, MSG_FREEZECTRL, FALSE, 0);
}

static void instolistview (HWND hwnd,PFILEINFO pfi)
{
   int       nItemCount;
    LVSUBITEM subdata;
    LVITEM    item;
    char      chTemp[255];
    struct tm *ptm;

	icon_ft_dir = LoadSystemIcon (SYSICON_FT_DIR, 0);
	icon_ft_file = LoadSystemIcon (SYSICON_FT_FILE, 0);

   nItemCount = SendMessage (hwnd, LVM_GETITEMCOUNT, 0, 0);
    
    item.nItem = nItemCount;
    item.itemData = (DWORD)pfi->IsDir;
    item.nItemHeight = 30;
    SendMessage (hwnd, LVM_ADDITEM, 0, (LPARAM)&item);

    subdata.nItem = nItemCount;
    subdata.nTextColor = 0;
    subdata.flags =LVFLAG_ICON;

	if (pfi->IsDir)
        subdata.image = (DWORD)icon_ft_dir;
	else
        subdata.image = (DWORD)icon_ft_file;
    
    subdata.subItem = 0;
    subdata.pszText = (char *)malloc (NAME_MAX+1);
    if ( subdata.pszText == NULL) 
        return ;

    strcpy (subdata.pszText, pfi->filename);
    SendMessage (hwnd, LVM_SETSUBITEM, 0, (LPARAM)&subdata);

    subdata.flags = 0;
    subdata.image = 0;
    subdata.subItem = 1;
    sprintf(chTemp, " %d", pfi->filesize);
    strcpy(subdata.pszText, chTemp);
    SendMessage (hwnd, LVM_SETSUBITEM, 0, (LPARAM)&subdata);

    subdata.subItem = 2;
    ptm = (struct tm *)localtime (&(pfi->modifytime));
    sprintf (subdata.pszText, "%d-%.2d-%.2d",  ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
    sprintf (subdata.pszText + 10, " %.2d:%.2d:%.2d",  ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    SendMessage (hwnd, LVM_SETSUBITEM, 0, (LPARAM)&subdata);

    if (subdata.pszText != NULL) free (subdata.pszText);
}
//******************************************************************
void delet_notif_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
	if (nc == BN_CLICKED)
	{
		int  nSelItem;
		int  nIsDir;
		LVSUBITEM subItem;
		int choose=0;
		PNEWFILEDLGDATA ppfile;
		
		switch(id) {
		case IDC_BTNDELET:
			nSelItem = SendMessage (hEProIn, LVM_GETSELECTEDITEM, 0, 0);
	                if (nSelItem > 0 ) 
			{
				choose = MessageBox (hwnd,
					"do you want delet file ?",
					"Delete File",
					MB_YESNOCANCEL | MB_ICONQUESTION);
				
				if(choose==IDYES)
				{
					nIsDir = SendMessage (hEProIn, LVM_GETITEMADDDATA, 0, nSelItem);    
                	    		memset (&subItem, 0, sizeof (subItem));
					subItem.subItem = 0;
					subItem.pszText = (char *)calloc (NAME_MAX + 1, 1);
					if (subItem.pszText == NULL) 
					break;
					SendMessage (hEProIn, LVM_GETSUBITEMTEXT,  (WPARAM)nSelItem, (LPARAM)&subItem);
					remove(subItem.pszText);
					if (subItem.pszText !=NULL ) free (subItem.pszText); 

					getcwd (ppfile->filepath,PATH_MAX);
					GetFileAndDirList(hEProIn,ppfile->filepath);
					ShowWindow(hEProIn,SW_SHOW);
					ShowWindow(GetDlgItem(hEEdit,IDC_MLEDIT),SW_HIDE);
				}
				else if(choose==IDCANCEL) break;
			}
			else break;
		break;}
	}
}

void up_notif_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
	HWND hCtrlWnd;
	char chPath[PATH_MAX+1];
		
	hCtrlWnd = GetDlgItem (hEEdit, IDC_EPATH);
	memset (chPath, 0, sizeof(chPath));
        GetWindowText (hCtrlWnd, chPath, MY_PATHMAX);

	if (nc == BN_CLICKED){
		switch(id) {
		case IDC_EUP:
			GetParentDir (chPath);
	                SetWindowText (hCtrlWnd, chPath); 
                	GetFileAndDirList (hEProIn, chPath);
			ShowWindow(hEProIn,SW_SHOW);
			SetWindowText(GetDlgItem(hEEdit,IDC_EFNAME),"");
			if(chPath!=NULL)free(chPath);
		break;
			}
		}
}
void path_notif_proc(HWND hwnd, int id, int nc, DWORD add_data)
{
	char filename[NAME_MAX+1];
	int index;

	if(nc==CBN_SELCHANGE)
	{
		index=SendMessage(hwnd,CB_GETCURSEL,0,0);
		if(index>=0){
		SendDlgItemMessage(hEEdit,IDC_EPATH,CB_GETLBTEXT,index,(LPARAM)filename);
		GetFileAndDirList (hEProIn, filename);
		ShowWindow(hEProIn,SW_SHOW);
		ShowWindow(GetDlgItem(hEEdit,IDC_MLEDIT),SW_HIDE);
		SetWindowText(GetDlgItem(hEEdit,IDC_EFNAME),"");
		}
	}
}
void listview_notif_proc(HWND hwnd, int id, int nc, DWORD add_data)
{
	HWND hCtrlWnd;
	char chPath[PATH_MAX+1];
	int nSelItem,nIsDir;
	LVSUBITEM  subItem;


	hCtrlWnd = GetDlgItem (hEEdit, IDC_EPATH);
        memset (chPath, 0, sizeof(chPath));
        GetWindowText (hCtrlWnd, chPath,PATH_MAX);

	if(nc==LVN_ITEMDBCLK){
		switch(id){
		case IDC_LISTVIEW:
			hCtrlWnd=GetDlgItem (hEEdit, IDC_LISTVIEW);
			nSelItem = SendMessage (hCtrlWnd, LVM_GETSELECTEDITEM, 0, 0);
                	if (nSelItem > 0 )
			{
                		nIsDir = SendMessage (hCtrlWnd, LVM_GETITEMADDDATA, 0, nSelItem);
				memset (&subItem, 0, sizeof (subItem));
				subItem.subItem = 0;
				subItem.pszText = (char *)malloc (NAME_MAX+1);
				if (subItem.pszText == NULL) 
				break; 
				SendMessage (hCtrlWnd, LVM_GETSUBITEMTEXT, (WPARAM)nSelItem, (LPARAM)&subItem);
				if (nIsDir == 1 )
				{
                        	sprintf (chPath, "%s/%s", strcmp (chPath, "/") == 0 ? "" : chPath, subItem.pszText);
                        	hCtrlWnd = GetDlgItem (hEEdit, IDC_EPATH);
                        	GetFileAndDirList (hEProIn, chPath);
	//printf("nihao!\n");
				SetWindowText (hCtrlWnd, chPath);
                            if (CB_ERR == SendMessage (hCtrlWnd, CB_FINDSTRINGEXACT, 0,(LPARAM)chPath))
                            	SendMessage (hCtrlWnd, CB_INSERTSTRING, 0,(LPARAM)chPath);
//	printf("ok?\n");
                        	}

			if (subItem.pszText != NULL ) free (subItem.pszText);
			}
		}
	}

}

void draw_notif_proc(HWND hwnd, int id, int nc, DWORD add_data)
{
	if (nc == BN_CLICKED)
	{
	int  nSelItem;
	int nIsDir;
	LVSUBITEM subItem;
	float WC ,LC ;
		
	char ptext[204800];		//����������Ϊ200K
	int num;			// ��������(�س�����)
	memset(ptext, '0', sizeof(ptext));
	
	nSelItem = SendMessage (hEProIn, LVM_GETSELECTEDITEM, 0, 0);
	if (nSelItem > 0 ) {
		HWND h1=GetDlgItem (hEEdit, IDC_GRAPH);
		InvalidateRect(h1, NULL, TRUE);

		nIsDir = SendMessage (hEProIn, LVM_GETITEMADDDATA, 0, nSelItem);    
               	memset (&subItem, 0, sizeof (subItem));
		subItem.subItem = 0;
		subItem.pszText = (char *)calloc (NAME_MAX + 1, 1);
		if (subItem.pszText == NULL) 
		return;
		SendMessage (hEProIn, LVM_GETSUBITEMTEXT,  (WPARAM)nSelItem, (LPARAM)&subItem);
		
		if(!OpenFile(ptext,subItem.pszText,hEProIn)) return;		//����������ݵ�ptext��
		num=stringnum(ptext, "\n");	//�����лس����ĸ���(����)
		if(num<=0) return;
		int sign,Sign1,Sign2,Sign3,Sign4;                        //���߲���״̬��־
		double X0,Z0,X1,Z1,R=0.05,d0,d1,r0x,r0x_abs,r0z,r0z_abs,r1x,r1x_abs,r1z,r1z_abs;
		double S1z,S1x,S2z,S2x,S3z,S3x,S4z,S4x,K1,K2,Sz,Sx,Sz1,Sx1,Sz2,Sx2,Start_Z=0.,Start_X=0.;
		double I_one,K_one,I_two,K_two,KBx0,KBz0,KBx1,KBz1,Ox,Oz,K_arc,a_arc,b_arc,c_arc;
		double Kline,r_arc;
		//printf("%d\n",num);
		/*if(ptext[0]!="G")
		{
			MessageBox (hwnd,"�����ļ�","Program Input", MB_OK | MB_ICONEXCLAMATION);
			return;
		}*/
		nc_data data[num],cs[40],as[40];		//��������������������
		//m_point point[num];		//ÿ�в���һ��"����"����� 
		
	int i,j,c=0, a=0;;			//iΪdata�±�, i+1Ϊ����
	char *phead;		//����ÿһ���ַ�����ͷָ��
	char *delim ="\n";		//�ûس����ָ����
	for(i=0; phead=(0==i ? strtok(ptext, delim) : strtok(NULL, delim)); i++)
	{
		//�к�
		data[i].row_id=i+1;
		//·���岹ģʽ
		if(strstr(phead,"G00")) data[i].G_pathmode=0;
			else if(strstr(phead,"G01")) data[i].G_pathmode=1; 
			else if(strstr(phead,"G02")) data[i].G_pathmode=2; 
			else if(strstr(phead,"G03")) data[i].G_pathmode=3;
			else data[i].G_pathmode = (0==i ? 1 : data[i-1].G_pathmode);
		//����ģʽ
		if(strstr(phead,"G90")) data[i].G_coormode=90; 
			else if(strstr(phead,"G91")) data[i].G_coormode=91;
			else data[i].G_coormode = (0==i ? 91 : data[i-1].G_coormode);
		//����ƽ��
		if(strstr(phead,"G17")) data[i].G_plane=17;
		if(strstr(phead,"G18")) data[i].G_plane=18; 
		if(strstr(phead,"G19")) data[i].G_plane=19;
		//���߲���
		if(strstr(phead,"G40")) data[i].G_compensate=40;
			else if(strstr(phead,"G41")) {data[i].G_compensate=41; sign=1;}
			else if(strstr(phead,"G42")) {data[i].G_compensate=42; sign=1;}
			else data[i].G_compensate = (0==i ? 40 : data[i-1].G_compensate);
		//�Ƿ����趨����ԭ��
		if(strstr(phead,"G92")) data[i].G_origin=1;
		//����ֵ
		if(strstr(phead,"X")) data[i].X=Gcode2d(phead,"X");
			else data[i].X = (0==i ? 0 : data[i-1].X);
		if(strstr(phead,"Y")) data[i].Y=Gcode2d(phead,"Y");
			else data[i].Y = (0==i ? 0 : data[i-1].Y);
		if(strstr(phead,"Z")) data[i].Z=Gcode2d(phead,"Z");
			else data[i].Z = (0==i ? 0 : data[i-1].Z);
		if(strstr(phead,"B")) data[i].B=Gcode2d(phead,"B");
			else data[i].B = (0==i ? 0 : data[i-1].B);
		if(strstr(phead,"C")) data[i].C=Gcode2d(phead,"C");
			else data[i].C = (0==i ? 0 : data[i-1].C);
		if(strstr(phead,"I")) data[i].I=Gcode2d(phead,"I");
			else data[i].I = 0;
		if(strstr(phead,"J")) data[i].J=Gcode2d(phead,"J");
			else data[i].J = 0;
		if(strstr(phead,"K")) data[i].K=Gcode2d(phead,"K");
			else data[i].K = 0;
		if(strstr(phead,"R")) data[i].R=Gcode2d(phead,"R");
			else data[i].R = 0;
		//M����
		if(strstr(phead,"M")) data[i].M=Gcode2i(phead,"M");
			else data[i].M=0;
		//S����
		if(strstr(phead,"S")) data[i].S=Gcode2i(phead,"S");
		//D����
		if(strstr(phead,"D")) data[i].D=Gcode2i(phead,"D");

		printf("cde daxiao wei %d\n",c);

		if(sign == 1)//���е���
		{
			if(c != 0)//����������		
			{
          printf("��������\n");
				if(data[i].G_pathmode == 1 && cs[c-1].G_pathmode ==1)//ֱ�߽�ֱ��
				{

					if(cs[c-1].G_plane == 18)
					{
						Z0 = cs[c-1].Z;
						X0 = cs[c-1].X;
						Z1 = data[i].Z;
						X1 = data[i].X;
					}
					else if(cs[c-1].G_plane == 17)
					{
						Z0 = cs[c-1].X;
						X0 = cs[c-1].Y;
						Z1 = data[i].X;
						X1 = data[i].Y;
					}
					else if(cs[c-1].G_plane == 19)
					{
						Z0 = cs[c-1].Y;
						X0 = cs[c-1].Z;
						Z1 = data[i].Y;
						X1 = data[i].Z;
					}
					if(cs[c-1].G_coormode == 91)		//G90�������Ҫ����......................;
					{
						d0 = sqrt(Z0*Z0 +X0*X0);	
						d1 = sqrt(Z1*Z1 +X1*X1);	
						
						if(cs[c-1].G_compensate == 41)
						{
							r0x = R*Z0/d0;
							r0z = (-R)*X0/d0;

							r1x = R*Z1/d1;
							r1z = (-R)*X1/d1;							
						}
						else if(cs[c-1].G_compensate == 42)
						{
							r0x = (-R)*Z0/d0;
							r0z = R*X0/d0;

							r1x = (-R)*Z1/d1;
							r1z = R*X1/d1;
						}
					}
          //printf("ok\n");
				//�жϽ�������:����,����,�쳤
						if( ((r0x*r1x + r0z*r1z) >= 0) && (((r1x-r0x)*X0 + (r1z-r0z)*Z0) >= 0) )//�쳤��||������
						{					//�쳤�ͺ������ͳ���һ��,���Ժϲ�Ϊһ�γ���***			
          printf("�쳤�Ե���\n");
							if(Z0 == 0)
							{
							S1z= r0z;	S1x=r0x; //ǰһ��G��������Ӧ�ĵ�����������,���ڱ�ʾ��һ��ֱ��;
							S2z= Z0+Z1+r1z;	S2x=X0+X1+r1x;K2=X1/Z1;//��һG���յ��Ӧ�ĵ�����������,ȷ���ڶ�����;
							
							Sz = S1z;
							Sx = K2*(Sz - S2z) + S2x;//�쳤�ߵĽ�������;					
							}
							else if(Z1 == 0)
							{
							S1z= r0z;	S1x=r0x;K1=X0/Z0; 
							S2z= Z0+Z1+r1z;	S2x=X0+X1+r1x;
							
							Sz = S2z;
							Sx = K1*(Sz - S1z) + S1x;//�쳤�ߵĽ�������;					
							}
							else if((X1*Z0) == (X0*Z1))
							{
							S1z= r0z;    S1x=r0x; 
                                                        S2z= Z0+Z1+r1z;    S2x=X0+X1+r1x;
							Sz = Z0+r0z;
							Sx = X0+r0x;
							}
							else
							{
							S1z= r0z;	S1x=r0x; K1=X0/Z0;//��һ���߶���㴦�İ뾶ʸ����������;
							S2z= Z0+Z1+r1z;	S2x=X0+X1+r1x; K2=X1/Z1;//�ڶ������յ㴦�İ뾶ʸ����������;
						
							Sz = (S2x - S1x - K2*S2z + K1*S1z)/(K1 - K2);
							Sx = K1*(Sz - S1z) + S1x;
							}//�쳤�ߵĽ�������;
							

							as[a]= cs[c-1];		//��ֵ,����
							if(cs[c-1].G_plane == 18)
							{
								as[a].Z = Sz - Start_Z;
								as[a].X = Sx - Start_X;
							}
							else if(cs[c-1].G_plane == 17)
							{
								as[a].X = Sz - Start_Z;
								as[a].Y = Sx - Start_X;
							}
							else if(cs[c-1].G_plane == 19)
							{
								as[a].Y = Sz - Start_Z;
								as[a].Z = Sx - Start_X;
							}
					printf("�쳤 as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
							a++;

							Start_Z = Sz - Z0;	
							Start_X = Sx - X0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
						}//�쳤
				
						else if( ((r1x-r0x)*X0 + (r1z-r0z)*Z0) < 0 )//������
						{
          printf("�����Ե���\n");
							if(Z0 == 0)
							{
							S1z= r0z;	S1x=r0x; 
							S2z= Z0+Z1+r1z;	S2x=X0+X1+r1x;K2=X1/Z1;
							
							Sz = S1z;
							Sx = K2*(Sz - S2z) + S2x;//�쳤�ߵĽ�������;					
							}
							else if(Z1 == 0)
							{
							S1z= r0z;	S1x=r0x;K1=X0/Z0; 
							S2z= Z0+Z1+r1z;	S2x=X0+X1+r1x;
							
							Sz = S2z;
							Sx = K1*(Sz - S1z) + S1x;//�쳤�ߵĽ�������;					
							}
							else
							{
							S1z= r0z;	S1x=r0x; K1=X0/Z0;//��һ���߶���㴦�İ뾶ʸ����������;
							S2z= Z0+Z1+r1z;	S2x=X0+X1+r1x; K2=X1/Z1;//�ڶ������յ㴦�İ뾶ʸ����������;
						
							Sz = (S2x - S1x - K2*S2z + K1*S1z)/(K1 - K2);
							Sx = K1*(Sz - S1z) + S1x;
							}//�쳤�ߵĽ�������;
							

							as[a]= cs[c-1];		//��ֵ,����
							if(cs[c-1].G_plane == 18)
							{
								as[a].Z = Sz - Start_Z;
								as[a].X = Sx - Start_X;
							}
							else if(cs[c-1].G_plane == 17)
							{
								as[a].X = Sz - Start_Z;
								as[a].Y = Sx - Start_X;
							}
							else if(cs[c-1].G_plane == 19)
							{
								as[a].Y = Sz - Start_Z;
								as[a].Z = Sx - Start_X;
							}
					printf("����  as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
							a++;

							Start_Z = Sz - Z0;	
							Start_X = Sx - X0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
							
						}//������

					else if( (((r1z-r0z)*Z0 +(r1x-r0x)*X0) >= 0) && ((r0x*r1x + r0z*r1z) < 0) )//������
						{
          printf("�����Ե���\n");
							S1z= Z0 + r0z;	S1x=X0 + r0x; //��һ���߶��յ㴦�İ뾶ʸ����������;
							S2z= Z0 + r1z;	S2x=X0 + r1x;//�ڶ�������㴦�İ뾶ʸ����������;
										//S1,S2���������;
							if(X0 > 0) Sign1 = 1;
							else if(X0 == 0) Sign1 = 0;
							else Sign1 = -1;
								
							if(Z0 > 0) Sign2 = 1;
							else if(Z0 == 0) Sign2 = 0;
							else Sign2 = -1;

							if(-X1 > 0) Sign3 = 1;
							else if(-X1 == 0) Sign3 = 0;
							else Sign3 = -1;

							if(-Z1 > 0) Sign4 = 1;
							else if(-Z1 == 0) Sign4 = 0;
							else Sign4 = -1;
							
							if(r0x <0) r0x_abs = -r0x;
							else r0x_abs = r0x;

							if(r0z <0) r0z_abs = -r0z;
							else r0z_abs = r0z;

							if(r1x <0) r1x_abs = -r1x;
							else r1x_abs = r1x;

							if(r1z <0) r1z_abs = -r1z;
							else r1z_abs = r1z;
							
	printf("Sig1=%d,%d,%d,%d,r0x=%f,r0z=%f,r1x=%f,r1z=%f\n",Sign1,Sign2,Sign3,Sign4,r0x_abs,r0z_abs,r1x_abs,r1z_abs);

							S3x = S1x + Sign1*r0z_abs;
							S3z = S1z + Sign2*r0x_abs; 	

							S4x = S2x + Sign3*r1z_abs;
							S4z = S2z + Sign4*r1x_abs; 	
	printf("Start_Z=%f,Start_X=%f\n",Start_Z,Start_X);
	printf("s3x=%f,s3z=%f,s4x=%f,s4z=%f\n",S3x,S3z,S4x,S4z);

							as[a]= cs[c-1];		//��ֵ,����
							if(cs[c-1].G_plane == 18)
							{
								as[a].Z = S3z - Start_Z;
								as[a].X = S3x - Start_X;
							}
							else if(cs[c-1].G_plane == 17)
							{
								as[a].X = S3z - Start_Z;
								as[a].Y = S3x - Start_X;
							}
							else if(cs[c-1].G_plane == 19)
							{
								as[a].Y = S3z - Start_Z;
								as[a].Z = S3x - Start_X;
							}
					printf("�����һ as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
							a++;
							
							as[a]= cs[c-1];		//���������
							if(cs[c-1].G_plane == 18)
							{
								as[a].Z = S4z - S3z;
								as[a].X = S4x - S3x;
								as[a].row_id=a+1;
							}
							else if(cs[c-1].G_plane == 17)
							{
								as[a].X = S4z - S3z;
								as[a].Y = S4x - S3x;
								as[a].row_id=a+1;
							}
							else if(cs[c-1].G_plane == 19)
							{
								as[a].Y = S4z - S3z;
								as[a].Z = S4x - S3x;
								as[a].row_id=a+1;
							}
					printf("����ڶ� as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
							a++;

							Start_Z = S4z - Z0;	
							Start_X = S4x - X0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
						}//����
							
						if(data[i].G_compensate == 40)       //ȡ������;
							{
          printf("quxiao����2\n");
								as[a]= data[i];		//��ֵ,����
								if(cs[c-1].G_plane == 18)
								{
									as[a].Z = Z1 - Start_Z;
									as[a].X = X1 - Start_X;
								}
								else if(cs[c-1].G_plane == 17)
								{
									as[a].X = Z1 - Start_Z;
									as[a].Y = X1 - Start_X;
								}
								else if(cs[c-1].G_plane == 19)
								{
									as[a].Y = Z1 - Start_Z;
									as[a].Z = X1 - Start_X;
								}
					printf("����ȡ�� as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
								a++;
								sign = 0;//��������
								c = 0;//Ϊ��һ�ε�������ʼ��.
								Start_Z = 0.;	
								Start_X = 0.;
							}//ȡ������;			
				}//ֱ�߽�ֱ��;


				else if(data[i].G_pathmode == 1 && (cs[c-1].G_pathmode ==3 || cs[c-1].G_pathmode ==2))//Բ����ֱ��
				{
//********************************************************************************
					if(cs[c-1].G_plane == 18)
					{
						Z0 = cs[c-1].Z;
						X0 = cs[c-1].X;
						K_one=cs[c-1].K;
						I_one=cs[c-1].I;

						Z1 = data[i].Z;
						X1 = data[i].X;
					}
					else if(cs[c-1].G_plane == 17)
					{
						Z0 = cs[c-1].X;
						X0 = cs[c-1].Y;
						K_one=cs[c-1].I;
						I_one=cs[c-1].J;

						Z1 = data[i].X;
						X1 = data[i].Y;
					}
					else if(cs[c-1].G_plane == 19)
					{
						Z0 = cs[c-1].Y;
						X0 = cs[c-1].Z;
						K_one=cs[c-1].J;
						I_one=cs[c-1].K;

						Z1 = data[i].Y;
						X1 = data[i].Z;
					}
					if(cs[c-1].G_coormode == 91)		//G90�������Ҫ����......................;
					{
						d0 = sqrt((X0-I_one)*(X0-I_one)+(Z0-K_one)*(Z0-K_one));	
						d1 = sqrt(Z1*Z1 +X1*X1);	
						
		if((cs[c-1].G_compensate == 41 && cs[c-1].G_pathmode == 2)||(cs[c-1].G_compensate == 42 && cs[c-1].G_pathmode == 3))
						{
							r0x = R*(X0-I_one)/d0;
							r0z = R*(Z0-K_one)/d0;
						}
		else if((cs[c-1].G_compensate == 42 && cs[c-1].G_pathmode == 2)||(cs[c-1].G_compensate == 41 && cs[c-1].G_pathmode == 3))
						{
							r0x = (-R)*(X0-I_one)/d0;
							r0z = (-R)*(Z0-K_one)/d0;
						}

						if(data[i].G_compensate == 41)
						{
							r1x = R*Z1/d1;
							r1z = (-R)*X1/d1;								
						}		
						else if(data[i].G_compensate == 42)
						{
							r1x = (-R)*Z1/d1;
							r1z = R*X1/d1;
						}//ȷ��r0x,r0z,r1x,r1z����;

						if(cs[c-1].G_pathmode == 2)
						{
							KBx0= -(Z0-K_one);
							KBz0=X0-I_one;
						}
						else if(cs[c-1].G_pathmode == 3)
						{
							KBx0=Z0-K_one;
							KBz0= -(X0-I_one);
						}// ȷ��KB����;
					}//G91
				//�жϽ�������:����,����,�쳤
						if( ((r0x*r1x + r0z*r1z) >= 0) && (((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0) >= 0) )//�쳤��	
     						{
	printf("�쳤�Ե���\n");
							if(KBz0 == 0 && Z1 != 0)
							{
							S1z= Z0+r0z;	S1x=X0+r0x; 
							S2z= Z0+r1z;	S2x=X0+r1x;K2=X1/Z1;
							
							Sz = S1z;
							Sx = K2*(Sz - S2z) + S2x;//ֱ��б�����޴�ʱ;					
							}
							else if(KBz0 != 0 && Z1 == 0)
							{
							S1z= Z0+r0z;	S1x=X0+r0x;K1=KBx0/KBz0; 
							S2z= Z0+r1z;	S2x=X0+r1x;
							
							Sz = S2z;
							Sx = K1*(Sz - S1z) + S1x;//ֱ��б�����޴�ʱ;					
							}
							else if((KBx0*Z1) == (X1*KBz0))
							{
							
							Sz = Z0+r0z;
							Sx = X0+r0x;			
							}//ֱ��б�����ʱ;
							else
							{
							S1z= Z0+r0z;	S1x=X0+r0x;K1=KBx0/KBz0; 
							S2z= Z0+r1z;	S2x=X0+r1x;K2=X1/Z1;
							
							Sz = (S2x - S1x - K2*S2z + K1*S1z)/(K1 - K2);
							Sx = K1*(Sz - S1z) + S1x;
							}//�쳤�ߵĽ�������;					
						

							as[a]= cs[c-1];		//��ֵ,����
							if(cs[c-1].G_plane == 18)
							{
								as[a].Z = (Z0+r0z)-Start_Z;
								as[a].X = (X0+r0x)-Start_X;
								as[a].K = K_one-Start_Z;
								as[a].I = I_one-Start_X;
							}
							else if(cs[c-1].G_plane == 17)
							{
								as[a].X = (Z0+r0z)-Start_Z;
								as[a].Y = (X0+r0x)-Start_X;
								as[a].I = K_one-Start_Z;
                                                                as[a].J = I_one-Start_X;
							}
							else if(cs[c-1].G_plane == 19)
							{
								as[a].Y = (Z0+r0z)-Start_Z;
								as[a].Z = (X0+r0x)-Start_X;
								as[a].J = K_one-Start_Z;
                                                                as[a].K = I_one-Start_X;
							}
							a++;//��һ��Բ��;

							as[a]= data[i];		//��ֵ,����
							if(cs[c-1].G_plane == 18)
							{
								as[a].Z = Sz-(Z0+r0z);
								as[a].X = Sx-(X0+r0x);
							}
							else if(cs[c-1].G_plane == 17)
							{
								as[a].X = Sz-(Z0+r0z);
								as[a].Y = Sx-(X0+r0x);
							}
							else if(cs[c-1].G_plane == 19)
							{
								as[a].Y = Sz-(Z0+r0z);
								as[a].Z = Sx-(X0+r0x);
							}
							a++;//Բ�����ߵ��쳤����;

							Start_Z = Sz - Z0;	
							Start_X = Sx - X0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
							
						}//�쳤��;

						else if( (((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0) < 0)||(((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)==0&&cs[c-1].G_compensate == 41 && cs[c-1].G_pathmode == 2)||(((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)==0&&cs[c-1].G_compensate == 42 && cs[c-1].G_pathmode== 3)) //������	
						{
          printf("�����Ե���\n");
							S1z=Z0+r1z;	S1x=X0+r1x;
							S2z=Z0+Z1+r1z;	S2x=X0+X1+r1x;//��Բ���ཻ��ֱ�ߵ�������;

							Oz=K_one;	Ox=I_one;//Բ������;
							r_arc=sqrt( (Z0+r0z-K_one)*(Z0+r0z-K_one) + (X0+r0x-I_one)*(X0+r0x-I_one) );//Բ���뾶

							if(S1z == S2z)
							{
							Sz1=S1z ;	Sx1=Ox + sqrt(r_arc*r_arc-(S1z-Oz)*(S1z-Oz));
							Sz2=S1z ;	Sx2=Ox - sqrt(r_arc*r_arc-(S1z-Oz)*(S1z-Oz));
							}
							else if(S1x == S2x)
							{
							Sz1=Oz + sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));	Sx1=S1x;
							Sz2=Oz - sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));	Sx2=S1x;
							}
							else
							{
							K_arc=(S2z-S1z)/(S2x-S1x);
							a_arc=1+K_arc*K_arc;
							b_arc=2*K_arc*(S2z-Oz-K_arc*S2x-Ox/K_arc);
							c_arc=(S2z-Oz-K_arc*S2x)*(S2x-Oz-K_arc*S2x)-r_arc*r_arc+Ox*Ox;
							
						Sx1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sz1=S2z + K_arc*(Sx1-S2x);
						Sx2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sz2=S2z + K_arc*(Sx2-S2x);
							}//�쳤�ߵĽ�������;
							
	//if( ((S1z*Z1+S1x*X1)>=0 && ((Sz2-Sz1)*Z1+(Sx2-Sx1)*X1)<=0) || ((S1z*Z1+S1x*X1)<0 && ((Sz2-Sz1)*Z1+(Sx2-Sx1)*X1)>0) )
	if( ((r0z*Z1+r0x*X1)>=0 && ((Sz2-Sz1)*Z1+(Sx2-Sx1)*X1)<=0) || ((r0z*Z1+r0x*X1)<0 && ((Sz2-Sz1)*Z1+(Sx2-Sx1)*X1)>0) )
							{
							Sx=Sx1;
							Sz=Sz1;
							}
	//else if( ((S1z*Z1+S1x*X1)>=0 && ((Sz2-Sz1)*Z1+(Sx2-Sx1)*X1)>0) || ((S1z*Z1+S1x*X1)<0 && ((Sz2-Sz1)*Z1+(Sx2-Sx1)*X1)<=0) )
	else if( ((r0z*Z1+r0x*X1)>=0 && ((Sz2-Sz1)*Z1+(Sx2-Sx1)*X1)>0) || ((r0z*Z1+r0x*X1)<0 && ((Sz2-Sz1)*Z1+(Sx2-Sx1)*X1)<=0) )
							{
							Sx=Sx2;
							Sz=Sz2;
							}
							as[a]= cs[c-1];		//��ֵ,����
							if(cs[c-1].G_plane == 18)
							{
								as[a].Z = Sz - Start_Z;
								as[a].X = Sx - Start_X;
								as[a].K = K_one-Start_Z;
								as[a].I = I_one-Start_X;
							}
							else if(cs[c-1].G_plane == 17)
							{
								as[a].X = Sz - Start_Z;
								as[a].Y = Sx - Start_X;
								as[a].I = K_one-Start_Z;
                                                                as[a].J = I_one-Start_X;
							}
							else if(cs[c-1].G_plane == 19)
							{
								as[a].Y = Sz - Start_Z;
								as[a].Z = Sx - Start_X;
								as[a].J = K_one-Start_Z;
                                                                as[a].K = I_one-Start_X;
							}
							a++;

							Start_Z = Sz - Z0;	
							Start_X = Sx - X0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
							
						}//������
				

					else if( ((((r1z-r0z)*KBz0 +(r1x-r0x)*KBx0) >0)&&((r0x*r1x + r0z*r1z) < 0))||(((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)==0&&cs[c-1].G_compensate == 42 && cs[c-1].G_pathmode ==2)||(((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)==0&&cs[c-1].G_compensate == 41 && cs[c-1].G_pathmode ==3) )//������
						{printf("�����Ե���\n");		
							S1z= Z0 + r0z;	S1x=X0 + r0x; //��һ���߶��յ㴦�İ뾶ʸ����������;
							S2z= Z0 + r1z;	S2x=X0 + r1x;//�ڶ�������㴦�İ뾶ʸ����������;
										//S1,S2���������;

							if(cs[c-1].G_pathmode ==3)
							{
								if((Z0-K_one) > 0) Sign1 = 1;
								else if((Z0-K_one) == 0) Sign1 = 0;
								else Sign1 = -1;

								if((I_one-X0) > 0) Sign2 = 1;
								else if((I_one-X0) == 0) Sign2 = 0;
								else Sign2 = -1;
							}
							else if(cs[c-1].G_pathmode ==2)
							{
								if((K_one-Z0) > 0) Sign1 = 1;
								else if((K_one-Z0) == 0) Sign1 = 0;
								else Sign1 = -1;

								if((X0-I_one) > 0) Sign2 = 1;
								else if((X0-I_one) == 0) Sign2 = 0;
								else Sign2 = -1;
							}
							if((-X1) > 0) Sign3 = 1;
							else if((-X1) == 0) Sign3 = 0;
							else Sign3 = -1;

							if(-Z1 > 0) Sign4 = 1;
							else if(-Z1 == 0) Sign4 = 0;
							else Sign4 = -1;
							
							if(r0x <0) r0x_abs = -r0x;
							else r0x_abs = r0x;

							if(r0z <0) r0z_abs = -r0z;
							else r0z_abs = r0z;

							if(r1x <0) r1x_abs = -r1x;
							else r1x_abs = r1x;

							if(r1z <0) r1z_abs = -r1z;
							else r1z_abs = r1z;
							
	printf("Sig1=%d,%d,%d,%d,r0x=%f,r0z=%f,r1x=%f,r1z=%f\n",Sign1,Sign2,Sign3,Sign4,r0x_abs,r0z_abs,r1x_abs,r1z_abs);

							S3x = S1x + Sign1*r0z_abs;
							S3z = S1z + Sign2*r0x_abs; 	

							S4x = S2x + Sign3*r1z_abs;
							S4z = S2z + Sign4*r1x_abs; 	
	printf("Start_Z=%f,Start_X=%f\n",Start_Z,Start_X);
	printf("s3x=%f,s3z=%f,s4x=%f,s4z=%f\n",S3x,S3z,S4x,S4z);

							as[a]= cs[c-1];		//��ֵ,����,Բ������
							if(cs[c-1].G_plane == 18)
							{
								as[a].Z = S1z - Start_Z;
								as[a].X = S1x - Start_X;
								as[a].K = K_one-Start_Z;
								as[a].I = I_one-Start_X;
							}
							else if(cs[c-1].G_plane == 17)
							{
								as[a].X = S1z - Start_Z;
								as[a].Y = S1x - Start_X;
								as[a].I = K_one-Start_Z;
                                                                as[a].J = I_one-Start_X;
							}
							else if(cs[c-1].G_plane == 19)
							{
								as[a].Y = S1z - Start_Z;
								as[a].Z = S1x - Start_X;
								as[a].J = K_one-Start_Z;
                                                                as[a].K = I_one-Start_X;
							}
							a++;

							as[a]= data[i];		//��ֵ,����,�쳤��ֱ��S1S3����;
							if(cs[c-1].G_plane == 18)
							{
								as[a].Z = S3z - S1z;
								as[a].X = S3x - S1x;
								as[a].row_id=a+1;
							}
							else if(cs[c-1].G_plane == 17)
							{
								as[a].X = S3z - S1z;
								as[a].Y = S3x - S1x;
								as[a].row_id=a+1;
							}
							else if(cs[c-1].G_plane == 19)
							{
								as[a].Y = S3z - S1z;
								as[a].Z = S3x - S1x;
								as[a].row_id=a+1;
							}
							a++;
							
							as[a]= data[i];		//�������S3S4����;
							if(cs[c-1].G_plane == 18)
							{
								as[a].Z = S4z - S3z;
								as[a].X = S4x - S3x;
								as[a].row_id=a+1;
							}
							else if(cs[c-1].G_plane == 17)
							{
								as[a].X = S4z - S3z;
								as[a].Y = S4x - S3x;
								as[a].row_id=a+1;
							}
							else if(cs[c-1].G_plane == 19)
							{
								as[a].Y = S4z - S3z;
								as[a].Z = S4x - S3x;
								as[a].row_id=a+1;
							}
							a++;

							Start_Z = S4z - Z0;	
							Start_X = S4x - X0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
						}//����			
							if(data[i].G_compensate == 40)       //ȡ������;
							{
								as[a]= data[i];		//��ֵ,����
								if(cs[c-1].G_plane == 18)
								{
									as[a].Z = Z1 - Start_Z;
									as[a].X = X1 - Start_X;
								}
								else if(cs[c-1].G_plane == 17)
								{
									as[a].X = Z1 - Start_Z;
									as[a].Y = X1 - Start_X;
								}
								else if(cs[c-1].G_plane == 19)
								{
									as[a].Y = Z1 - Start_Z;
									as[a].Z = X1 - Start_X;
								}
					printf("����ȡ�� as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
								a++;
								sign = 0;//��������
								c = 0;//Ϊ��һ�ε�������ʼ��.
								Start_Z = 0.;	
								Start_X = 0.;
							}//ȡ������  */
				}//Բ����ֱ��

/*****************************************************************************************/
				else if((data[i].G_pathmode == 2 || data[i].G_pathmode == 3) && cs[c-1].G_pathmode ==1)//ֱ�߽�Բ��
				{
					if(cs[c-1].G_plane == 18)
					{
						Z0 = cs[c-1].Z;
						X0 = cs[c-1].X;

						Z1 = data[i].Z;
						X1 = data[i].X;
						K_two=data[i].K;
						I_two=data[i].I;
					}
					else if(cs[c-1].G_plane == 17)
					{
						Z0 = cs[c-1].X;
						X0 = cs[c-1].Y;

						Z1 = data[i].X;
						X1 = data[i].Y;
						K_two=data[i].I;
						I_two=data[i].J;
					}
					else if(cs[c-1].G_plane == 19)
					{
						Z0 = cs[c-1].Y;
						X0 = cs[c-1].Z;

						Z1 = data[i].Y;
						X1 = data[i].Z;
						K_two=data[i].J;
						I_two=data[i].K;
					}
					if(cs[c-1].G_coormode == 91)		//G90�������Ҫ����......................;
					{
						d0 = sqrt(Z0*Z0 +X0*X0);	
						d1 = sqrt((X1-I_two)*(X1-I_two)+(Z1-K_two)*(Z1-K_two));	
						
						if(cs[c-1].G_compensate == 41)
						{
							r0x = R*Z0/d0;
							r0z = (-R)*X0/d0;								
						}		
						else if(cs[c-1].G_compensate == 42)
						{
							r0x = (-R)*Z0/d0;
							r0z = R*X0/d0;
						}

				if(data[i].G_compensate == 41 && data[i].G_pathmode == 2)
						{
							r1x = R*(-I_two)/d1;
							r1z = R*(-K_two)/d1;
						}
				else if(data[i].G_compensate == 42 && data[i].G_pathmode == 3)
						{
							r1x = R*(-I_two)/d1;
                                                        r1z = R*(-K_two)/d1;
                                                }
				else if(data[i].G_compensate == 42 && data[i].G_pathmode == 2)
						{
							r1x = R*I_two/d1;
							r1z = R*K_two/d1;

						}
				else if(data[i].G_compensate == 41 && data[i].G_pathmode == 3)
						{
							r1x = R*I_two/d1;
                                                        r1z = R*K_two/d1;
						}
						//ȷ��r0x,r0z,r1x,r1z����;

						if(data[i].G_pathmode == 2)
						{
							KBx1= K_two;
							KBz1=-I_two;
						}
						else if(data[i].G_pathmode == 3)
						{
							KBx1=-K_two;
							KBz1= I_two;
						}// ȷ��KB����,Բ����㴦������;
					}//G91
				//�жϽ�������:����,����,�쳤
						if( ((r0x*r1x + r0z*r1z) >= 0) && (((r1x-r0x)*X0 + (r1z-r0z)*Z0) >= 0) )//�쳤��	
     						{
     printf("�쳤�Ե���\n");
							if(KBz1 == 0 && Z0 != 0)
							{
							S1z= Z0+r0z;	S1x=X0+r0x; K1=X0/Z0;
							S2z= Z0+r1z;	S2x=X0+r1x;
							
							Sz = S2z;
							Sx = K1*(Sz - S1z) + S1x;//ֱ��б�����޴�ʱ;	
							}
							else if(KBz1 != 0 && Z0 == 0)
							{
							S1z= Z0+r0z;	S1x=X0+r0x; 
							S2z= Z0+r1z;	S2x=X0+r1x;K2=KBx1/KBz1;
							
							Sz = S1z;
							Sx = K2*(Sz - S2z) + S2x;//ֱ��б�����޴�ʱ;					
							}
							else if((KBx1*Z0) == (X0*KBz1))
							{
							S1z= Z0+r0z;    S1x=X0+r0x; 
                                                        S2z= Z0+r1z;    S2x=X0+r1x;
							Sz = Z0+r0z;
							Sx = X0+r0x;			
							}//ֱ��б�����ʱ;
							else
							{
							S1z= Z0+r0z;	S1x=X0+r0x;K1=X0/Z0; 
							S2z= Z0+r1z;	S2x=X0+r1x;K2=KBx1/KBz1;
							
							Sz = (S2x - S1x - K2*S2z + K1*S1z)/(K1 - K2);
							Sx = K1*(Sz - S1z) + S1x;
							}//�쳤�ߵĽ�������;					
						
							as[a]= cs[c-1];		//��ֵ,������һ��ֱ��
							if(cs[c-1].G_plane == 18)
							{
								as[a].Z = Sz - Start_Z;
								as[a].X = Sx - Start_X;
							}
							else if(cs[c-1].G_plane == 17)
							{
								as[a].X = Sz - Start_Z;
								as[a].Y = Sx - Start_X;
							}
							else if(cs[c-1].G_plane == 19)
							{
								as[a].Y = Sz - Start_Z;
								as[a].Z = Sx - Start_X;
							}
							a++;


							as[a]= cs[c-1];		//��ֵ,����Բ�����ֵ��쳤����S-S2;
							if(cs[c-1].G_plane == 18)
							{
								as[a].Z = S2z - Sz;
								as[a].X = S2x - Sx;
							}
							else if(cs[c-1].G_plane == 17)
							{
								as[a].X = S2z - Sz;
								as[a].Y = S2x - Sx;
							}
							else if(cs[c-1].G_plane == 19)
							{
								as[a].Y = S2z - Sz;
								as[a].Z = S2x - Sx;
							printf("honghua S2x:%f,Sx:%f",S2x,Sx);
							}
							a++;//��һ��Բ��;

							Start_Z = S2z - Z0;	
							Start_X = S2x - X0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
						}//�쳤��;

						else if((((r1x-r0x)*X0 + (r1z-r0z)*Z0) < 0)||((((r1x-r0x)*X0 + (r1z-r0z)*Z0)== 0)&&data[i].G_compensate == 41 && data[i].G_pathmode == 2)||((((r1x-r0x)*X0 + (r1z-r0z)*Z0)== 0)&&data[i].G_compensate == 42 && data[i].G_pathmode == 3)) //������	
						{
							S1z=r0z;	S1x=r0x;
							S2z=Z0+r0z;	S2x=X0+r0x;//��Բ���ཻ��ֱ�ߵ�������;

							Oz=Z0+K_two;	Ox=X0+I_two;//Բ������;
							r_arc=sqrt( (Z0+r1z-Oz)*(Z0+r1z-Oz) + (X0+r1x-Ox)*(X0+r1x-Ox) );//Բ���뾶

							if(S1z == S2z)
							{
							Sz1=S1z ;	Sx1=Ox + sqrt(r_arc*r_arc-(S1z-Oz)*(S1z-Oz));
							Sz2=S1z ;	Sx2=Ox - sqrt(r_arc*r_arc-(S1z-Oz)*(S1z-Oz));
							}
							else if(S1x == S2x)
							{
							Sz1=Oz + sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));	Sx1=S1x;
							Sz2=Oz - sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));	Sx2=S1x;
							}
							else
							{
							K_arc=(S2z-S1z)/(S2x-S1x);
							a_arc=1+K_arc*K_arc;
							b_arc=2*K_arc*(S2z-Oz-K_arc*S2x-Ox/K_arc);
							c_arc=(S2z-Oz-K_arc*S2x)*(S2x-Oz-K_arc*S2x)-r_arc*r_arc+Ox*Ox;
							
						Sx1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sz1=S2z + K_arc*(Sx1-S2x);
						Sx2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sz2=S2z + K_arc*(Sx2-S2x);
							}//�쳤�ߵĽ�������;
							
	//if( ((S2z*Z0+S2x*X0)>=0 && ((Sz2-Sz1)*Z0+(Sx2-Sx1)*X0)<=0) || ((S2z*Z0+S2x*X0)<0 && ((Sz2-Sz1)*Z0+(Sx2-Sx1)*X0)>0) )
	if( ((r1z*Z0+r1x*X0)>=0 && ((Sz2-Sz1)*Z0+(Sx2-Sx1)*X0)<=0) || ((r1z*Z0+r1x*X0)<0 && ((Sz2-Sz1)*Z0+(Sx2-Sx1)*X0)>0) )
							{
							Sx=Sx1;
							Sz=Sz1;
							}
	//else if( ((S2z*Z0+S2x*X0)>=0 && ((Sz2-Sz1)*Z0+(Sx2-Sx1)*X0)>0) || ((S2z*Z0+S2x*X0)<0 && ((Sz2-Sz1)*Z0+(Sx2-Sx1)*X0)<=0) )
	else if( ((r1z*Z0+r1x*X0)>=0 && ((Sz2-Sz1)*Z0+(Sx2-Sx1)*X0)>0) || ((r1z*Z0+r1x*X0)<0 && ((Sz2-Sz1)*Z0+(Sx2-Sx1)*X0)<=0) )
							{
							Sx=Sx2;
							Sz=Sz2;
							}
							as[a]= cs[c-1];		//��ֵ,����
							if(cs[c-1].G_plane == 18)
							{
								as[a].Z = Sz - Start_Z;
								as[a].X = Sx - Start_X;
							}
							else if(cs[c-1].G_plane == 17)
							{
								as[a].X = Sz - Start_Z;
								as[a].Y = Sx - Start_X;
							}
							else if(cs[c-1].G_plane == 19)
							{
								as[a].Y = Sz - Start_Z;
								as[a].Z = Sx - Start_X;
							}
							a++;

							Start_Z = Sz - Z0;	
							Start_X = Sx - X0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
						}//������

					else if(((((r1z-r0z)*Z0 +(r1x-r0x)*X0) > 0) && ((r0x*r1x + r0z*r1z) < 0))||((((r1z-r0z)*Z0 +(r1x-r0x)*X0)== 0)&&data[i].G_compensate == 41 && data[i].G_pathmode == 3)||((((r1z-r0z)*Z0 +(r1x-r0x)*X0)== 0)&&data[i].G_compensate == 42 && data[i].G_pathmode == 2))//������
						{
         					 	S1z= Z0 + r0z;	S1x=X0 + r0x; //��һ���߶��յ㴦�İ뾶ʸ����������;
							S2z= Z0 + r1z;	S2x=X0 + r1x;//�ڶ�������㴦�İ뾶ʸ����������;
										//S1,S2���������;
							if(X0 > 0) Sign1 = 1;
							else if(X0 == 0) Sign1 = 0;
							else Sign1 = -1;
								
							if(Z0 > 0) Sign2 = 1;
							else if(Z0 == 0) Sign2 = 0;
							else Sign2 = -1;

							if(data[i].G_pathmode ==3)
							{
								if(K_two > 0) Sign3 = 1;
								else if(K_two == 0) Sign3 = 0;
								else Sign3 = -1;

								if((-I_two) > 0) Sign4 = 1;
								else if((-I_two) == 0) Sign4 = 0;
								else Sign4 = -1;
							}
							else if(data[i].G_pathmode ==2)
							{
								if((-K_two) > 0) Sign3 = 1;
								else if((-K_two) == 0) Sign3 = 0;
								else Sign3 = -1;

								if(I_two > 0) Sign4 = 1;
								else if(I_two == 0) Sign4 = 0;
								else Sign4 = -1;
							}
							
							if(r0x <0) r0x_abs = -r0x;
							else r0x_abs = r0x;

						if(r0z <0) r0z_abs = -r0z;
						else r0z_abs = r0z;

						if(r1x <0) r1x_abs = -r1x;
						else r1x_abs = r1x;

						if(r1z <0) r1z_abs = -r1z;
						else r1z_abs = r1z;

						S3x = S1x + Sign1*r0z_abs;
						S3z = S1z + Sign2*r0x_abs; 	

						S4x = S2x + Sign3*r1z_abs;
						S4z = S2z + Sign4*r1x_abs; 
						
						as[a]= cs[c-1];		//��ֵ,����,��һ��ֱ��
						if(cs[c-1].G_plane == 18)
						{
							as[a].Z = S3z - Start_Z;
							as[a].X = S3x - Start_X;
						}
						else if(cs[c-1].G_plane == 17)
						{
							as[a].X = S3z - Start_Z;
							as[a].Y = S3x - Start_X;
						}
						else if(cs[c-1].G_plane == 19)
						{
							as[a].Y = S3z - Start_Z;
							as[a].Z = S3x - Start_X;
						}
						a++;

						as[a]= cs[c-1];		//��ֵ,����,�����ֱ��S3S4����;
						if(cs[c-1].G_plane == 18)
						{
							as[a].Z = S4z - S3z;
							as[a].X = S4x - S3x;
							as[a].row_id=a+1;
						}
						else if(cs[c-1].G_plane == 17)
						{
							as[a].X = S4z - S3z;
							as[a].Y = S4x - S3x;
							as[a].row_id=a+1;
						}
						else if(cs[c-1].G_plane == 19)
						{
							as[a].Y = S4z - S3z;
							as[a].Z = S4x - S3x;
							as[a].row_id=a+1;
						}
						a++;
						
						as[a]= cs[c-1];		//�������S4S2����;
						if(cs[c-1].G_plane == 18)
						{
							as[a].Z = S2z - S4z;
							as[a].X = S2x - S4x;
							as[a].row_id=a+1;
						}
						else if(cs[c-1].G_plane == 17)
						{
							as[a].X = S2z - S4z;
							as[a].Y = S2x - S4x;
							as[a].row_id=a+1;
						}
						else if(cs[c-1].G_plane == 19)
						{
							as[a].Y = S2z - S4z;
							as[a].Z = S2x - S4x;
							as[a].row_id=a+1;
						}
						a++;

						Start_Z = S2z - Z0;	
						Start_X = S2x - X0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
					}//����			
						if(data[i].G_compensate == 40)       //ȡ������;
						{
							as[a]= data[i];		//��ֵ,����
							if(cs[c-1].G_plane == 18)
							{
								as[a].Z = Z1 - Start_Z;
								as[a].X = X1 - Start_X;
							}
							else if(cs[c-1].G_plane == 17)
							{
								as[a].X = Z1 - Start_Z;
								as[a].Y = X1 - Start_X;
							}
							else if(cs[c-1].G_plane == 19)
							{
								as[a].Y = Z1 - Start_Z;
								as[a].Z = X1 - Start_X;
							}
				printf("����ȡ�� as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
							a++;
							sign = 0;//��������
							c = 0;//Ϊ��һ�ε�������ʼ��.
							Start_Z = 0.;	
							Start_X = 0.;
						}
			}//ֱ�߽�Բ��
else if((data[i].G_pathmode==2 || data[i].G_pathmode==3)&& (cs[c-1].G_pathmode==2 || cs[c-1].G_pathmode==3))//Բ����Բ��
			{
				if(cs[c-1].G_plane == 18)
				{
					Z0 = cs[c-1].Z;
					X0 = cs[c-1].X;
					K_one=cs[c-1].K;
					I_one=cs[c-1].I;

					Z1 = data[i].Z;
					X1 = data[i].X;
					K_two=data[i].K;
					I_two=data[i].I;
				}
				else if(cs[c-1].G_plane == 17)
				{
					Z0 = cs[c-1].X;
					X0 = cs[c-1].Y;
					K_one=cs[c-1].I;
					I_one=cs[c-1].J;

					Z1 = data[i].X;
					X1 = data[i].Y;
					K_two=data[i].I;
					I_two=data[i].J;
				}
				else if(cs[c-1].G_plane == 19)
				{
					Z0 = cs[c-1].Y;
					X0 = cs[c-1].Z;
					K_one=cs[c-1].J;
					I_one=cs[c-1].K;

					Z1 = data[i].Y;
					X1 = data[i].Z;
					K_two=data[i].J;
					I_two=data[i].K;
				}
				if(cs[c-1].G_coormode == 91)		//G90�������Ҫ����......................;
				{
					d0 = sqrt((X0-I_one)*(X0-I_one)+(Z0-K_one)*(Z0-K_one));	
					d1 = sqrt((X1-I_two)*(X1-I_two)+(Z1-K_two)*(Z1-K_two));	
					
	if((cs[c-1].G_compensate == 41 && cs[c-1].G_pathmode == 2)||(cs[c-1].G_compensate == 42 && cs[c-1].G_pathmode == 3))
					{
						r0x = R*(X0-I_one)/d0;
						r0z = R*(Z0-K_one)/d0;
					}
	else if((cs[c-1].G_compensate == 42 && cs[c-1].G_pathmode == 2)||(cs[c-1].G_compensate == 41 && cs[c-1].G_pathmode == 3))
					{
						r0x = (-R)*(X0-I_one)/d0;
						r0z = (-R)*(Z0-K_one)/d0;
					}

	if((data[i].G_compensate == 41 && data[i].G_pathmode == 2)||(data[i].G_compensate == 42 && data[i].G_pathmode == 3))
					{
						r1x = R*(-I_two)/d1;
						r1z = R*(-K_two)/d1;
					}
	else if((data[i].G_compensate == 42 && data[i].G_pathmode == 2)||(data[i].G_compensate == 41 && data[i].G_pathmode == 3))
					{
						r1x = R*I_two/d1;
						r1z = R*K_two/d1;
					}
					//ȷ��r0x,r0z,r1x,r1z����;

					if(cs[c-1].G_pathmode == 2)
					{
						KBx0= -(Z0-K_one);
						KBz0=X0-I_one;
					}
					else if(cs[c-1].G_pathmode == 3)
					{
						KBx0=Z0-K_one;
						KBz0= -(X0-I_one);
					}// ȷ��KB����;
					if(data[i].G_pathmode == 2)
					{
						KBx1= K_two;
						KBz1=-I_two;
					}
					else if(data[i].G_pathmode == 3)
					{
						KBx1=-K_two;
						KBz1= I_two;
					}// ȷ��KB����,Բ����㴦������;
				}//G91
			
			//�жϽ�������:����,����,�쳤
					if( ((r0x*r1x + r0z*r1z) >= 0) && (((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0) >= 0) )//�쳤��	
					{
						if(KBz0 == 0 && KBz1 != 0)
						{
						S1z= Z0+r0z;	S1x=X0+r0x; 
						S2z= Z0+r1z;	S2x=X0+r1x;K2=KBx1/KBz1;
						
						Sz = S1z;
						Sx = K2*(Sz - S2z) + S2x;//ֱ��б�����޴�ʱ;					
						}
						else if(KBz0 != 0 && KBz1 == 0)
						{
						S1z= Z0+r0z;	S1x=X0+r0x;K1=KBx0/KBz0; 
						S2z= Z0+r1z;	S2x=X0+r1x;
						
						Sz = S2z;
						Sx = K1*(Sz - S1z) + S1x;//ֱ��б�����޴�ʱ;					
						}
						else if((KBx0*KBz1) == (KBx1*KBz0))
						{
						Sz = Z0+r0z;
						Sx = X0+r0x;			
						}//ֱ��б�����ʱ;
						else
						{
						S1z= Z0+r0z;	S1x=X0+r0x;K1=KBx0/KBz0; 
						S2z= Z0+r1z;	S2x=X0+r1x;K2=KBx1/KBz1;
						
						Sz = (S2x - S1x - K2*S2z + K1*S1z)/(K1 - K2);
						Sx = K1*(Sz - S1z) + S1x;
						}//�쳤�ߵĽ�������;					
					

						as[a]= cs[c-1];		//��ֵ,����
						if(cs[c-1].G_plane == 18)
						{
							as[a].Z = (Z0+r0z)-Start_Z;
							as[a].X = (X0+r0x)-Start_X;
							as[a].K = K_one-Start_Z;
							as[a].I = I_one-Start_X;
						}
						else if(cs[c-1].G_plane == 17)
						{
							as[a].X = (Z0+r0z)-Start_Z;
							as[a].Y = (X0+r0x)-Start_X;
							as[a].I = K_one-Start_Z;
							as[a].J = I_one-Start_X;
						}
						else if(cs[c-1].G_plane == 19)
						{
							as[a].Y = (Z0+r0z)-Start_Z;
							as[a].Z = (X0+r0x)-Start_X;
							as[a].J = K_one-Start_Z;
							as[a].K = I_one-Start_X;
						}
						a++;//��һ��Բ��;

						//��ֵ,����,��һ��Բ�����쳤ֱ��;
						as[a].G_pathmode=1;
						as[a].G_coormode=91;
						if(cs[c-1].G_plane == 18)
						{
							as[a].Z = Sz-(Z0+r0z);
							as[a].X = Sx-(X0+r0x);
						}
						else if(cs[c-1].G_plane == 17)
						{
							as[a].X = Sz-(Z0+r0z);
							as[a].Y = Sx-(X0+r0x);
						}
						else if(cs[c-1].G_plane == 19)
						{
							as[a].Y = Sz-(Z0+r0z);
							as[a].Z = Sx-(X0+r0x);
						}
						a++;//��һ��Բ�����쳤ֱ�߲���;

						as[a].G_pathmode=1;
						as[a].G_coormode=91;
						if(cs[c-1].G_plane == 18)
						{
							as[a].Z = (Z0+r1z)-Sz;
							as[a].X = (X0+r1x)-Sx;
						}
						else if(cs[c-1].G_plane == 17)
						{
							as[a].X = (Z0+r1z)-Sz;
							as[a].Y = (X0+r1x)-Sx;
						}
						else if(cs[c-1].G_plane == 19)
						{
							as[a].Y = (Z0+r1z)-Sz;
							as[a].Z = (X0+r1x)-Sx;
						}
						a++;//�ڶ���Բ�����ߵ��쳤����;
						Start_Z = r1z;	
						Start_X = r1x;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
						
					}//�쳤��;

					else if(( ((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)< 0) ||((((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)== 0)&&cs[c-1].G_compensate == 41 && cs[c-1].G_pathmode == 2)||((((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)== 0)&&cs[c-1].G_compensate == 42 && cs[c-1].G_pathmode == 3)) //������	
					{
  				printf("�����Ե���\n");
						S1z=Z0;	S1x=X0;
					
						Oz=K_one;	Ox=I_one;//Բ������;
						r_arc=sqrt( (Z0+r0z-K_one)*(Z0+r0z-K_one) + (X0+r0x-I_one)*(X0+r0x-I_one) );//Բ���뾶
						
						if(Z0+K_two-Oz==0)
						{
						S2z = K_one;
						S2x = S1x;
						}
						else if(X0+I_two-Ox==0)
						{
						S2z = S1z; 
						S2x = I_one;
						}
						else
						{
						Kline=(X0+I_two - Ox)/(Z0+K_two - Oz);//����Բ�ĵ�ֱ��б��;
						S2z=(Kline/(Kline*Kline+1))*(X0-Ox+Z0/Kline+Kline*Oz);//����Բ�ĵ�ֱ��,
										//�Ե�һ��Բ��Բ������Ϊ������,����
										//������Բ�������ߵ� ����;
						S2x=Kline*(S2z-Oz)+Ox;
						}
						if(S1z == S2z)
						{
						Sz1=S1z ;	Sx1=Ox + sqrt(r_arc*r_arc-(S1z-Oz)*(S1z-Oz));
						Sz2=S1z ;	Sx2=Ox - sqrt(r_arc*r_arc-(S1z-Oz)*(S1z-Oz));
						}
						else if(S1x == S2x)
						{
						Sz1=Oz + sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));	Sx1=S1x;
						Sz2=Oz - sqrt(r_arc*r_arc-(S1x-Ox)*(S1x-Ox));	Sx2=S1x;
						}
						else
						{
						K_arc=(S2z-S1z)/(S2x-S1x);
						a_arc=1+K_arc*K_arc;
						b_arc=2*K_arc*(S2z-Oz-K_arc*S2x-Ox/K_arc);
						c_arc=(S2z-Oz-K_arc*S2x)*(S2x-Oz-K_arc*S2x)-r_arc*r_arc+Ox*Ox;
						
					Sx1=( -b_arc+sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sz1=S2z + K_arc*(Sx1-S2x);
					Sx2=( -b_arc-sqrt(b_arc*b_arc-4*a_arc*c_arc) )/(2*a_arc);	Sz2=S2z + K_arc*(Sx2-S2x);
						}
						
	if( ((r1z*KBz0+r1x*KBx0)>=0 && ((Sz2-Sz1)*KBz0+(Sx2-Sx1)*KBx0)<=0) || ((r1z*KBz0+r1x*KBx0)<0 && ((Sz2-Sz1)*KBz0+(Sx2-Sx1)*KBx0)>0) )
							{
							Sx=Sx2;
							Sz=Sz2;
							}
	else if(((r1z*KBz0+r1x*KBx0)>=0 && ((Sz2-Sz1)*KBz0+(Sx2-Sx1)*KBx0)>0)||((r1z*KBz0+r1x*KBx0)<0 && ((Sz2-Sz1)*KBz0+(Sx2-Sx1)*KBx0)<=0) )
							{
							Sx=Sx1;
							Sz=Sz1;
							}
							as[a]= cs[c-1];		//��ֵ,����
							if(cs[c-1].G_plane == 18)
							{
								as[a].Z = Sz - Start_Z;
								as[a].X = Sx - Start_X;
								as[a].K = K_one-Start_Z;
								as[a].I = I_one-Start_X;
							}
							else if(cs[c-1].G_plane == 17)
							{
								as[a].X = Sz - Start_Z;
								as[a].Y = Sx - Start_X;
								as[a].I = K_one-Start_Z;
                                                                as[a].J = I_one-Start_X;
							}
							else if(cs[c-1].G_plane == 19)
							{
								as[a].Y = Sz - Start_Z;
								as[a].Z = Sx - Start_X;
								as[a].J = K_one-Start_Z;
                                                                as[a].K = I_one-Start_X;
							}
							a++;

							Start_Z = Sz - Z0;	
							Start_X = Sx - X0;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
							
						}//������
				
					else if(( (((r1z-r0z)*KBz0 +(r1x-r0x)*KBx0) > 0) && ((r0x*r1x + r0z*r1z) < 0) )||((((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)== 0)&&cs[c-1].G_compensate == 42 && cs[c-1].G_pathmode == 2)||((((r1x-r0x)*KBx0 + (r1z-r0z)*KBz0)== 0)&&cs[c-1].G_compensate == 41 && cs[c-1].G_pathmode == 3))//������
						{
          printf("�����Ե���\n");
							S1z= Z0 + r0z;	S1x=X0 + r0x; //��һ���߶��յ㴦�İ뾶ʸ����������;
							S2z= Z0 + r1z;	S2x=X0 + r1x;//�ڶ�������㴦�İ뾶ʸ����������;
										//S1,S2���������;

							if(cs[c-1].G_pathmode ==3)
							{
								if((Z0-K_one) > 0) Sign1 = 1;
								else if((Z0-K_one) == 0) Sign1 = 0;
								else Sign1 = -1;

								if((I_one-X0) > 0) Sign2 = 1;
								else if((I_one-X0) == 0) Sign2 = 0;
								else Sign2 = -1;
							}
							else if(cs[c-1].G_pathmode ==2)
							{
								if((K_one-Z0) > 0) Sign1 = 1;
								else if((K_one-Z0) == 0) Sign1 = 0;
								else Sign1 = -1;

								if((X0-I_one) > 0) Sign2 = 1;
								else if((X0-I_one) == 0) Sign2 = 0;
								else Sign2 = -1;
							}
							if(data[i].G_pathmode ==3)
							{
								if(K_two > 0) Sign3 = 1;
								else if(K_two == 0) Sign3 = 0;
								else Sign3 = -1;

								if((-I_two) > 0) Sign4 = 1;
								else if((-I_two) == 0) Sign4 = 0;
								else Sign4 = -1;
							}
							else if(data[i].G_pathmode ==2)
							{
								if((-K_two) > 0) Sign3 = 1;
								else if((-K_two) == 0) Sign3 = 0;
								else Sign3 = -1;

								if(I_two > 0) Sign4 = 1;
								else if(I_two == 0) Sign4 = 0;
								else Sign4 = -1;
							}
							if(r0x <0) r0x_abs = -r0x;
							else r0x_abs = r0x;

							if(r0z <0) r0z_abs = -r0z;
							else r0z_abs = r0z;

							if(r1x <0) r1x_abs = -r1x;
							else r1x_abs = r1x;

							if(r1z <0) r1z_abs = -r1z;
							else r1z_abs = r1z;
							
	//printf("Sig1=%d,%d,%d,%d,r0x=%f,r0z=%f,r1x=%f,r1z=%f\n",Sign1,Sign2,Sign3,Sign4,r0x_abs,r0z_abs,r1x_abs,r1z_abs);

							S3x = S1x + Sign1*r0z_abs;
							S3z = S1z + Sign2*r0x_abs; 	

							S4x = S2x + Sign3*r1z_abs;
							S4z = S2z + Sign4*r1x_abs; 	

							as[a]= cs[c-1];		//��ֵ,����,Բ������
							if(cs[c-1].G_plane == 18)
							{
								as[a].Z = S1z - Start_Z;
								as[a].X = S1x - Start_X;
								as[a].K = K_one-Start_Z;
								as[a].I = I_one-Start_X;
							}
							else if(cs[c-1].G_plane == 17)
							{
								as[a].X = S1z - Start_Z;
								as[a].Y = S1x - Start_X;
								as[a].I = K_one-Start_Z;
                                                                as[a].J = I_one-Start_X;
							}
							else if(cs[c-1].G_plane == 19)
							{
								as[a].Y = S1z - Start_Z;
								as[a].Z = S1x - Start_X;
								as[a].J = K_one-Start_Z;
                                                                as[a].K = I_one-Start_X;
							}
							a++;

							as[a].G_pathmode=1;
							as[a].G_coormode=91;
							if(cs[c-1].G_plane == 18)
							{
								as[a].Z = S3z - S1z;
								as[a].X = S3x - S1x;
								as[a].row_id=a+1;
							}
							else if(cs[c-1].G_plane == 17)
							{
								as[a].X = S3z - S1z;
								as[a].Y = S3x - S1x;
								as[a].row_id=a+1;
							}
							else if(cs[c-1].G_plane == 19)
							{
								as[a].Y = S3z - S1z;
								as[a].Z = S3x - S1x;
								as[a].row_id=a+1;
							}
							a++;
							
							as[a].G_pathmode=1;
							as[a].G_coormode=91;
							if(cs[c-1].G_plane == 18)
							{
								as[a].Z = S4z - S3z;
								as[a].X = S4x - S3x;
								as[a].row_id=a+1;
							}
							else if(cs[c-1].G_plane == 17)
							{
								as[a].X = S4z - S3z;
								as[a].Y = S4x - S3x;
								as[a].row_id=a+1;
							}
							else if(cs[c-1].G_plane == 19)
							{
								as[a].Y = S4z - S3z;
								as[a].Z = S4x - S3x;
								as[a].row_id=a+1;
							}
							a++;


							as[a].G_pathmode=1;
							as[a].G_coormode=91;
							if(cs[c-1].G_plane == 18)
							{
								as[a].Z = S2z - S4z;
								as[a].X = S2x - S4x;
								as[a].row_id=a+1;
							}
							else if(cs[c-1].G_plane == 17)
							{
								as[a].X = S2z - S4z;
								as[a].Y = S2x - S4x;
								as[a].row_id=a+1;
							}
							else if(cs[c-1].G_plane == 19)
							{
								as[a].Y = S2z - S4z;
								as[a].Z = S2x - S4x;
								as[a].row_id=a+1;
							}
							a++;

							Start_Z = r1z;	
							Start_X = r1x;//ת�ӵ������ڶ���G���������������,Ϊ��һ�ε�����׼��;
						}//����
			
							if(data[i].G_compensate == 40)       //ȡ������;
							{
								as[a]= data[i];		//��ֵ,����
								if(cs[c-1].G_plane == 18)
								{
									as[a].Z = Z1 - Start_Z;
									as[a].X = X1 - Start_X;
								}
								else if(cs[c-1].G_plane == 17)
								{
									as[a].X = Z1 - Start_Z;
									as[a].Y = X1 - Start_X;
								}
								else if(cs[c-1].G_plane == 19)
								{
									as[a].Y = Z1 - Start_Z;
									as[a].Z = X1 - Start_X;
								}
					printf("����ȡ�� as[%d] Y:%f Z:%f\n",a,as[a].Y,as[a].Z);
								a++;
								sign = 0;//��������
								c = 0;//Ϊ��һ�ε�������ʼ��.
								Start_Z = 0.;	
								Start_X = 0.;
							}//ȡ������  */
				}//Բ����Բ��*/

			}//����������
   		 if(data[i].G_compensate != 40)
			{
				cs[c]=data[i];	
				c++;
			}
		}//���е���

		else {as[a]=data[i]; 
          printf("tiao chu bu chang\n");
				a++;}//����Ҫ��������ʱ;

	}		//for

	/*��Z��������д���ͼ����ʾ�ϵ�Y��*/
		
	/*���괦��-"��������"*/
	m_point point[a+2];		//ÿ�в���һ��"����"����� 

	point[0].X=0;
        point[0].Y=0;

	for(i=0;i<=a;i++)
	{
		double  u1,u2;
		u1=as[i].Z;
		u2=-u1;
		as[i].Z=u2;		//ԭ����Y������,�ֱ���������
		point[i+1].X = point[i].X + as[i].Y*1000;
		point[i+1].Y = point[i].Y + as[i].Z*1000;
	}
	
	/*����ʾͼ�εĳ���*/
	double m , n, a1, b ,width,length,mw,ml;
	m=n=0;
	a1=b=0;
	for(i=0;i<=(a+1);i++)
	{
		/*Y��*/
		if(point[i].X >= 0 )
		{
			if(m < point[i].X)
			m = point[i].X;
		}
		else
		{
			if(n>point[i].X)
			n=point[i].X;	
		}
		/*Z��*/
		if(point[i].Y >=0)
		{
			if(a1<point[i].Y)
			a1 = point[i].Y;
		}
		else
		{
			if(b>point[i].Y)
			b = point[i].Y;
		}
	}
	width=(m -n);
	length=(a1 - b);
	//printf("%f %f\n",width,length);
	/*��ͼ׼��*/
	//HWND h1=GetDlgItem (hEEdit, IDC_GRAPH);
	RECT r1;
	HDC PCD1= GetDC(h1);
	int angle1, angle2, sx, sy,sr;
	double sm, R1;
	
	GetClientRect(h1 ,&r1);
		
	WC=(float)((r1.right-r1.left)/(width*1.2));
	LC=(float)((r1.bottom-r1.top)/(length*1.2));

	if(width>=length)
	{
		mw=((m+n)/2)*WC;
		ml=((a1+b)/2)*WC;
		for(i=0;i<=(a+1);i++)
        	{
        	point[i].X=point[i].X*WC+(r1.right*0.5-mw);
		point[i].Y=point[i].Y*WC+(r1.bottom*0.5-ml);		//��Ϊ��������Y������,���д�����Ϊ�˰���ʾͼ��������������
        	}
	}
	else
	{
		mw=((m+n)/2)*LC;
		ml=((a1+b)/2)*LC;
		for(i=0;i<=(a+1);i++)
                {
                point[i].X=point[i].X*LC+(r1.right*0.5-mw);
                point[i].Y=point[i].Y*LC+(r1.bottom*0.5-ml);
                }
	}
	
	SetPenWidth(PCD1, 20);
	SetPenType(PCD1, PT_SOLID);
	SetPenColor(PCD1, PIXEL_green);
	SetPenJoinStyle(PCD1, PT_JOIN_ROUND);
	SetRasterOperation(PCD1,ROP_XOR);
/*���ؼ���ͼ��*/
/*	HICON myicon1;
	myicon1=LoadIconFromFile(PCD1,"/home/minigui/minigui-res-1.6/icon/w95mbx01.ico",0);
	if(myicon1==0)
	printf("load icon file fail\n");*/


	
	for(i=0;i<=a;i++)
	{	
	
		if (as[i].G_pathmode==1)
		{
		MoveTo(PCD1,(int) point[i].X, (int) point[i].Y);
		LineTo(PCD1,(int) point[i+1].X,(int)point[i+1].Y);
		//DrawIcon(PCD1,(int)(point[i+1].X-3),(int)(point[i+1].Y-3),6,6,myicon1);
		//printf("%f %f\n",point[i+1].X,point[i+1].Y);
		}

		else if(as[i].G_pathmode==2)
			{
			R1=sqrt(as[i].J*as[i].J + as[i].K*as[i].K);
			sm=as[i].Y*as[i].Y + as[i].Z*as[i].Z;
			//printf("%f %f\n",R,sm);
			
			double  m1,m2;
			m1=as[i].K;
			m2=-m1;
			as[i].K=m2;
	
			if(as[i].J>0&&as[i].K==0)		//data[i].K��data[i].Z���Ǿ���ȡ����,����ʱע��
			{
				angle2=180;
				if(as[i].Y*as[i].Z<=0)
				angle1=(int)(180-2*180*asin(sqrt(sm)/(2*R1))/3.14);
				else
				angle1=(int)(2*180*asin(sqrt(sm)/(2*R1))/3.14-180);
			}
			if(as[i].J==0&&as[i].K<0)
			{
			//	printf("ok?\n");
				angle2=-90;
				if(as[i].Y*as[i].Z>=0)
                                angle1=(int)(270-2*180*asin(sqrt(sm)/(2*R1))/3.14);
                                else
                                angle1=(int)(2*180*asin(sqrt(sm)/(2*R1))/3.14-90);

			}
			if(as[i].J<0&&as[i].K==0)
			{
				angle2=0;
				if(as[i].Y*as[i].Z<=0)
                                angle1=(int)(-2*180*asin(sqrt(sm)/(2*R1))/3.14);
                                else
                                angle1=(int)(2*180*asin(sqrt(sm)/(2*R1))/3.14);

			}
			if(as[i].J==0&&as[i].K>0)
			{
				angle2=90;
				if(as[i].Y*as[i].Z>=0)
                                angle1=(int)(-2*180*asin(sqrt(sm)/(2*R1))/3.14+90);
                                else
                                angle1=(int)(90+2*180*asin(sqrt(sm)/(2*R1))/3.14);

			}
			/*if(as[i].J>0&&as[i].K<0)			//���Ӳ�����Բ����Ҫǰ����������G����
			{
				angle2=180*atan(-as[i].K/as[i].J)/3.14+180;
				if(as[i].Z<=as[i].K/as[i].J*as[i].Y)
				angle1=180-(2*180*asin(sqrt(sm)/(2*R1))/3.14+180*atan(as[i].K/as[i].J)/3.14);
				else
				angle1=-(180-2*180*asin(sqrt(sm)/(2*R1))/3.14+180*atan(as[i].K/as[i].J)/3.14);
			}
			if(as[i].J>0&&as[i].K>0)
			{
				angle2=180-180*atan(as[i].K/as[i].J)/3.14;
				if(as[i].Z<=as[i].K/as[i].J*as[i].Y)
				angle1=-2*180*asin(sqrt(sm)/(2*R1))/3.14+90+180*atan(as[i].K/as[i].J)/3.14;
				else
				angle1=-180+2*180*asin(sqrt(sm)/(2*R1))/3.14-180*atan(as[i].K/as[i].J)/3.14;
			}
			if(as[i].J<0&&as[i].K<0)
			{
				angle2=-180*atan(as[i].K/as[i].J)/3.14;
				if(data[i].Z>=as[i].K/as[i].J*as[i].Y)
				angle1=-2*180*asin(sqrt(sm)/(2*R1))/3.14-180*atan(as[i].K/as[i].J)/3.14;
				else
				angle1=2*180*asin(sqrt(sm)/(2*R1))/3.14-180*atan(as[i].K/as[i].J)/3.14;
			}
			if(as[i].J<0&&as[i].K>0)
			{
				angle2=-180*atan(as[i].K/as[i].J)/3.14;
				if(as[i].Z>=as[i].K/as[i].J*as[i].Y)
				angle1=-2*180*asin(sqrt(sm)/(2*R1))/3.14-180*atan(as[i].K/as[i].J)/3.14;
				else
				angle1=2*180*asin(sqrt(sm)/(2*R1))/3.14-180*atan(as[i].K/as[i].J)/3.14;
			}
*/
			if(width>=length)
			{
			sx=(int)(point[i].X+as[i].J*1000*WC);
			sy=(int)(point[i].Y+as[i].K*1000*WC);
			sr=(int)(R1*1000*WC);
			}else
			{
			sx=(int)(point[i].X+as[i].J*1000*LC);
                        sy=(int)(point[i].Y+as[i].K*1000*LC);
                        sr=(int)(R1*1000*LC);

			}
			CircleArc(PCD1,sx,sy,sr,angle1*64,angle2*64);
			printf("%d %d %d %d %d\n",sx,sy,sr,angle1,angle2);
			}
		else if(as[i].G_pathmode==3)
			{
			R1=sqrt(as[i].J*as[i].J + as[i].K*as[i].K);
                        sm=as[i].Y*as[i].Y + as[i].Z*as[i].Z;
                        //printf("%f %f\n",R,sm);
                                                                                                                                               
                        double  m1,m2;
                        m1=as[i].K;
                        m2=-m1;
                        as[i].K=m2;
                                                                                                                                               
                        if(as[i].J>0&&as[i].K==0)           //data[i].K��data[i].Z���Ǿ���ȡ����,����ʱע��
                        {
                                angle1=180;
                                if(as[i].Y*as[i].Z<=0)
                                angle2=(int)(180-2*180*asin(sqrt(sm)/(2*R1))/3.14);
                                else
                                angle2=(int)(2*180*asin(sqrt(sm)/(2*R1))/3.14-180);
                        }
                        if(as[i].J==0&&as[i].K<0)
                        {
                        //      printf("ok?\n");
                                angle1=-90;
                                if(as[i].Y*as[i].Z>=0)
                                angle2=(int)(270-2*180*asin(sqrt(sm)/(2*R1))/3.14);
                                else
                                angle2=(int)(2*180*asin(sqrt(sm)/(2*R1))/3.14-90);
                                                                                                                                               
                        }
			if(as[i].J<0&&as[i].K==0)
                        {
                                angle1=0;
                                if(as[i].Y*as[i].Z<=0)
                                angle2=(int)(-2*180*asin(sqrt(sm)/(2*R1))/3.14);
                                else
                                angle2=(int)(2*180*asin(sqrt(sm)/(2*R1))/3.14);
                                                                                                                                               
                        }
                        if(as[i].J==0&&as[i].K>0)
                        {
                                angle1=90;
                                if(as[i].Y*as[i].Z>=0)
                                angle2=(int)(-2*180*asin(sqrt(sm)/(2*R1))/3.14+90);
                                else
                                angle2=(int)(90+2*180*asin(sqrt(sm)/(2*R1))/3.14);
                                                                                                                                               
                        }
		/*
			if(as[i].J>0&&as[i].K<0)                  //���Ӳ�����Բ����Ҫǰ����������G����
                        {
                                angle1=180*atan(-as[i].K/as[i].J)/3.14+180;
                                if(as[i].Z<=as[i].K/as[i].J*as[i].Y)
                                angle2=180-(2*180*asin(sqrt(sm)/(2*R1))/3.14+180*atan(as[i].K/as[i].J)/3.14);
                                else
                                angle2=-(180-2*180*asin(sqrt(sm)/(2*R1))/3.14+180*atan(as[i].K/as[i].J)/3.14);
                        }
                        if(as[i].J>0&&as[i].K>0)
                        {
                                angle1=180-180*atan(as[i].K/as[i].J)/3.14;
                                if(as[i].Z<=as[i].K/as[i].J*as[i].Y)
                                angle2=-2*180*asin(sqrt(sm)/(2*R1))/3.14+90+180*atan(as[i].K/as[i].J)/3.14;
                                else
                                angle2=-180+2*180*asin(sqrt(sm)/(2*R1))/3.14-180*atan(as[i].K/as[i].J)/3.14;
                        }
                        if(as[i].J<0&&as[i].K<0)
                        {
                                angle1=-180*atan(as[i].K/as[i].J)/3.14;
                                if(as[i].Z>=as[i].K/as[i].J*as[i].Y)
                                angle2=-2*180*asin(sqrt(sm)/(2*R1))/3.14-180*atan(as[i].K/as[i].J)/3.14;
                                else
                                angle2=2*180*asin(sqrt(sm)/(2*R1))/3.14-180*atan(as[i].K/as[i].J)/3.14;
                        }
                        if(as[i].J<0&&as[i].K>0)
                        {
                                angle1=-180*atan(as[i].K/as[i].J)/3.14;
                                if(as[i].Z>=as[i].K/as[i].J*as[i].Y)
                                angle2=-2*180*asin(sqrt(sm)/(2*R1))/3.14-180*atan(as[i].K/as[i].J)/3.14;
                                else
                                angle2=2*180*asin(sqrt(sm)/(2*R1))/3.14-180*atan(as[i].K/as[i].J)/3.14;
                        }
	*/
			 if(width>=length)
                        {
                        sx=(int)(point[i].X+as[i].J*1000*WC);
                        sy=(int)(point[i].Y+as[i].K*1000*WC);
                        sr=(int)(R1*1000*WC);
                        }else
                        {
                        sx=(int)(point[i].X+as[i].J*1000*LC);
                        sy=(int)(point[i].Y+as[i].K*1000*LC);
                        sr=(int)(R1*1000*LC);
                                                                                                                                               
                        }
                        CircleArc(PCD1,sx,sy,sr,angle1*64,angle2*64);
                        printf("%d %d %d %d %d\n",sx,sy,sr,angle1,angle2);
			
			}
	}
	
	ReleaseDC(PCD1);
	
	if (subItem.pszText != NULL ) free (subItem.pszText); 
			}
	}
}
