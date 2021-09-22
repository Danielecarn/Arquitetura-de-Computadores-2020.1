#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

using namespace std;

//definiÃ§es de tipos
typedef unsigned char byte;
typedef unsigned int  word;
typedef unsigned long microcode;

//estrutura para guardar uma microinstruÃ§ao decodificada
struct decoded_microcode
{
    word nadd;
    byte jam;
    byte sft;
    byte alu;
    word reg_w;
    byte mem;
    byte reg_r;
};

//FunÃ§Ãµes utilitÃ¡rias ======================
void write_microcode(microcode w) //Dado uma microinstrucao, exibe na tela devidamente espaÃ§ado pelas suas partes.
{
   unsigned int v[36];
   for(int i = 35; i >= 0; i--)
   {
       v[i] = (w & 1);
       w = w >> 1;
   }

   for(int i = 0; i < 36; i++)
   {
       cout << v[i];
       if(i == 8 || i == 11 || i == 13 || i == 19 || i == 28 || i == 31) cout << " ";
   }
}

void write_word(word w) //Dada uma palavra (valor de 32 bits / 4 bytes), exibe o valor binÃ¡rio correspondente.
{
   unsigned int v[32];
   for(int i = 31; i >= 0; i--)
   {
       v[i] = (w & 1);
       w = w >> 1;
   }

   for(int i = 0; i < 32; i++)
       cout << v[i];
}

void write_byte(byte b) //Dado um byte (valor de 8 bits), exibe o valor binÃ¡rio correspondente na tela.
{
   unsigned int v[8];
   for(int i = 7; i >= 0; i--)
   {
       v[i] = (b & 1);
       b = b >> 1;
   }

   for(int i = 0; i < 8; i++)
       cout << v[i];
}

void write_dec(word d) //Dada uma palavra (valor de 32 bits / 4 bytes), exibe o valor decimal correspondente.
{
   cout << (int)d << endl;
}
//=========================================

//sinalizador para desligar mÃ¡quina
bool halt = false;

//memoria principal
#define MEM_SIZE 0xFFFF+1 //0xFFFF + 0x1; // 64 KBytes = 64 x 1024 Bytes = 65536 (0xFFFF+1) x 1 Byte;
byte memory[MEM_SIZE]; //0x0000 a 0xFFFF (0 a 65535)

//registradores
word mar=0, mdr=0, pc=0, sp=0, lv=0, cpp=0, tos=0, opc=0, h=0;
byte mbr=0;

//barramentos
word bus_a=0, bus_b=0, bus_c=0, alu_out=0;

//estado da ALU para salto condicional
byte n=0, z=1;

//registradores de microprograma
word mpc;

//memÃ³ria de microprograma: 512 x 64 bits = 512 x 8 bytes = 4096 bytes = 4 KBytes.
//Cada microinstruÃ§Ã£o Ã© armazenada em 8 bytes (64 bits), mas apenas os 4,5 bytes (36 bits) de ordem mais baixa sÃ£o de fato decodificados.
//Os 28 bits restantes em cada posiÃ§Ã£o da memÃ³ria sÃ£o ignorados, mas podem ser utilizados para futuras melhorias nas microinstruÃ§Ãµes para controlar microarquiteturas mais complexas.
microcode microprog[512];

