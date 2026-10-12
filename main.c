#include <stdio.h>

int add(int a, int b) {
	return a + b;
}

void main() {
	int r = add(11, 22);
	printf("%d", r);
}