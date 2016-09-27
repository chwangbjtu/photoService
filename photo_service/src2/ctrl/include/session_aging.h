#ifndef __SESSION_AGING_H
#define __SESSION_AGING_H
#include "ktask.h"

class session_aging : public fsk::ktask	
{
public:
	session_aging();
	~session_aging();
	virtual int run(const time_t now);
private:
	unsigned int _cnt;
};

#endif //__SESSION_AGING_H

