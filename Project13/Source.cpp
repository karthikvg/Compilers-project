/*
* NAME      : V.SAI KARTHIK
* PROJECT   : ASSEMBLY COMPILER
* INSTRUCTOR: ABJITH
* DATE      : 30-5-2018
* TASK ID   : 5
* TEAM ID   : 3
* COLLEGE   : VIGNAN CO-ED
*/
#define _CRT_SECURE_NO_WARNINGS 
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include "myheader.h"

#define register_size 8
#define	OPCODE_SIZE 15
#define LABEL_SIZE 26
#define MAX_REGISTERS 8
#define MEMORY_SIZE 500
#define SYMBOLS_SIZE 100
#define INTER_CODE_SIZE 100

struct label
{
	char block_name;
	int address;
};
struct symbol_table
{
	char name;
	int address;
	int size;
};
struct intercode
{
	int instruction_no;
	int opcode;
	int* p;
	int no_of_parameters;
};

char opcodes[OPCODE_SIZE][6] = { "ENDIF","MOV", "MOV", "ADD", "SUB", "MUL", "JMP", "IF", "EQ", "LT", "GT", "LTEQ", "GTEQ", "PRINT", "READ" };

label* label_table = (label*)malloc(sizeof(label_table)*LABEL_SIZE);
int label_table_len = 0;

char registers[register_size];
char* memory = (char*)malloc(sizeof(char) *MEMORY_SIZE);

symbol_table* symbols = (symbol_table*)malloc(sizeof(symbol_table) * SYMBOLS_SIZE);
int symbol_table_len=0;

intercode* inter_code = (intercode*)malloc(sizeof(intercode) * INTER_CODE_SIZE);
int inter_code_len = 0;

bool is_register(char* name)
{	/*checks if the given name is register or not*/
	if (strlen(name) != 2)
		return false;
	if (name[0]<'A' || name[0]>'H')
		return false;
	return true;
}
bool is_label(char* name)
{
	/*returns if the given string is a label or not*/
	if (name == NULL)
		return false;
	if (name[0] > 'Z' || name[0] < 'A'||name[1]!=':')
		return false;
	return true;
}
int get_value_from_register(char* name)
{
	/*returns the register value AX=0 BX=1.....*/
	return registers[name[0] - 'A'];
}
int get_register_number(char* reg)
{
	/*return the registers id ie AX -0 BX -1.....*/
	return reg[0] - 'A';
}
int get_value_from_symbol_table(char name)
{
	/*returns the value of variable from its respective memory from using its address in symbol table*/
	for (int i = 0; i < symbol_table_len; i++)
	{
		if (symbols[i].name==name)
		{
			return symbols[i].address;
		}
	}
}
char* get_string(char* line, int* i)
{
	/*returns a new string from given line and point till it meets a whitespace character*/
	if (line[*i] == '\n' || line[*i] == '\0')
		return NULL;
	int j=*i;
	while (line[j] != '\0'&&line[j] != ' '&&line[j] != '\n')
		j++;
	char* str = (char*)malloc(sizeof(char)*(j - *i + 1));
	int x = *i;
	for (x=*i; line[x]!='\0'&&line[x]!=' '&&line[x]!='\n'; x++)
	{
		str[x-*i] = line[x];
	}
	str[x-*i] = '\0';
	*i = x+1;
	return str;
}
int get_size(char* str)
{
	/*for a given string containing array it returns the value present in the subscripts*/
	if (str[0] == '\0'||str[0]==','||str[0]!='[')
		return 1;
	else
	{
		int ans = 0;
		for (int i = 1; str[i] != ']'; i++)
		{
			ans *= 10;
			ans += str[i] - '0';
		}
		return ans;
	}
}