//carrega microprograma
//Escreve um microprograma de controle na memÃ³ria de controle (array microprog, declarado logo acima)
void load_microprog()
{
    //implementar!
    
    //aula 9
    //microprog[0] = 0b000000000100001101010000001000010001;
    //microprog[1] = 0b000000010000001101010000001000010001;
    //microprog[2] = 0b000000011000000101001000000000000010;
    //microprog[3] = 0b000000100000001101010000001000010001;
    //microprog[4] = 0b000000101000000101000100000000000010;
    //microprog[5] = 0b000000000000001111000000000100000010
    
    //aula 10
    //MAIN
    //microprog[0] = 0b000000000100001101010000001000010001; //PC <- PC + 1; fetch; GOTO MBR;

    //OPC = OPC + memory[end_word];
    //microprog[2] = 0b000000011000001101010000001000010001; //PC <- PC + 1; fetch;
    //microprog[3] = 0b000000100000000101000000000010100010; //MAR <- MBR; read;
    //microprog[4] = 0b000000101000000101001000000000000000; //H <- MDR;
    //microprog[5] = 0b000000000000001111000100000000001000; //OPC <- OPC + H; GOTO MAIN;

    //memory[end_word] = OPC;
    //microprog[6] = 0b000000111000001101010000001000010001; //PC <- PC + 1; fetch;
    //microprog[7] = 0b000001000000000101000000000010000010; //MAR <- MBR;
    //microprog[8] = 0b000000000000000101000000000101001000; //MDR <- OPC; write; GOTO MAIN;

    //goto endereco_comando_programa;
    //microprog[9] = 0b000001010000001101010000001000010001; //PC <- PC + 1; fetch;
    //microprog[10] = 0b000000000100000101000000001000010010; //PC <- MBR; fetch; GOTO MBR;

    //if OPC = 0 goto endereco_comando_programa else goto proxima_linha;
    //microprog[11] = 0b000001100001000101000100000000001000; //OPC <- OPC; IF ALU = 0 GOTO 268 (100001100) 								                ELSE GOTO 12 (000001100);
    //microprog[12] = 0b000000000000001101010000001000000001; //PC <- PC + 1; GOTO MAIN;
    //microprog[268] = 0b100001101000001101010000001000010001; //PC <- PC + 1; fetch;
    //microprog[269] = 0b000000000100000101000000001000010010; //PC <- MBR; fetch; GOTO MBR;

    //OPC = OPC - memory[end_word];
    //microprog[13] = 0b000001110000001101010000001000010001; //PC <- PC + 1; fetch;
    //microprog[14] = 0b000001111000000101000000000010100010; //MAR <- MBR; read;
    //microprog[15] = 0b000010000000000101001000000000000000; //H <- MDR;
    //microprog[16] = 0b000000000000001111110100000000001000; //OPC <- OPC - H; GOTO MAIN;
    //memory[1] = 0x73; //init (bytes 2 e 3 são descartados por conveniência de implementação)

    //memory[4] = 0x00000006; //(CPP inicia com o valor 0x0006 guardado na palavra 1 – bytes 4 a 7.)

    //word tmp = 0x00001001; //LV 

   // memcpy(&(memory[8]), &tmp, 4); //(LV inicia com o valor de tmp guardado na palavra 2 – bytes 8 a 11)

    //tmp = 0x00000400; //PC

   // memcpy(&(memory[12]), &tmp, 4); //(PC inicia com o valor de tmp guardado na palavra 3 – bytes 12 a 15)

   // tmp = 0x00001003; //SP
//SP (Stack Pointer) é o ponteiro para o topo da pilha. 
//A base da pilha é LV e ela já começa com algumas variáveis empilhadas (dependendo do programa).
//Cada variável gasta uma palavra de memória. Por isso a soma de LV com num_of_vars.

    //memcpy(&(memory[16]), &tmp, 4); //(SP inicia com o valor de tmp guardado na palavra 4 – bytes 16 a 19)

   // memory[1025] = 0x19;
   // memory[1026] = 0x15;
   // memory[1027] = 0x22;
    //memory[1029] = 0x19;
    //memory[1030] = 0x0C;
    //memory[1031] = 0x19;
   // memory[1032] = 0x03;
   // memory[1033] = 0x02;
   // memory[1035] = 0x01;
   // memory[1036] = 0x1C;
    //memory[1037] = 0x01;
   // memory[1038] = 0x1C;
   // memory[1039] = 0x00;
   // memory[1040] = 0x4B;
    //memory[1042] = 0x08;
   // memory[1043] = 0x1C;
    //memory[1044] = 0x01;
   // memory[1045] = 0x3C;
   // memory[1046] = 0xFF;
    //memory[1047] = 0xF2;
    //memory[1048] = 0x01;
    
FILE *microp = fopen("microprog.rom", "rb");
    if (microp != NULL){
        fread(microprog, sizeof(microcode), 512, microp);
        fclose(microp);
    }
    else{
        std::cout << "Cadê o microprograma? " << std::endl;
        exit(-1);
    }
}

