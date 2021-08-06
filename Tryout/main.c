#include "connmgr.h"

int main(){
	connmgr_listen(5678);
	connmgr_free();
	return 0;
}