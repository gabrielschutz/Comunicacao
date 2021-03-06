int start=0; //Variavel de controle
int valor; //Variavel leitura do photoresistor
char mensagem[256]; //Mensagem a ser recebida
char crc[16]; //Crc a ser recebido
char binFinal[272]; //Data final a ser recebida
long divisor = 65521; //Numero Divisor para verificar se teve erro //65521
char binDivisor[17]; //Numero binario do divisor
int k = 0; //Contador para o CRC

int TaxaTransferencia = 4;
int NRZ = 0; //1 Para l e 0 Para i 


//================ Conversor ============
char auxChar[9]; //Para converter para texto
char c;


char exor(char a,char b){ //Funcao auxiliar para calculo do CRC
  if(a==b)
    return '0';
  else
    return '1';
}

void checkcrc(char data[], char key[]){ //Funcao para calculo do CRC e doa array final a ser enviado
  
    int datalen = strlen(data);
    int keylen = strlen(key);

    for (int i = 0; i < keylen - 1; i++) // Adiciona 0 para toda a data restante
        data[datalen + i] = '0';
    data[datalen + keylen - 1] = '\0';

    int codelen = datalen + keylen - 1; //Tamanho da palavra

    char temp[17], rem[17]; //Variaveis para calculo do Remainder

    for (int i = 0; i < keylen; i++)
        rem[i] = data[i]; 

    for (int j = keylen; j <= codelen; j++)
    {
        for (int i = 0; i < keylen; i++)
            temp[i] = rem[i]; 

        if (rem[0] == '0')
        {
            for (int i = 0; i < keylen - 1; i++)
                rem[i] = temp[i + 1];
        }
        else
        {
            for (int i = 0; i < keylen - 1; i++)
                rem[i] = exor(temp[i + 1], key[i + 1]);
        }
        if (j != codelen)
            rem[keylen - 1] = data[j]; 
        else
            rem[keylen - 1] = '\0';
    }

    for (int i = 0; i < keylen - 1; i++)
        data[datalen + i] = rem[i];
    data[codelen] = '\0';


    Serial.println("============== CALCULO DE ERRO ==============");
    Serial.print("O resto da divisar pelo numero divisor foi de: ");
    Serial.print(rem);
    

}

void setup() {
  
  Serial.begin(9600);
  delay(1000);
  itoa(divisor, binDivisor, 2); //Converte o numero divisor em binario
  Serial.print("Numero Divisor = "); //Numero divisor
  Serial.print(divisor);
  Serial.print(" == Divisor em Binario = "); //Numero divisor
  Serial.println(binDivisor);

 
  Serial.print("Velocidade de transmissao 'Bits por Segundo (B/s)': ");
  Serial.println(TaxaTransferencia);
  Serial.print("Delay entre Bits: ");
  TaxaTransferencia = 1000/TaxaTransferencia;
  Serial.println(TaxaTransferencia);
  
  
}


void loop() {
  
  if (start = 0){
    Serial.println("Receptor Ligado!");
    Serial.println("Aguardando Luz!");
    delay(1000);
    start = 1;
  }
  
  Serial.print("Luminosidade: ");
  valor = analogRead(A1);
  Serial.println(valor);
  
  if (valor > 300){
    
    Serial.println("Em 5 segundos comecara o recebimento da mensagem");
   
    delay(5000);

    Serial.println("============================== COMECANDO Recebimento  =================================");
    
    for (int i = 0; i < 256; i++) {
      valor = analogRead(A1);

      if (valor > 400 && valor < 550){
        
        Serial.println("Mensagem Recebida!");
        Serial.println("Em 5 segundos comecara a ser recebido o CRC");
        
        delay(5000);

        for (int l = i; l < i+15; l++) {
          
          valor = analogRead(A1);

          if (valor > 400){
            crc[k] = '0';
            binFinal[l] = '1';
            if (NRZ == 0){
              crc[k] = '1';
              binFinal[l] = '0';    
            }   
          }else{
            crc[k] = '1';
            binFinal[l] = '0';
            if (NRZ == 0){
              crc[k] = '0';
              binFinal[l] = '1';    
            }
          }

          Serial.print(crc[k]);
          Serial.print(" ");

          k++;
      
          delay(TaxaTransferencia);
        }
        break;
      }

                
        if (valor > 400){
          mensagem[i] = '0';
          binFinal[i] = '1';
          if (NRZ == 0){
             mensagem[i] = '1';
             binFinal[i] = '0';    
          }
        }else{
          mensagem[i] = '1';
          binFinal[i] = '0';
          if (NRZ == 0){
             mensagem[i] = '0';
             binFinal[i] = '1';    
          }
        }
              

      Serial.print(mensagem[i]);
      Serial.print(" ");

      delay(TaxaTransferencia);
    
    }

    Serial.println(" ");
    Serial.print("Mensagem Recebida = '");
    Serial.print(mensagem);
    Serial.println("'");

    Serial.print("CRC Recebido = '");
    Serial.print(crc);
    Serial.println("'");

    
    Serial.print("Data final Recebida = '");
    Serial.print(binFinal);
    Serial.println("'");

    Serial.println(" ");
    Serial.print("Mensagem Final recebida: '");

    for(int i = 0; i < (strlen(binFinal))-16;i+=8){ 
      for(int p = 0; p < 8;p++){
        auxChar[p] = binFinal[i+p];
      }
      c = strtol(auxChar, 0, 2);
      
      Serial.print(c);
      
    }

    Serial.println("'");

    
    checkcrc(binFinal,binDivisor); //Verifica se a divisao da 0
   

    delay(3000);

    exit(1);
    
  } 

  
}