//carrega programa na memÃ³ria principal para ser executado pelo emulador.
//programa escrito em linguagem de mÃ¡quina (binÃ¡rio) direto na memÃ³ria principal (array memory declarado mais acima).
void load_prog()
{
    //implementar!
    //Aula 9 (Somar 5 + 4)
    //memory[0] = 0b00000000;
    //memory[1] = 0b00000001;
    //memory[2] = 0b00000101;  //5
    //memory[3] = 0b00000100;  //4
    
    //aula 10
    //memory[1] = 0b00000010; //fetch do microprog[0]
    //memory[2] = 0b00001010; //fetch do microprog[2]
    //memory[3] = 0b00000010; //fetch do microprog[0]
    //memory[4] = 0b00001011; //fetch do microprog[2]
    //memory[5] = 0b00000110; //fetch do microprog[0]
    //memory[6] = 0b00001100; //fetch do microprog[6]
    //memory[40] = 0b00000101; //read do microprog[3]
    //memory[44] = 0b00000011; //read do microprog[3]

    FILE *f = fopen("progra.exe", "rb");
    word A;
    fread(&A, sizeof(word), 1, f); //os 4 bytes do cabeçalho
    fread(&memory[0], sizeof(byte), 20, f);
    word PC;
    memcpy(&PC, memory + 12, 4);
    fread(memory +PC +1, sizeof(byte), A-20, f);
    fclose;
 
}

//exibe estado da mÃ¡quina
void debug(bool clr = true)
{
    if(clr) system("clear");

    cout << "MicroinstruÃ§Ã£o: ";
    write_microcode(microprog[mpc]);

    cout << "\n\nMemÃ³ria principal: \nPilha: \n";
    for(int i = lv*4; i <= sp*4; i+=4)
    {
        write_byte(memory[i+3]);
        cout << " ";
        write_byte(memory[i+2]);
        cout << " ";
        write_byte(memory[i+1]);
        cout << " ";
        write_byte(memory[i]);
        cout << " : ";
        if(i < 10) cout << " ";
        cout << i << " | " << memory[i+3] << " " << memory[i+2] << " " << memory[i+1] << " " << memory[i];
        word w;
        memcpy(&w, &memory[i], 4);
        cout << " | " << i/4 << " : " << w << endl;
    }

    cout << "\n\nPC: \n";
    for(int i = (pc-1); i <= pc+20; i+=4)
    {
        write_byte(memory[i+3]);
        cout << " ";
        write_byte(memory[i+2]);
        cout << " ";
        write_byte(memory[i+1]);
        cout << " ";
        write_byte(memory[i]);
        cout << " : ";
        if(i < 10) cout << " ";
        cout << i << " | " << memory[i+3] << " " << memory[i+2] << " " << memory[i+1] << " " << memory[i];
        word w;
        memcpy(&w, &memory[i], 4);
        cout << " | " << i/4 << " : " << w << endl;
    }

    cout << "\nRegistradores - \nMAR: " << mar << " ("; write_word(mar);
    cout << ") \nMDR: " << mdr << " ("; write_word(mdr);
    cout << ") \nPC : " << pc << " ("; write_word(pc);
    cout << ") \nMBR: " << (int) mbr << " ("; write_byte(mbr);
    cout << ") \nSP : " << sp << " (";  write_word(sp);
    cout << ") \nLV : " << lv << " ("; write_word(lv);
    cout << ") \nCPP: " << cpp << " ("; write_word(cpp);
    cout << ") \nTOS: " << tos << " ("; write_word(tos);
    cout << ") \nOPC: " << opc << " ("; write_word(opc);
    cout << ") \nH  : " << h << " ("; write_word(h);
    cout << ")" << endl;
    write_byte(memory[48]);
    cout << memory[48] << endl;
    cout << mdr << endl;
}

decoded_microcode decode_microcode(microcode code) //Recebe uma microinstruÃ§Ã£o binÃ¡ria e separa suas partes preenchendo uma estrutura de microinstrucao decodificada, retornando-a.
{
    decoded_microcode dec;

    //implementar!
    dec.reg_r = code & 0b1111;
    code = code >> 4;
    dec.mem = code & 0b111;
    code = code >> 3;
    dec.reg_w = code & 0b111111111;
    code = code >> 9;
    dec.alu = code & 0b111111;
    code = code >> 6;
    dec.sft = code & 0b11;
    code = code >> 2;
    dec.jam = code & 0b111;
    code = code >> 3; 
    dec.nadd = code & 0b111111111;

    return dec;
}

