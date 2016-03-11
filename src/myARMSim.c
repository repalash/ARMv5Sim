
/* 

The project is developed as part of Computer Architecture class
Project Name: Functional Simulator for subset of ARM Processor

Developer's Name: Palash Ranjan Bansal
Developer's Email id: palash14072@iiitd.ac.in
Date: 31 March 2015

*/


/* myARMSim.cpp
   Purpose of this file: implementation file for myARMSim
*/

#include "myARMSim.h"
#include <stdlib.h>
#include <stdio.h>


//Register file
static unsigned int R[16];
//flags
static int N,C,V,Z;
//memory
static unsigned char MEM[4000];

//intermediate datapath and control path signals

static unsigned int instruction_word;
static unsigned int operand1;
static unsigned int operand2;
static unsigned int destination;
static unsigned int opcode; 
static unsigned int f;

void run_armsim() {
  while(1) {
    fetch();
    decode();
    execute();
    mem();
    write_back();
    printf("\n");
  }
}

// it is used to set the reset values
//reset all registers and memory content to 0
void reset_proc() {
  int i;
  for(i=0;i<15;i++){
    R[i]=0;
  }
  for(i=0; i<4000; i++){
    MEM[i]=0;
  }
  R[13]=4000;
}

//load_program_memory reads the input memory, and populates the instruction 
// memory
void load_program_memory(char *file_name) {
  FILE *fp;
  unsigned int address, instruction;
  fp = fopen(file_name, "r");
  if(fp == NULL) {
    printf("Error opening input mem file\n");
    exit(1);
  }
  while(fscanf(fp, "%x %x", &address, &instruction) != EOF) {
    write_word(MEM, address, instruction);
  }
  fclose(fp);
}

//writes the data memory in "data_out.mem" file
void write_data_memory() {
  FILE *fp;
  unsigned int i;
  fp = fopen("data_out.mem", "w");
  if(fp == NULL) {
    printf("Error opening dataout.mem file for writing\n");
    return;
  }
  
  for(i=0; i < 4000; i = i+4){
    fprintf(fp, "%x %x\n", i, read_word(MEM, i));
  }
  fclose(fp);
}

//should be called when instruction is swi_exit
void swi_exit() {
  write_data_memory();
  exit(0);
}


