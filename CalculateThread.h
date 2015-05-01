#ifndef CALTHREADH
#define CALTHREADH

#include <wx/thread.h>

#include "Sand.h"


class CalculateThread : public wxThread {
public:
	inline void drawData(int center);
	inline void found(particle *p, particle *otherp, int center, int other);
	void drawCircle(int centerx, int centery);
	void drawCircle(int centerx, int centery, int diameter, int sandtype);
	virtual void* Entry();
};



#endif