//alu
//recebe uma operaÃ§Ã£o de alu binÃ¡ria representada em um byte (ignora-se os 2 bits de mais alta ordem, pois a operaÃ§Ã£o Ã© representada em 6 bits)
//e duas palavras (as duas entradas da alu), carregando no barramento alu_out o resultado da respectiva operaÃ§Ã£o aplicada Ã s duas palavras.
void alu(byte func, word a, word b)
{
    //implementar!
   
   if(func == 0b011000){
      alu_out = a;
    }
    if(func == 0b010100){
      alu_out = b;
    }
    if(func == 0b011010){
      alu_out = ~a;
    }
    if(func == 0b101100){
      alu_out = ~b;
    }
    if(func == 0b111100){
      alu_out = a + b;
    }
    if(func == 0b111101){
      alu_out = a+ b + 1;
    }
    if(func == 0b111001){
      alu_out = a + 1;
    }
    if(func == 0b110101){
      alu_out = b + 1;
    }
    if(func == 0b111111){
      alu_out = b - a;
    }
    if(func == 0b110110){
      alu_out = b - 1;
    }
    if(func == 0b111011){
      alu_out = -a;
    }
    if(func == 0b001100){
      alu_out = a & b;
    }
    if(func == 0b011100){
      alu_out = a | b;
    }
    if(func == 0b010000){
      alu_out = 0;
    }
    if(func == 0b110001){
      alu_out = 1;
    }
    if(func == 0b110010){
      alu_out = -1;
    } 
    
    if(alu_out == 0){
      n = 0;
      z = 1;
    }else{
      n = 1;
      z = 0;
    }
}



//Deslocamento. Recebe a instruÃ§Ã£o binÃ¡ria de deslocamento representada em um byte (ignora-se os 6 bits de mais alta ordem, pois o deslocador eh controlado por 2 bits apenas)
//e uma palavra (a entrada do deslocador) e coloca o resultado no barramento bus_c.
void shift(byte s, word w)
{
    //implementar!
    if (s & 0b10){ //SRA1
      w = w >> 1;
    }
    if (s & 0b01){ //SLL8
      w = w << 8;
    }
    bus_c = w;
}

//Leitura de registradores. Recebe o nÃºmero do registrador a ser lido (0 = mdr, 1 = pc, 2 = mbr, 3 = mbru, ..., 8 = opc) representado em um byte,
//carregando o barramento bus_b (entrada b da ALU) com o valor do respectivo registrador e o barramento bus_a (entrada A da ALU) com o valor do registrador h.
void read_registers(byte reg_end)
{
    //implementar!
    bus_a = h;
    if(reg_end == 0b0000){
      bus_b = mdr;
    }else if(reg_end == 0b0001){
      bus_b = pc;
    }else if(reg_end == 0b0010){
      bus_b = mbr;
    }else if(reg_end == 0b0011){
      if(bus_b > 256) {
        bus_b = bus_b | (0b111111111111111111111111 << 8);
      }
    }else if(reg_end == 0b0100){
      bus_b = sp;
    }else if(reg_end == 0b0101){
      bus_b = lv;
    }else if(reg_end == 0b0110){
      bus_b = cpp;
    }else if(reg_end == 0b0111){
      bus_b = tos;
    }else if(reg_end == 0b1000){
      bus_b =opc;
    }
}

//Escrita de registradores. Recebe uma palavra (valor de 32 bits) cujos 9 bits de ordem mais baixa indicam quais dos 9 registradores que
//podem ser escritos receberao o valor que estÃ¡ no barramento bus_c (saÃ­da do deslocador).
void write_register(word reg_end)
{
    //implementar!
    if (reg_end & 0b000000001){
      mar = bus_c;
    }
    if (reg_end & 0b000000010){
      mdr = bus_c;
    }
    if (reg_end & 0b000000100){
      pc = bus_c;
    }
    if (reg_end & 0b000001000){
      sp = bus_c;
    }
    if (reg_end & 0b000010000){
      lv = bus_c;
    }
    if (reg_end & 0b000100000){
      cpp = bus_c;
    }
    if (reg_end & 0b001000000){
      tos = bus_c;
    }
    if (reg_end & 0b010000000){
      opc = bus_c; 
    }
    if (reg_end & 0b100000000){
      h = bus_c;
    }
}

