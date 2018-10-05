#include<stdlib.h>
int stack[1000];
int topper = -1;
bool isempty()
{
	return topper == -1;
}
void push(int x)
{
	stack[++topper] = x;
}
void pop()
{
	topper--;
}
int top()
{
	return stack[topper];
}