#include <stdio.h>
#include <windows.h>
#include <iphlpapi.h> // network adapters
#include <winsock2.h> // ip
#include <wininet.h> // (ip), internet connect

#pragma comment(lib, "wininet.lib")


// looking for os
int operating_system()
{
#ifdef _WIN32
    return 1; // win
#elif __linux__
    return 2; // lix
#elif TARGET_OS_MAC
    return 3; // mac
#else
    return 0; // none os
#endif
}






// gets pub ip
void get_public_ip() {
    HINTERNET hInternet, hConnect;
    DWORD bytesRead;
    char buffer[256];

    // initialization WinINet
    hInternet = InternetOpen("PublicIPCheck", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        printf("Chyba pri inicializaci WinINet.\n");
        return;
    }

    // connect to ipinfo.io
    hConnect = InternetOpenUrl(hInternet, "https://ipinfo.io/ip", NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        printf("Chyba pri pripojeni k serveru.\n");
        InternetCloseHandle(hInternet);
        return;
    }

    // reads response
    if (InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead)) {
        buffer[bytesRead] = '\0'; // Ukonceni retezce
        printf("Verjna IP adresa: %s\n", buffer);
    } else {
        printf("Chyba pri cteni odpovedi.\n");
    }

    // close connection
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
}


// gets mac addr
void get_mac_address() {
    IP_ADAPTER_INFO adapterInfo[16];
    DWORD dwBufLen = sizeof(adapterInfo);
    DWORD dwStatus = GetAdaptersInfo(adapterInfo, &dwBufLen);

    if (dwStatus == ERROR_SUCCESS) {
        PIP_ADAPTER_INFO pAdapterInfo = adapterInfo;
        while (pAdapterInfo) {
            printf("Jmeno adapteru: %s\n", pAdapterInfo->AdapterName);
            printf("Popis: %s\n", pAdapterInfo->Description);
            printf("MAC adresa: ");
            for (int i = 0; i < 6; i++) {
                printf("%02X", pAdapterInfo->Address[i]);
                if (i != 5) printf(":");
            }
            printf("\n");

            pAdapterInfo = pAdapterInfo->Next;
        }
    } else {
        printf("Chyba pri ziskavani informaci o sitovem adaptoru\n");
    }
}








int windows()
{
    // info os
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (GetVersionEx(&osvi)) {
        printf("Verze systemu: %ld.%ld\n", osvi.dwMajorVersion, osvi.dwMinorVersion);
        printf("Verze build: %ld\n", osvi.dwBuildNumber);
        printf("Verze platformy: %ld\n", osvi.dwPlatformId);
        printf("Nazev verze: %s\n", osvi.szCSDVersion);
    } else {
        printf("Chyba pri ziskavani verze systemu\n");
    }

    // info pc
    char computerName[256];
    DWORD size = sizeof(computerName);
    if (GetComputerName(computerName, &size)) {
        printf("Nazev pocitace: %s\n", computerName);
    } else {
        printf("Chyba pri ziskavani nazvu pocitace\n");
    }

    // info user
    char userName[256];
    size = sizeof(userName);
    if (GetUserName(userName, &size)) {
        printf("Uzivatelske jmeno: %s\n", userName);
    } else {
        printf("Chyba pri ziskavani uzivatelskeho jmena\n");
    }

    // info processor
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    printf("Pocet procesoru: %u\n", sysInfo.dwNumberOfProcessors);

    // info memory
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    if (GlobalMemoryStatusEx(&statex)) {
        printf("Celkova pamet: %llu MB\n", statex.ullTotalPhys / 1024 / 1024);
        printf("Volna pamet: %llu MB\n", statex.ullAvailPhys / 1024 / 1024);
    } else {
        printf("Chyba pri ziskavani informaci o pameti\n");
    }

    // info disk
    DWORD dwFreeSpace, dwTotalSpace, dwTotalFreeSpace;
    if (GetDiskFreeSpaceEx("C:", (PULARGE_INTEGER)&dwFreeSpace, (PULARGE_INTEGER)&dwTotalSpace, (PULARGE_INTEGER)&dwTotalFreeSpace)) {
        printf("Celkovi prostor na disku: %llu MB\n", dwTotalSpace / 1024 / 1024);
        printf("Volny prostor na disku: %llu MB\n", dwTotalFreeSpace / 1024 / 1024);
    } else {
        printf("Chyba pri ziskavani informaci o disku\n");
    }

    // info display device
    DISPLAY_DEVICE dd;
    memset(&dd, 0, sizeof(dd));
    dd.cb = sizeof(dd);
    int deviceIndex = 0;

    printf("Seznam zobrazovacich zarizeni:\n");
    while (EnumDisplayDevices(NULL, deviceIndex, &dd, 0)) {
        printf("Zarizeni: %s\n", dd.DeviceName);
        printf("Popis: %s\n", dd.DeviceString);
        deviceIndex++;
    }

    // info wlan
    IP_ADAPTER_INFO adapterInfo[16];
    DWORD dwBufLen = sizeof(adapterInfo); // Jediná deklarace
    DWORD dwStatus = GetAdaptersInfo(adapterInfo, &dwBufLen); // Jediná deklarace

    if (dwStatus == ERROR_SUCCESS) {
        PIP_ADAPTER_INFO pAdapterInfo = adapterInfo;
        printf("Seznam sitovych adaptoru:\n");
        while (pAdapterInfo) {
            printf("Jmeno adapteru: %s\n", pAdapterInfo->AdapterName);
            printf("Popis: %s\n", pAdapterInfo->Description);
            pAdapterInfo = pAdapterInfo->Next;
        }
    } else {
        printf("Chyba pri ziskavani informaci o sitovem adaptoru\n");
    }

    // info power
    SYSTEM_POWER_STATUS status;
    if (GetSystemPowerStatus(&status)) {
        if (status.ACLineStatus == 1) {
            printf("Zarizeni je pripojeno k napajeci siti\n");
        } else {
            printf("Zarizeni bezi na baterii\n");
        }
    } else {
        printf("Chyba pri ziskavani informaci o napajeni\n");
    }

    // initialization winstock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup selhal.\n");
        return 1;
    }

    // info adapters
    dwStatus = GetAdaptersInfo(adapterInfo, &dwBufLen); // Použití již existující proměnné

    if (dwStatus == ERROR_SUCCESS) {
        PIP_ADAPTER_INFO pAdapterInfo = adapterInfo;
        while (pAdapterInfo) {
            printf("Jmeno adapteru: %s\n", pAdapterInfo->AdapterName);
            printf("Popis: %s\n", pAdapterInfo->Description);

            // printf ip address
            PIP_ADDR_STRING pIpAddr = &pAdapterInfo->IpAddressList;
            while (pIpAddr) {
                printf("IP adresa: %s\n", pIpAddr->IpAddress.String);
                pIpAddr = pIpAddr->Next;
            }

            pAdapterInfo = pAdapterInfo->Next;
        }
    } else {
        printf("Chyba pri ziskavani informaci o sitovem adaptoru\n");
    }

    get_mac_address; // mac
    get_public_ip; // ip
    // end Winsock
    WSACleanup();

    return 0;
}


int main()
{
    int os  = operating_system(); // store varible from definition
    if (os == 1){
        windows(); // windows
    }
    
    return 0;
}






/* 
vyresit proc se nepise mac a ip momentalne funguje vse ostatni udělat ukládání do souboru a nahrání souboru na github do repositáře nebo na c2 server

dale budu muset udelat linux a macos 
*/