//Leitura e escrita de memÃ³ria. Recebe em um byte o comando de memÃ³ria codificado nos 3 bits de mais baixa ordem (fetch, read e write, podendo executar qualquer conjunto dessas trÃªs operaÃ§Ãµes ao
//mesmo tempo, sempre nessa ordem) e executa a respectiva operaÃ§Ã£o na memÃ³ria principal.
//fetch: lÃª um byte da memÃ³ria principal no endereÃ§o constando em PC para o registrador MBR. EndereÃ§amento por byte.
//write e read: escreve e lÃª uma PALAVRA na memÃ³ria principal (ou seja, 4 bytes em sequÃªncia) no endereÃ§o constando em MAR com valor no registrador MDR. Nesse caso, o endereÃ§amento Ã© dado em palavras.
//Mas, como a memoria principal eh um array de bytes, deve-se fazer a conversÃ£o do endereÃ§amento de palavra para byte (por exemplo, a palavra com endereÃ§o 4 corresponde aos bytes 16, 17, 18 e 19).
//Lembrando que esta Ã© uma mÃ¡quina "little endian", isto Ã©, os bits menos significativos sÃ£o os de posiÃ§Ãµes mais baixas.
//No exemplo dado, suponha os bytes:
//16 = 00110011
//17 = 11100011
//18 = 10101010
//19 = 01010101
//Tais bytes correspondem Ã  palavra 01010101101010101110001100110011
void mainmemory_io(byte control)
{
    //implementar!
    if (control & 0b001){
    	mbr = memory[pc];
    }
    if (control & 0b010){
    	word palavra;
    	palavra = (memory[(mar*4)+3]);
    	palavra = palavra << 8;
    	palavra = palavra + (memory[(mar*4)+2]);
    	palavra = palavra << 8;
    	palavra = palavra + (memory[(mar*4)+1]);
    	palavra = palavra << 8;
    	palavra = palavra + (memory[(mar*4)]);
    	mdr = palavra;
    }
    if (control & 0b100){
    	word palavramdr = mdr;
    	for(int i=0; i<4; i++){
    		memory[(mar*4)+i] = (palavramdr & 0xff);
    		palavramdr = palavramdr >> 8;
    	}
    }
}

//Define prÃ³xima microinstruÃ§Ã£o a ser executada. Recebe o endereÃ§o da prÃ³xima instruÃ§Ã£o a ser executada codificado em uma palavra (considera-se, portanto, apenas os 9 bits menos significativos)
//e um modificador (regra de salto) codificado em um byte (considera-se, portanto, apenas os 3 bits menos significativos, ou seja JAMZ (bit 0), JAMN (bit 1) e JMPC (bit 2)), construindo e
//retornando o endereÃ§o definitivo (codificado em uma word - desconsidera-se os 21 bits mais significativos (sÃ£o zerados)) da prÃ³xima microinstruÃ§Ã£o.
word next_address(word next, byte jam)
{
    //implementar!
    if(jam & 0b001){
       next = next | z << 8;
    }
    if(jam & 0b010){
    	next = next | n <<8;
    }
    if(jam & 0b100){
    	next = next | mbr;
    }

    return next;
}

int main(int argc, char* argv[])
{
    load_microprog(); //carrega microprograma de controle

    load_prog(); //carrega programa na memÃ³ria principal a ser executado pelo emulador. JÃ¡ que nÃ£o temos entrada e saÃ­da, jogamos o programa direto na memÃ³ria ao executar o emulador.

    decoded_microcode decmcode;

    //laÃ§o principal de execuÃ§Ã£o do emulador. Cada passo no laÃ§o corresponde a um pulso do clock.
    //o debug mostra o estado interno do processador, exibindo valores dos registradores e da memÃ³ria principal.
    //o getchar serve para controlar a execuÃ§Ã£o de cada pulso pelo clique de uma tecla no teclado, para podermos visualizar a execuÃ§Ã£o passo a passo.
    //Substitua os comentÃ¡rios pelos devidos comandos (na ordem dos comentÃ¡rios) para ter a implementaÃ§Ã£o do laÃ§o.
    while(!halt)
    {
        debug();

        //implementar! Pega uma microinstruÃ§Ã£o no armazenamento de controle no endereÃ§o determinado pelo registrador contador de microinstruÃ§Ã£o e decodifica (gera a estrutura de microinstruÃ§Ã£o, ou seja, separa suas partes). Cada parte Ã© devidamente passada como parÃ¢metro para as funÃ§Ãµes que vÃªm a seguir.
        //implementar! LÃª registradores
        //implementar! Executa alu
        //implementar! Executa deslocamento
        //implementar! Escreve registradores
        //implementar! Manipula memÃ³ria principal
        //implementar! Determina endereÃ§o da microinstruÃ§Ã£o (mpc) a ser executada no prÃ³ximo pulso de clock
        
        decmcode = decode_microcode(microprog[mpc]);
        read_registers(decmcode.reg_r);
        alu(decmcode.alu, h , bus_b);
        bus_c = alu_out;
        shift(decmcode.sft, alu_out);
        write_register(decmcode.reg_w);
        mainmemory_io(decmcode.mem);
        mpc =next_address(decmcode.nadd, decmcode.jam);

	    getchar();
    }

    debug(false);

    return 0;
}