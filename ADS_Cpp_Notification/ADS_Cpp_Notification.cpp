#include <Windows.h>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include "TcAdsDef.h"
#include "TcAdsAPI.h"

using namespace std;

void __stdcall SymbolChanged(AmsAddr*, AdsNotificationHeader*, ULONG);

map<uint32_t, void*> pointerMap;

unsigned short plc_n_cnt_1 = 0;
unsigned short plc_n_cnt_2 = 0;
unsigned short plc_n_cnt_3 = 0;

int main()
{
	long					nErr;
	AmsAddr					Addr;
	unsigned long			varHdl_1, varHdl_2, varHdl_3;
	unsigned long			hNotification_1, hNotification_2, hNotification_3;
	AdsNotificationAttrib	adsNotificationAttrib;


	char s_hndl_1[] = { "MAIN.n_cnt_2" };
	char s_hndl_2[] = { "MAIN.n_cnt_2" };
	char s_hndl_3[] = { "MAIN.n_cnt_3" };

	long   nTemp;
	AdsVersion* pDLLVersion;


	nTemp = AdsGetDllVersion();
	pDLLVersion = (AdsVersion*)&nTemp;


	// Open communication port on the ADS router
	int32_t port = AdsPortOpen();
	AdsGetLocalAddress(&Addr);

	//Addr.netId = {5, 80, 201, 232, 1, 1};
	Addr.port = 851;

	adsNotificationAttrib.cbLength = sizeof(uint32_t);
	adsNotificationAttrib.nTransMode = ADSTRANS_SERVERONCHA;
	adsNotificationAttrib.nCycleTime = 0;
	adsNotificationAttrib.nMaxDelay = 500;


	nErr = AdsSyncReadWriteReq(&Addr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(varHdl_1),
		&varHdl_1, sizeof(s_hndl_1), s_hndl_1);
	if (nErr) {
		cerr << "Error: AdsSyncReadWriteReq: " << nErr << '\n';
		exit(-1);
	}

	pointerMap.insert(std::make_pair(varHdl_1, (void*)&plc_n_cnt_1));


	nErr = AdsSyncReadWriteReq(&Addr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(varHdl_2),
		&varHdl_2, sizeof(s_hndl_2), s_hndl_2);
	if (nErr) {
		cerr << "Error: AdsSyncReadWriteReq: " << nErr << '\n';
		exit(-1);
	}

	pointerMap.insert(std::make_pair(varHdl_2, (void*)&plc_n_cnt_2));


	nErr = AdsSyncReadWriteReq(&Addr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(varHdl_3),
		&varHdl_3, sizeof(s_hndl_3), s_hndl_3);
	if (nErr) {
		cerr << "Error: AdsSyncReadWriteReq: " << nErr << '\n';
		exit(-1);
	}

	pointerMap.insert(std::make_pair(varHdl_3, (void*)&plc_n_cnt_3));

	/* REGISTER CALLBACK */
	nErr = AdsSyncAddDeviceNotificationReq(&Addr, ADSIGRP_SYM_VALBYHND, varHdl_1, &adsNotificationAttrib, SymbolChanged, varHdl_1, &hNotification_1);
	if (nErr) cerr << "Error: AdsSyncAddDeviceNotificationReq: " << nErr << '\n';

	nErr = AdsSyncAddDeviceNotificationReq(&Addr, ADSIGRP_SYM_VALBYHND, varHdl_2, &adsNotificationAttrib, SymbolChanged, varHdl_2, &hNotification_2);
	if (nErr) cerr << "Error: AdsSyncAddDeviceNotificationReq: " << nErr << '\n';

	nErr = AdsSyncAddDeviceNotificationReq(&Addr, ADSIGRP_SYM_VALBYHND, varHdl_3, &adsNotificationAttrib, SymbolChanged, varHdl_3, &hNotification_3);
	if (nErr) cerr << "Error: AdsSyncAddDeviceNotificationReq: " << nErr << '\n';

	char c;
	do {
		cout << "Press Enter to Exit" << endl;
		std::cin >> c;
	} while (c == '\r');

	// Release notofication
	nErr = AdsSyncDelDeviceNotificationReq(&Addr, hNotification_1);
	if (nErr) cerr << "Error: AdsSyncWriteReq: " << nErr << '\n';

	nErr = AdsSyncDelDeviceNotificationReq(&Addr, hNotification_2);
	if (nErr) cerr << "Error: AdsSyncWriteReq: " << nErr << '\n';

	//Release handle of plc variable
	nErr = AdsSyncWriteReq(&Addr, ADSIGRP_SYM_RELEASEHND, 0, sizeof(varHdl_1), &varHdl_1);
	if (nErr) cerr << "Error: AdsSyncWriteReq: " << nErr << '\n';

	nErr = AdsSyncWriteReq(&Addr, ADSIGRP_SYM_RELEASEHND, 0, sizeof(varHdl_2), &varHdl_2);
	if (nErr) cerr << "Error: AdsSyncWriteReq: " << nErr << '\n';

	// Close the communication port
	nErr = AdsPortClose();
	if (nErr) cerr << "Error: AdsPortClose: " << nErr << '\n';
}

// callback-function
void __stdcall SymbolChanged(AmsAddr* pAddr, AdsNotificationHeader* pNotification, ULONG hUser)
{
	unsigned short* tmp = (unsigned short*)pointerMap[(unsigned long)hUser]; // get address of global variables
	*tmp = *(unsigned short*)pNotification->data; // write data to global variables

	cout << __func__ << "() - " << "plc_n_cnt_1: " << plc_n_cnt_1 << "   plc_n_cnt_2: " << plc_n_cnt_2 << "   plc_n_cnt_3: " << plc_n_cnt_3 << endl;
}