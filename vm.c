// Sarah Chin
// sa820429
// COP 3402, Fall 2018

#include <stdio.h>
#include <stdlib.h>
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
		return;
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
	int base;
	
	base = currentBP;
	
	while (L > 0)
	{
		base = stack[base + 1];
		L--;
	}
	
	return base;
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

int executeInstruction(VirtualMachine* vm, Instruction ins, FILE* vmIn, FILE* vmOut)
{
	int level;
	
    switch(ins.op)
    {
        case 1:
			vm->RF[ins.r] = ins.m;
			break;
		case 2:
			vm->SP = vm->BP-1;
			vm->BP = vm->stack[vm->SP+3];
			vm->PC = vm->stack[vm->SP+4];
			break;
		case 3:
			level = getBasePointer(vm->stack, vm->BP, ins.l);
			level += ins.m;
			vm->RF[ins.r] = vm->stack[level];
			break;
		case 4: 
			level = getBasePointer(vm->stack, vm->BP, ins.l);
			level += ins.m;
			vm->stack[level] = vm->RF[ins.r];
			break;
		case 5: // CAL
			vm->stack[vm->SP + 1] = 0;
			vm->stack[vm->SP + 2] = getBasePointer(vm->stack, vm->BP, ins.l);
			vm->stack[vm->SP + 3] = vm->BP;
			vm->stack[vm->SP + 4] = vm->PC;
			vm->BP = vm->SP + 1;
			vm->PC = ins.m;
			break;
		case 6:
			vm->SP = vm->SP + ins.m;
			break;
		case 7: // JMP
			vm->PC = ins.m;
			break;
		case 8: // JPC
			if (vm->RF[ins.r] == 0)
				vm->PC = ins.m;
			break;
		case 9:
			fprintf(vmOut, "%d", vm->RF[ins.r]);
			break;
		case 10:
			fscanf(vmIn, "%d", &(vm->RF[ins.r]));
			break;
		case 11: 
			return HALT;
		case 12:
			vm->RF[ins.r] = vm->RF[ins.l];
			break;
		case 13:
			vm->RF[ins.r] = vm->RF[ins.l] + vm->RF[ins.m];
			break;
		case 14:
			vm->RF[ins.r] = vm->RF[ins.l] - vm->RF[ins.m];
			break;
		case 15:
			vm->RF[ins.r] = vm->RF[ins.l] * vm->RF[ins.m];
			break;
		case 16:
			vm->RF[ins.r] = vm->RF[ins.l] / vm->RF[ins.m];
			break;
		case 17:
			vm->RF[ins.r] = vm->RF[ins.r]%2;
			break;
		case 18:
			vm->RF[ins.r] = vm->RF[ins.l] % vm->RF[ins.m];
			break;
		case 19:
			if (vm->RF[ins.l] == vm->RF[ins.m])
				vm->RF[ins.r] = 1;
			else
				vm->RF[ins.r] = 0;
			break;
		case 20:
			if (vm->RF[ins.l] == vm->RF[ins.m])
				vm->RF[ins.r] = 0;
			else
				vm->RF[ins.r] = 1;
			break;
		case 21:
			if (vm->RF[ins.l] < vm->RF[ins.m])
				vm->RF[ins.r] = 1;
			else
				vm->RF[ins.r] = 0;
			break;
		case 22:
			if (vm->RF[ins.l] <= vm->RF[ins.m])
				vm->RF[ins.r] = 1;
			else
				vm->RF[ins.r] = 0;
			break;
		case 23:
			if (vm->RF[ins.l] > vm->RF[ins.m])
				vm->RF[ins.r] = 1;
			else
				vm->RF[ins.r] = 0;
			break;
		case 24:
			if (vm->RF[ins.l] >= vm->RF[ins.m])
				vm->RF[ins.r] = 1;
			else
				vm->RF[ins.r] = 0;
			break;
        default:
            fprintf(stderr, "Illegal instruction?");
            return HALT;
    }

    return CONT;
}

void simulateVM(FILE* inp, FILE* outp, FILE* vm_inp, FILE* vm_outp)
{
	int i = 0, flag = CONT, numOfIns = 0;
	
	VirtualMachine* vm = NULL;
	Instruction *ins = NULL;
	
	// Create Instruction array. (The code store).
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
	vm = malloc(sizeof(VirtualMachine));

    // Initialize the virtual machine
    initVM(vm);

	// Fetch & Execute the instructions on the virtual machine until halting
    while(flag == CONT)
    {
		// Fetch
		i = vm->PC;
		
		// Advance PC - before execution!
		vm->PC += 1;
		
		// Execute the instruction
		flag = executeInstruction(vm, ins[i], vm_inp, vm_outp);
		
		// Print current state
		fprintf
		(
			outp,
            "%3d %3s %3d %3d %3d %3d %3d %3d ", i,
			opcodes[ins[i].op], ins[i].r, ins[i].l, ins[i].m,
             vm->PC, vm->BP, vm->SP
        );
		
		// Print stack info
		dumpStack(outp, vm->stack, vm->SP, vm->BP);

		fprintf(outp, "\n");
    }

    fprintf(outp, "HLT\n");
	
	free(ins);
	free(vm);
	
    return;
}
