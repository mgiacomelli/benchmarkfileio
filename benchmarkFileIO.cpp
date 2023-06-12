// benchmarkFileIO.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include "stdio.h"
//#include <windows.h>
#include <wtypes.h>
#include <math.h>

#include <stdlib.h>
#include <windows.h>

#include <intrin.h>
#include <immintrin.h>

#include <sstream>
#include <iostream>
#include <fstream>

int writeOsman(int* buf, int bytesToWrite);
int writePlainC(int* buf, int bytesToWrite);
int writeWin32(int* buf, int bytesToWrite);
int writeWin32Unbuffered(int* buf, int bytesToWrite);

LARGE_INTEGER frequency;
LARGE_INTEGER start;
LARGE_INTEGER end;

//cpp fileio
int bufferSize = 4096 * 1024;
char* obuffer;

const char* outPath = "D:\\";

int main()
{
    int copiesToWrite = 5;
    long long bytesToWrite = 6656 * 1024 * 32;        //208 MB exactly
    //long long bytesToWrite = 2048 * 2048 * 4 * 2;        //32 MB exactly
    int* data = (int*)malloc(bytesToWrite);

    if (data == NULL)
        return -1;

    char* obuffer = new char[bufferSize];

    //check sector size 

    DWORD  lpSectorsPerCluster=0, lpBytesPerSector = 0, lpNumberOfFreeClusters = 0, lpTotalNumberOfClusters = 0;
    GetDiskFreeSpaceA(outPath, &lpSectorsPerCluster, &lpBytesPerSector, &lpNumberOfFreeClusters, &lpTotalNumberOfClusters);
    printf("Sector size:  %d\n", lpBytesPerSector);


    printf("Testing write to %s\n", outPath);


    //randomize contents
    for (int i = 0; i < bytesToWrite / 4; i++)
    {
        data[i] = rand();
    }

    double interval;

    QueryPerformanceFrequency(&frequency);


    //test Osman's write code

    QueryPerformanceCounter(&start);

    for(int i=0; i< copiesToWrite; i++)
        writeOsman(data, bytesToWrite);

    QueryPerformanceCounter(&end);

    interval = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;

    printf("CPP: %f (%f MB/s)\n", interval, bytesToWrite* copiesToWrite / interval/1024/1024);
    Sleep(1000);

    //test plain c f code

    QueryPerformanceCounter(&start);
    
    for (int i = 0; i < copiesToWrite; i++)
        writePlainC(data, bytesToWrite);

    QueryPerformanceCounter(&end);
    interval = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
    
    printf("C I/O: %f (%f MB/s)\n", interval, bytesToWrite * copiesToWrite / interval / 1024 / 1024);
    Sleep(1000);

    //test basic win32

    QueryPerformanceCounter(&start);

    for (int i = 0; i < copiesToWrite; i++)
        writeWin32(data, bytesToWrite);

    QueryPerformanceCounter(&end);
    interval = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;

    printf("Win32: %f (%f MB/s)\n", interval, bytesToWrite * copiesToWrite / interval / 1024 / 1024);
    Sleep(1000);

    //test unbuffered win32

    QueryPerformanceCounter(&start);

    for (int i = 0; i < copiesToWrite; i++)
        writeWin32Unbuffered(data, bytesToWrite);

    QueryPerformanceCounter(&end);
    interval = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;

    printf("Win32 Unbuffered: %f (%f MB/s)\n", interval, bytesToWrite * copiesToWrite / interval / 1024 / 1024);

}

int writeOsman(int* buf, int bytesToWrite)
{
    char fullPath[512];
    fullPath[0] = '\0';
    strcat(fullPath, outPath);
    //write out the NLM frame
    std::ofstream ofs(strcat(fullPath, "cpp_output.bin"), std::ofstream::binary);
    ofs.rdbuf()->pubsetbuf(obuffer, bufferSize);
    ofs.write((char*)buf, bytesToWrite);

    return 0;
}

int writePlainC(int* buf, int bytesToWrite)
{
    char fullPath[512];
    fullPath[0] = '\0';
    strcat(fullPath, outPath);
    FILE *f = fopen(strcat(fullPath, "c_output.bin"), "wb");
    fwrite(buf, 1, bytesToWrite, f);
    fclose(f);
    
    return 0;
}

int writeWin32(int* buf, int bytesToWrite)
{
    HANDLE hFile;
    BOOL bErrorFlag = FALSE;
    DWORD dwBytesWritten;
    char fullPath[512];
    fullPath[0] = '\0';
    strcat(fullPath, outPath);

    hFile = CreateFileA(strcat(fullPath, "CreateFile.bin"),    // name of the write
        GENERIC_WRITE,          // open for writing
        0,                      // do not share
        NULL,                   // default security
        CREATE_ALWAYS,             // create new file only
        FILE_ATTRIBUTE_NORMAL,  // normal file
        NULL);                  // no attr. template

    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("Error:  could not CreateFileA\n");
    }

    bErrorFlag = WriteFile(
        hFile,           // open file handle
        buf,      // start of data to write
        bytesToWrite,  // number of bytes to write
        &dwBytesWritten, // number of bytes that were written
        NULL);            // no overlapped structure

    if (FALSE == bErrorFlag)
    {
        
        printf("Terminal failure: Unable to write to file.\n");
    }
    else
    {
        if (dwBytesWritten != bytesToWrite)
        {
            printf("Error: dwBytesWritten != dwBytesToWrite\n");
        }
    }

    CloseHandle(hFile);

    return 0;

}

int writeWin32Unbuffered(int* buf, int bytesToWrite)
{
    HANDLE hFile;
    BOOL bErrorFlag = FALSE;
    DWORD dwBytesWritten;
   
    char fullPath[512];
    fullPath[0] = '\0';
    strcat(fullPath, outPath);

    //hFile = CreateFileA("D:\\temp\\CreateFileUnbuffered.bin",    // name of the write
    hFile = CreateFileA(strcat(fullPath,"CreateFileUnbuffered.bin"),    // name of the write
        GENERIC_WRITE,          // open for writing
        0,                      // do not share
        NULL,                   // default security
        CREATE_ALWAYS,             // create new file only
        FILE_FLAG_NO_BUFFERING,  // normal file
        NULL);                  // no attr. template

    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("Error:  could not CreateFileA\n");
    }

    bErrorFlag = WriteFile(
        hFile,           // open file handle
        buf,      // start of data to write
        bytesToWrite,  // number of bytes to write
        &dwBytesWritten, // number of bytes that were written
        NULL);            // no overlapped structure

    if (FALSE == bErrorFlag)
    {

        printf("Terminal failure: Unable to write to file.\n");
    }
    else
    {
        if (dwBytesWritten != bytesToWrite)
        {
            printf("Error: dwBytesWritten != dwBytesToWrite\n");
        }
    }

    CloseHandle(hFile);

    return 0;

}


