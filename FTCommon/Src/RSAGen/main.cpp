#ifdef	_WIN32
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/des.h>

int main(int argc, char *argv[], char *envp[])
{
	RSA *rsa = RSA_generate_key(1024, 65537, NULL, NULL);
	RSA *pub = RSAPublicKey_dup(rsa);
	FILE *fp = fopen("rsa.prv", "wt");
	PEM_write_RSAPrivateKey(fp, rsa, NULL, NULL, 0, NULL, NULL);
	fclose(fp);
	fp = fopen("rsa.pub", "wt");
	PEM_write_RSAPublicKey(fp, pub);
	fclose(fp);
	return(0);
}
