// Sarah Chin
// sa820429
// COP 3402, Fall 2018

#include <stdio.h>
#include "vm.h"
#include "data.h"

void initVM(VirtualMachine*);
int readInstructions(FILE*, Instruction*);
void dumpInstructions(FILE*, Instruction*, int numOfIns);
int getBasePointer(int *stack, int currentBP, int L);
void dumpStack(FILE*, int* stack, int sp, int bp);
int executeInstruction(VirtualMachine* vm, Instruction ins, FILE* vmIn, FILE* vmOut);

const char *opcodes[] = 
{
    "illegal", // opcode 0 is illegal
    "lit", "rtn", "lod", "sto", "cal", // 1, 2, 3 ..
    "inc", "jmp", "jpc", "sio", "sio",
    "sio", "neg", "add", "sub", "mul",
    "div", "odd", "mod", "eql", "neq",
    "lss", "leq", "gtr", "geq"
};

enum { CONT, HALT };

void initVM(VirtualMachine* vm)
{
	int i;
	vm = malloc(sizeof(VirtualMachine));

	vm->BP = 1;
	vm->SP = 0;
	vm->PC = 0;
	vm->IR = 0;
	
	for (i = 0; i < REGISTER_FILE_REG_COUNT; i++)
		vm->RF[i] = 0;
	
	for (i = 0; i < MAX_STACK_HEIGHT; i++)
		vm->stack[i] = 0;
	
    if(vm == NULL)
    {
        printf("Error! Not enough memory. Could not create VM.\n");
		exit();
    }
}

int readInstructions(FILE* in, Instruction* ins)
{
    // Instruction index
    int i = 0;
    
    while(fscanf(in, "%d %d %d %d", &ins[i].op, &ins[i].r, &ins[i].l, &ins[i].m) != EOF)
    {
        i++;
    }

    // Return the number of instructions read
    return i;
}

void dumpInstructions(FILE* out, Instruction* ins, int numOfIns)
{
    // Header
    fprintf(out,
        "***Code Memory***\n%3s %3s %3s %3s %3s \n",
        "#", "OP", "R", "L", "M"
        );

    // Instructions
    int i;
    for(i = 0; i < numOfIns; i++)
    {
        fprintf(
            out,
            "%3d %3s %3d %3d %3d \n", // formatting
            i, opcodes[ins[i].op], ins[i].r, ins[i].l, ins[i].m
        );
    }
}

int getBasePointer(int *stack, int currentBP, int L)
{
    // TODO
}

// Function that dumps the whole stack into output file
// Do not forget to use '|' character between stack frames
void dumpStack(FILE* out, int* stack, int sp, int bp)
{
    if(bp == 0)
        return;

    // bottom-most level, where a single zero value lies
    if(bp == 1)
    {
        fprintf(out, "%3d ", 0);
    }

    // former levels - if exists
    if(bp != 1)
    {
        dumpStack(out, stack, bp - 1, stack[bp + 2]);            
    }

    // top level: current activation record
    if(bp <= sp)
    {
        // indicate a new activation record
        fprintf(out, "| ");

        // print the activation record
        int i;
        for(i = bp; i <= sp; i++)
        {
            fprintf(out, "%3d ", stack[i]);
        }
    }
}

int base(l, base)
{
	int b1;
	
	b1 = base;
	
	while (1 > 0)
	{
		b1 = stack[b1 + 1];
		l--;
	}
	
	return b1;
}

int executeInstruction(VirtualMachine* vm, Instruction ins, FILE* vmIn, FILE* vmOut)
{
    switch(ins.op)
    {
        case 1:
			R[ins.r] = ins.m;
			break;
		case 2:
			vm->SP = BP-1;
			BP = vm->stack[vm->SP+3];
			PC = vm->stack[vm->SP+4];
			break;
		case 3:
		
        default:
            fprintf(stderr, "Illegal instruction?");
            return HALT;
    }

    return CONT;
}

void simulateVM(FILE* inp, FILE* outp, FILE* vm_inp, FILE* vm_outp)
{
	int numOfIns = 0;
	Instruction *ins = NULL;
	ins = malloc(sizeof(Instruction) * MAX_CODE_LENGTH);
	
	// Read instructions from file
	numOfIns = readInstructions(inp, ins);

    // Dump instructions to the output file
    dumpInstructions(outp, ins, numOfIns);

    // Before starting the code execution on the virtual machine,
    // .. write the header for the simulation part (***Execution***)
    fprintf(outp, "\n***Execution***\n");
    fprintf
	(
        outp,
        "%3s %3s %3s %3s %3s %3s %3s %3s %3s \n",         // formatting
        "#", "OP", "R", "L", "M", "PC", "BP", "SP", "STK" // titles
    );

    // Create a virtual machine
    VirtualMachine* vm = NULL;

    // Initialize the virtual machine
    initVM(vm);

    // Fetch&Execute the instructions on the virtual machine until halting
    while(flag == CONT)
    {
		flag = executeInstruction(vm, ins, vmIn, vmOut);
        fprintf(outp, "\n");
    }

    fprintf(outp, "HLT\n");
    return;
}