//reads from the instruction memory and updates the instruction register
void fetch() {
  instruction_word = read_word(MEM, R[15]);
  printf("FETCH: Fetch instruction 0x%x from address 0x%x\n", instruction_word, R[15]);
  R[15]+=4;
  //FETCH:Fetch instruction 0xE3A0200A from address 0x0
}
//reads the instruction register, reads operand1, operand2 fromo register file, decides the operation to be performed in execute stage
void decode() {
  if(instruction_word==0xEF000011)swi_exit();
  f=(instruction_word&0x0C000000)>>26;
  unsigned int Rn, Rd, i, op2;
  if(f==0){
    printf("DECODE: Opration is ");
    opcode = (instruction_word&0x01E00000)>>21;
    if(opcode==0){ //AND
      printf("AND");
    }else if(opcode==4){ //ADD
      printf("ADD");
    }else if(opcode==2){ //SUB
      printf("SUB");
    }else if(opcode==10){ //CMP
      printf("CMP");
    }else if(opcode==12){ //ORR
      printf("ORR");
    }else if(opcode==13){ //MOV
      printf("MOV");
    }else if(opcode==15){ //MNV
      printf("MNV");
    }
    Rn = (instruction_word&0x000F0000)>>16;
    Rd = (instruction_word&0x0000F000)>>12;
    if(Rn)printf(", first operand R%d", Rn);
    operand1 = R[Rn];
    i = (instruction_word&0x02000000)>>25;
    if(i==0){ //Register
      op2 = (instruction_word&0xF);
      if(!Rn) printf(", operand R%d", op2);
      else printf(", Second operand R%d", op2);
      operand2 = R[op2];
    }else{ //Immediate
      op2 = (instruction_word&0xFF);
      if(!Rn) printf(", operand #%d", op2);
      else printf(", Second operand #%d", op2);
      operand2 = op2;
    }
    if(Rd)printf(", destination register R%d\n", Rd);
    destination = Rd;
    if(Rn||!i){
      printf("DECODE: Read registers ");
      if(Rn)printf("R%d = %d", Rn, R[Rn]);
      if(Rn&&!i) printf(", ");
      if(!i)printf("R%d = %d", op2, R[op2]);
      printf("\n");
    }
  }else if(f==1){ //LDR/STR
    printf("DECODE: Opration is ");
    opcode = (instruction_word&0x03F00000)>>20;
    if(opcode==25){
      printf("LDR");
    }else if(opcode==24){
      printf("STR");
    }
    Rn = (instruction_word&0x000F0000)>>16;
    Rd = (instruction_word&0x0000F000)>>12;
    if(Rn)printf(", base address at R%d", Rn);
    if(Rd)printf(", destination register R%d\n", Rd);
    destination = Rd;
    operand1 = Rn;
    printf("DECODE: Read Register R%d = %x", Rn, R[Rn]);
    if(opcode==24) printf(", R%d = %d", Rd, R[Rd]);
    printf("\n");
  }else if(f==2){ //Branch
    opcode = (instruction_word&0xF0000000)>>28;
    operand2 = (instruction_word&0xFFFFFF);
    printf("DECODE: Operation is Branch ");
    if(opcode==0){ //EQ
      printf("Equal, ");
    }else if(opcode==1){ //NE
      printf("Not Equal, ");
    }else if(opcode==11){ //LT
      printf("Less Than, ");
    }else if(opcode==13){ //LE
      printf("Less Than or Equal, ");
    }else if(opcode==12){ //GT
      printf("Greater Than, ");
    }else if(opcode==10){ //GE
      printf("Greater Than or Equal, ");
    }else if(opcode==14){ //AL
      printf("Always, ");
    }
    printf("with offset 0x%x\n", operand2);
  }
}
//executes the ALU operation based on ALUop
void execute() {
  printf("EXECUTE: ");
  if(f==0){
    if(opcode==0){ //AND
      printf("AND %d and %d\n", operand1, operand2);
      operand1 = operand1&operand2;
    }else if(opcode==4){ //ADD
      printf("ADD %d and %d\n", operand1, operand2);
      operand1 = operand1+operand2;
    }else if(opcode==2){ //SUB
      printf("SUB %d and %d\n", operand1, operand2);
      operand1 = operand1-operand2;
    }else if(opcode==10){ //CMP
      printf("CMP %d and %d\n", operand1, operand2);
      Z=0; N=0;
      if(operand1-operand2==0){
        Z=1;
        printf("EXECUTE: Z updated to 1\n");
      }
      if(operand1-operand2<0){
        N=1;
        printf("EXECUTE: N updated to 1\n");
      }
    }else if(opcode==12){ //ORR
      printf("OR %d and %d\n", operand1, operand2);
      operand1 = operand1|operand2;
    }else if(opcode==13){ //MOV
      printf("No execute operation\n");
      operand1 = operand2;
    }else if(opcode==15){ //MNV
      printf("MNV %d\n", operand2);
      operand1 = ~operand2;
    }
  }else if(f==1){
    printf("No execute operation\n");
  }else if(f==2){
    operand1 = 0;
    if(opcode==0){ //EQ
      if(Z)operand1 = 1;
    }else if(opcode==1){ //NE
      if(!Z)operand1 = 1;
    }else if(opcode==11){ //LT
      if(N)operand1 = 1;
    }else if(opcode==13){ //LE
      if(N||Z)operand1 = 1;
    }else if(opcode==12){ //GT
      if(!N)operand1 = 1;
    }else if(opcode==10){ //GE
      if(!N&&Z)operand1 = 1;
    }else if(opcode==14){ //AL
      operand1 = 1;
    }
    if(operand1==1){
      unsigned int s = (operand2&0x800000)<<1, j;
      for(j=0;j<8; j++, s<<=1) operand2+=s;
      operand2<<=2;
      //R[14] = R[15];
      R[15]+=(signed int)operand2;
      printf("Updating PC to 0x%x\n", R[15]);
    }else{
      printf("No execute operation\n");
    }
  }
}
//perform the memory operation
void mem() {
  printf("MEMORY: ");
  if(f==0){
    printf("No memory operation\n");
  }
  else if(f==1){
    if(opcode==25){ //LDR
      unsigned int data = read_word(MEM, R[operand1]);
      printf("Read %d from address %x\n", data, R[operand1]);
      operand1 = data;
    }else if(opcode==24){ //STR
      write_word(MEM, R[operand1], R[destination]);
      printf("Write %d to address %x\n", R[destination], R[operand1]);
    }
  }
  else if(f==2){
    printf("No memory operation\n");
  }
}
//writes the results back to register file
void write_back() {
  printf("WRITEBACK: ");
  if(f==0){
    if(destination!=0){
      R[destination] = operand1;
      printf("write %d to R%d\n", operand1, destination);
    }else
      printf("no writeback operation\n");
  }else if(f==1){
    if(opcode==25){
      R[destination] = operand1;
      printf("write %d to R%d\n", operand1, destination);
    }else
      printf("no writeback operation\n");
  }else if(f==2){
    printf("No writeback operation\n");
  }
}


int read_word(char *mem, unsigned int address) {
  int *data;
  data =  (int*) (mem + address);
  return *data;
}

void write_word(char *mem, unsigned int address, unsigned int data) {
  int *data_p;
  data_p = (int*) (mem + address);
  *data_p = data;
}
