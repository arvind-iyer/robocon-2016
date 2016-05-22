#include "pk_math.h"

int max(int n, ...) {
	va_list arguments;
	va_start(arguments, n);
	int max = va_arg(arguments, int); 
	for (int i = 1; i != n; i++ )        
    {
        int arg = va_arg(arguments, int);
		if (arg > max) {
			max = arg;
		}
    }
	va_end(arguments);
	return max;
}