void push_to_table_as_data(char* line, int* i)
{
	/*for DATA B it pushes them in symbol table*/
	char* str = get_string(line, i);
	symbols[symbol_table_len].name = str[0];
	symbols[symbol_table_len].size=get_size(str + 1);
	if (symbol_table_len == 0)
		symbols[symbol_table_len].address = 0;
	else
		symbols[symbol_table_len].address = symbols[symbol_table_len - 1].address + abs(symbols[symbol_table_len - 1].size);
	symbol_table_len++;
}
void push_to_table_as_const(char* line, int* i)
{
	/*for a CONST B it will pushes it into symbol table*/
	symbols[symbol_table_len].name = line[*i];
	symbols[symbol_table_len].size = -1;
	if (symbol_table_len == 0)
		symbols[symbol_table_len].address = 0;
	else
		symbols[symbol_table_len].address = symbols[symbol_table_len - 1].address + abs(symbols[symbol_table_len - 1].size);
	char* num = get_string(line, i);
	num = get_string(line, i);
	num = get_string(line, i);
	int x=atoi(num);
	printf("%d\n", x);
	memory[symbols[symbol_table_len].address] = x;

	symbol_table_len++;
}
void push_to_label(char* name)
{
	label_table[label_table_len].block_name = name[0];
	label_table[label_table_len].address = inter_code_len;
	label_table_len++;
}
int get_symbol_address(char ch)
{
	/*returns the address of the symbol*/
	for (int i = 0; i < symbol_table_len; i++)
	{
		if (ch == symbols[i].name)
			return symbols[i].address;
	}
}
int get_opcode(char* name)
{
	/*returns the opcode of given input*/
	for (int i = 0; i < OPCODE_SIZE; i++)
	{
		if (strcmp(name, opcodes[i]) == 0)
			return i;
	}
}
int get_label_address(char* name)
{
	/*gets the label address which is corresponding to given string*/
	for (int i = 0; i < label_table_len; i++)
	{
		if (name[0] == label_table[i].block_name)
			return label_table[i].address;
	}
}
void parse_line(char* line)
{
	/*parsing the query and performing appropriate operation to intermediate code table*/
	int i = 0;
	while (true)
	{
		char* ans = get_string(line, &i);
		if (ans == NULL)
			return;
		if (strcmp(ans, "DATA") == 0)
		{
			push_to_table_as_data(line, &i);
		}
		else if (strcmp(ans, "CONST") == 0)
		{
			push_to_table_as_const(line, &i);			
		}
		else if (strcmp(ans, "READ")==0||strcmp(ans,"PRINT")==0)
		{
			char* temp = get_string(line, &i);
			int code = get_opcode(ans);
			//inter_code
			//inter_code_len
			inter_code[inter_code_len].instruction_no = inter_code_len + 1;
			inter_code[inter_code_len].no_of_parameters = 1;
			inter_code[inter_code_len].opcode = code;
			inter_code[inter_code_len].p = (int*)malloc(sizeof(int) * 1);
			inter_code[inter_code_len].p[0] = get_register_number(temp);
			inter_code_len++;
			
			//int value;
			//scanf("%d", &value);
			//registers[get_register_number(temp)] = value;
		}
		else if (strcmp(ans, "MOV") == 0)
		{
			char* dest = get_string(line, &i);
			char* source = get_string(line, &i);
			if (is_register(dest))
			{
				int code = 1;
				inter_code[inter_code_len].instruction_no = inter_code_len + 1;
				inter_code[inter_code_len].no_of_parameters = 2;
				inter_code[inter_code_len].opcode = code;
				inter_code[inter_code_len].p = (int*)malloc(sizeof(int) * 2);
				inter_code[inter_code_len].p[0] = get_symbol_address(dest[0]) + get_size(dest + 1) - 1;
				inter_code[inter_code_len].p[1] = get_register_number(source);

				inter_code_len++;

			}
			else
			{
				int code = 2;
				inter_code[inter_code_len].instruction_no = inter_code_len + 1;
				inter_code[inter_code_len].no_of_parameters = 2;
				inter_code[inter_code_len].opcode = code;
				inter_code[inter_code_len].p = (int*)malloc(sizeof(int) * 2);
				inter_code[inter_code_len].p[0] = get_register_number(dest);
				inter_code[inter_code_len].p[1] = get_symbol_address(source[0]) + get_size(source + 1) - 1;

				inter_code_len++;

			}
		}
		else if (strcmp(ans, "ADD") == 0||strcmp(ans,"MUL")==0||strcmp(ans,"SUB")==0)
		{
			char* a = get_string(line, &i);
			char* b = get_string(line, &i);
			char* c = get_string(line, &i);
			int code = get_opcode(ans);
			inter_code[inter_code_len].instruction_no = inter_code_len + 1;
			inter_code[inter_code_len].no_of_parameters = 3;
			inter_code[inter_code_len].opcode = code;
			inter_code[inter_code_len].p = (int*)malloc(sizeof(int) * 3);
			inter_code[inter_code_len].p[0] = get_register_number(a);
			inter_code[inter_code_len].p[1] = get_register_number(b);
			inter_code[inter_code_len].p[2] = get_register_number(c);

			inter_code_len++;
		}
		else if (is_label(ans))
		{
			push_to_label(ans);
		}
		else if (strcmp(ans, "IF") == 0)
		{
			char* a = get_string(line,&i);
			char* b = get_string(line,&i);
			char* c = get_string(line, &i);
			get_string(line, &i);

			int code = get_opcode(ans);
			inter_code[inter_code_len].instruction_no = inter_code_len + 1;
			inter_code[inter_code_len].no_of_parameters = 4;
			inter_code[inter_code_len].opcode = code;
			inter_code[inter_code_len].p = (int*)malloc(sizeof(int) * 4);
			inter_code[inter_code_len].p[0] = get_register_number(a);
			inter_code[inter_code_len].p[1] = get_register_number(c);
			inter_code[inter_code_len].p[2] = get_opcode(b);
			inter_code[inter_code_len].p[3] = -1;

			push(inter_code[inter_code_len].instruction_no);
			inter_code_len++;
		}
		else if (strcmp(ans, "JMP") == 0)
		{

			int code = get_opcode(ans);
			char* a = get_string(line, &i);
			inter_code[inter_code_len].instruction_no = inter_code_len + 1;
			inter_code[inter_code_len].no_of_parameters = 1;
			inter_code[inter_code_len].opcode = code;
			inter_code[inter_code_len].p = (int*)malloc(sizeof(int) * 1);
			inter_code[inter_code_len].p[0] = get_label_address(a);
			
			inter_code_len++;
		}
		else if (strcmp(ans, "ELSE") == 0)
		{
			int code = get_opcode("JMP");
			char* a = get_string(line, &i);
			inter_code[inter_code_len].instruction_no = inter_code_len + 1;
			inter_code[inter_code_len].no_of_parameters = 1;
			inter_code[inter_code_len].opcode = code;
			inter_code[inter_code_len].p = (int*)malloc(sizeof(int) * 1);
			inter_code[inter_code_len].p[0] = -1;
			int x = top() - 1; pop();

			push(inter_code[inter_code_len].instruction_no);
			inter_code_len++;
			inter_code[x].p[inter_code[x].no_of_parameters - 1] = inter_code_len;

		}
		else if (strcmp(ans, "ENDIF") == 0)
		{
			int code = get_opcode(ans);
			inter_code[inter_code_len].instruction_no = inter_code_len + 1;
			inter_code[inter_code_len].no_of_parameters = 0;
			inter_code[inter_code_len].opcode = code;
			int x = top() - 1; pop();

			inter_code_len++;
			inter_code[x].p[inter_code[x].no_of_parameters - 1] = inter_code_len;

		}
	}
	//printf("\n");
}
void trim_front(char* line)
{
	/*this function will trims out all white space
	 characters present to the front of string*/
	int first = 0;
	while (line[first] == ' '||line[first]=='\t')
	{
		first++;
	}
	int i;
	for (i = 0; line[first+i]!='\0'; i++)
		line[i] = line[first+i];
	line[i] = '\0';
}
void preprocess()
{
	/*allocates the initial values to memory 
	array and reads the assembly level code to 
	the console screen*/
	for (int i = 0; i < 500; i++)
		memory[i] = 0;
	FILE* fp = fopen("input.txt", "r");
	while (!feof(fp))
	{
		char line[200];
		fgets(line, 100, fp);
		trim_front(line);
		printf("%s", line);
		parse_line(line);
	}
	fclose(fp);
}
char get_char_from_console()
{
	/*
	  this method is used to read the integers as a 
	  string from the screen and converts them to into integer
	*/
	char input[100];
	scanf("%s", input);
	char ch = 0;
	for (int i = 0; input[i]; i++)
	{
		ch *= 10;
		ch += input[i] - '0';
	}
	return ch;
}
bool verifying(int a, int op, int b)
{
	/*this will justify the operation between a,b as a op b(example 5 < 4)*/
	a = registers[a], b = registers[b];
	if (op==8)
		return a == b;
	else if (op==9)
		return a < b;
	else if (op==10)
		return a > b;
	else if (op==11)
		return a <= b;
	else if (op==12)
		return a >= b;
}
void execute()
{
	/*this function executes the intermediate code*/
	for (int i = 0; i < inter_code_len; i++)
	{
		//printf("(i)=%d\n", i);
		int op_code = inter_code[i].opcode;
		if (op_code == 1)//move
		{
			memory[inter_code[i].p[0]] = registers[inter_code[i].p[1]];
		}
		else if (op_code == 2)//move
		{
			registers[inter_code[i].p[0]] = memory[inter_code[i].p[1]];
			
		}
		else if (op_code == 3)//addition
		{
			registers[inter_code[i].p[0]] = registers[inter_code[i].p[2]] + registers[inter_code[i].p[1]];
		}
		else if (op_code == 4)//subtraction
		{
			registers[inter_code[i].p[0]] = registers[inter_code[i].p[1]] - registers[inter_code[i].p[2]];
		}
		else if (op_code == 5)//multiplication
		{
			registers[inter_code[i].p[0]] = registers[inter_code[i].p[1]] * registers[inter_code[i].p[2]];
		}
		else if (op_code == 6)
		{
			i = inter_code[i].p[0]-1;
		}
		else if (op_code == 7)
		{
			if (verifying(inter_code[i].p[0], inter_code[i].p[2], inter_code[i].p[1]))
			{
				continue;
			}
			else
			{
				i = inter_code[i].p[3]-1;
				if (i == -1)
					break;
			}
		}
		else if (op_code == 13)
		{
			printf("%d\n", registers[inter_code[i].p[0]]);
		}
		else if (op_code == 14)//read
		{
			char ch = get_char_from_console();
			registers[inter_code[i].p[0]] = ch;
		}
	}
}
void show_intermediate_code()
{
	printf("\nintermediate code table:\n");
	for (int i = 0; i < inter_code_len; i++)
	{
		printf("%6d %6d %6d ", i, inter_code[i].instruction_no, inter_code[i].opcode);
		for (int j = 0; j < inter_code[i].no_of_parameters; j++)
		{
			printf("%6d ", inter_code[i].p[j]);
		}
		printf("\n");
	}
}
void show_symbol_table()
{
	printf("\nsymbol table:\n");
	printf("%6s %6s %6s\n", "name", "memory", "size");
	for (int i = 0; i <symbol_table_len; i++)
		printf("%5c %6d %6d\n", symbols[i].name, symbols[i].address, symbols[i].size);
}
void show_registers_data()
{
	printf("registers:\n");
	for (int i = 0; i < MAX_REGISTERS; i++)
		printf("id[%d]: %d\n", i, registers[i]);
}
void show_memory_segment()
{
	printf("memory:\n");
	for (int i = 0; i <= symbols[symbol_table_len - 1].address; i++)
		printf("%d\n", memory[i]);
}
void show_label_table()
{
	printf("label table:\n");
	for (int i = 0; i < label_table_len; i++)
	{
		printf("%c %d\n", label_table[i].block_name, label_table[i].address);
	}
}
void compile()
{
	preprocess();
	show_intermediate_code();
	show_symbol_table();
	show_memory_segment();
	show_label_table();
	system("pause");
	//system("cls");
	execute();//intermediate code execution point;
	system("pause");
}
void generate_c_file()
{
	show_intermediate_code();
}
int main()
{
	compile();
	return 0;
}