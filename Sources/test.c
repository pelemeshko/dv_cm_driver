#include "1986ve8_lib/cm4ikmcu.h"
#include "test.h"

void test(void)
{
	PORTC->KEY = _KEY_;
	PORTC->SANALOG =   0xFFC000FF;
	
}

