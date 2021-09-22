import sys
import struct

instrucoes = {'nop': 0x01, 
              'iadd': 0x02, 
              'isub': 0x05, 
              'iand': 0x08, 
              'ior': 0x0B,
              'dup': 0x0E,
              'pop': 0x10,
              'swap': 0x13,
              'bipush': 0x19,
              'iload': 0x1C, 
              'istore': 0x22, 
              'wide': 0x28, 
              'ldc_w': 0x32, 
              'iinc': 0x36, 
              'goto': 0x3C, 
              'iflt': 0x43,
              'ifeq': 0x47, 
              'if_icmpeq': 0x4B,
              'invokevirtual': 0x55,
              'ireturn': 0x6B }

def sintaxe(programa):
  label = []
  resultado = True
  
  for linha in programa:
    if len(linha) > 1 and linha[0] not in instrucoes:
      label.append(linha[0])

  for linha in programa:

    if len(linha) == 2:
      if linha[0] == "bipush":
        if linha[1].isnumeric() == True:
          resultado = True
        else: return False
      elif linha[0] == "iadd" or linha[0] == "iand" or linha[0] == "ior" or linha[0] == "isub" or linha[0] == "nop":
        return False
      elif linha[0] == "if_icmpeq" or linha[0] == "goto":
        if linha[1] in label:
          resultado = True
        else: 
          return False
      elif linha[0] == "iload" or linha[0] == "istore":
        
        if linha[1].isnumeric() == False:
          resultado = True
        else: 
          return False
    if len(linha) > 2:
      
      if linha[1] == "bipush":
        if linha[2].isnumeric() == True:
          resultado = True
        else: 
          return False
      else: 
        resultado = True
  return resultado
  
def escrever(prog):

  final = bytes(prog)

  with open("arquivo.exe", 'wb') as binary_output:
    escritos = binary_output.write(final)
  
def assembler(prog):
  twoBytes = ['goto', 'if_icmpeq']

  esq = {} 
  direi = {} 
  tam_prog = 0 
  variaveisNum = [] 
  
  for linha in prog:
    if len(linha) > 1 and linha[0] not in instrucoes:
      esq[linha[0]] = 0
    if len(linha)> 1 and linha[0] in twoBytes:
      direi[linha[1]] = []

  for linha in prog: 
    if len(linha) == 2 and linha[0] not in esq and linha[1] not in direi:
      if not linha[1].isnumeric() and linha[1] not in variaveisNum: 
        variaveisNum.append(linha[1])
      tam_prog += len(linha)

    elif len(linha) == 2 and linha[1] in direi:
      direi[linha[1]].append(tam_prog + 1) 
      tam_prog += len(linha) + 1
    elif len(linha) == 2 and linha[0] in esq:
      esq[linha[0]] = tam_prog + 1 
      tam_prog += len(linha) - 1

    elif len(linha) == 3 and linha[0] in esq:
      esq[linha[0]] = tam_prog + 1 
      tam_prog += len(linha) - 1 

    elif linha[-1] not in esq and linha[-1] not in variaveisNum and linha[-1] not in instrucoes:
      variaveisNum.append(linha[-1]) 
    else:
      tam_prog += len(linha)

  variaveis = []
  for v in variaveisNum:
     if v.isnumeric() == False:
      variaveis.append(v)
      
  Q = []

  programa = bytearray() 

  Q = struct.pack('<I', 20 + tam_prog)

  for b in Q:
    programa.append(b)
    
  registradores = [
    0x7300, 
    0x0006, 
    0x1001, 
    0x0400, 
    0x1001 + len(variaveis) 
  ]
 
  for r in registradores:
    r = struct.pack('<I', r)
    for b in r:
      programa.append(b)

  cont = 1
  check = []
  for linha in prog:
    if len(linha) > 1 and linha[0] not in instrucoes:
      cont -= 1
    if linha[0] in twoBytes:
      if linha[1] not in check:
        check.append(linha[1])
        label = linha[1]
        for i in range(0, len(direi[linha[1]])):
          direi[label][i] = esq[label] - direi[label][i]
      cont += 1
    for i in linha:
      cont += 1

  for linha in prog:  
    for i in linha:
      if i in instrucoes:
        programa.append(int(instrucoes[i]))
      elif i.isnumeric() == True:
        programa.append(int(i))
      elif i in variaveis:
        programa.append(int(variaveis.index(i)))
    if len(linha) > 1:
      if linha[1] in direi:
        label = linha[1]

        valor = hex(direi[label][0] & (2**16-1)) 
        valorconvint = int(valor, 16) 
        convertido = struct.pack('>I', valorconvint) 
        for b in range(2, len(convertido)):
          programa.append(int(convertido[b]))
        del direi[label][0]
  escrever(programa)
        
def main():
  prog = []

  with open(sys.argv[1], 'r') as prog:
    prog = [linha.split() for linha in prog]
  print("programa: ", prog)
  if sintaxe(prog) == True:
    assembler(prog)
    
  else:
    print("Programa escrito errado")
  

main()
