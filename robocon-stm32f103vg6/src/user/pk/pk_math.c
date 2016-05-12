#include "pk_math.h"

int max(int n, ...) {
	va_list arguments;
	int max = va_arg(arguments, int); 
	for (int i = 1; i != n; i++ )        
    {
        int arg = va_arg(arguments, int);
		if (arg > max) {
			max = arg;
		}
    }
	return max;
}
