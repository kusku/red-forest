#ifndef _BASE_H
#define _BASE_H

#include "Utils\Types.h"
#include <assert.h>

#define CHECKED_DELETE(x) if(x!=NULL){ delete x; x=NULL; }
#define CHECKED_RELEASE(x) if(x!=NULL){ x->Release(); x=NULL; }
#define CHECKED_DELETE_ARRAY(x) if(x){ delete [] x; x=NULL; }

#endif