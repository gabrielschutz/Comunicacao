#define LED 9

String Texto; //Mensagem a ser enviada
char binTexto[256]; //256 Bits tamanho maximo da palavra
char binCaracter[8]; //Vetor de bits para cada letra
int start = 0; //Controle do transmissor
long divisor = 65521; //Divisor, numero perto do numero maximo que cabe dentro dos 16 bits
char binDivisor[17]; //Numero binario do divisor
char binFinal[272]; //Array de envio final com CRC! (256 bits da mensagem e 16 do CRC)

int TaxaTransferencia = 4; //Unidade em bits por segundo

int NRZ = 0; //1 Para l e 0 Para i

char exor(char a,char b){ //Funcao auxiliar para calculo do CRC
  if(a==b)
    return '0';
  else
    return '1';
}

void crc(char data[], char key[]){ //Funcao para calculo do CRC e doa array final a ser enviado
  
    int datalen = strlen(data);
    int keylen = strlen(key);

    for (int i = 0; i < keylen - 1; i++) // Adiciona 0 para toda a data restante
        data[datalen + i] = '0';
    data[datalen + keylen - 1] = '\0';

    Serial.println("Data a ser dividida pelo divisor: ");
    Serial.println(data);

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

    
    strcpy(binFinal, data);
    
    Serial.println("CRC"); //Printa CRC
    Serial.println(rem); 
    Serial.println("Data final a ser enviada");
    Serial.println(data);

}



void setup() {
  
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  Serial.begin(9600);
  Serial.setTimeout(5000);

  Serial.print("Digite a mensagem a ser enviada: "); 
  Texto = Serial.readString();
  Texto.remove(Texto.length()-1,1);
  Serial.println(Texto);


  Serial.print("Velocidade de transmissao 'Bits por Segundo (B/s)': ");
  Serial.println(TaxaTransferencia);
  Serial.print("Delay entre Bits: ");
  TaxaTransferencia = 1000/TaxaTransferencia;
  Serial.println(TaxaTransferencia);

  char Caracter;

  delay(1000);

  for (int i = 0; i < Texto.length(); i++) { //For para rodar toda a mensagem
    Caracter = Texto.charAt(i); //Pega o caracter
    itoa(Caracter, binCaracter, 2); //Transforma em binario (A funcao itoa nao transforma a character direto nos 8 bits, ela pula o 1 bit da ASCII)
    binTexto[strlen(binTexto)] = '0'; //Adiciona um 0 antes devido ao problema citado acima
    if (Caracter == ' '){ //Caso seja espaco adiciona mais um 0
      binTexto[strlen(binTexto)] = '0';
    }
    strcat(binTexto, binCaracter); //Adiciona o array binario do character dentro da mensagem em binario
  }

  Serial.print("Mensagem a enviar: '"); //Mensagem
  Serial.print(Texto);
  Serial.println("'");

  
  Serial.print("Binario da mensagem = '"); //Binario da Mensagem
  Serial.print(binTexto);
  Serial.println("'");

  
  itoa(divisor, binDivisor, 2); //Converte o numero divisor em binario
  Serial.print("Numero Divisor = "); //Numero divisor
  Serial.print(divisor);
  Serial.print(" == Divisor em Binario = "); //Numero divisor
  Serial.println(binDivisor);


  crc(binTexto,binDivisor);

  if (NRZ == 0){
      for (int i = 0; i < (strlen(binFinal))+1; i++){
        if(binFinal[i]== '1')  {
          binFinal[i] = '0';
        }else if (binFinal[i]== '0')  {
          binFinal[i] = '1';
        }
      }
   }

}


void loop() {

  if (start == 0){ //Vai rodar so uma vez
    
    Serial.print("Ligue o receptor em ate 10 segundos! ");
    for (int k = 1; k<11;k++){
      Serial.print(k);
      Serial.print(" ");
      delay (1000);
    }
    start = 1;
  }

  if (start = 1){

    int contador1 = Texto.length() * 8;
    
    digitalWrite(LED, HIGH); //Aviso de comeco de envio
    Serial.println("Em 5 segundos comecara a ser enviada a mensagem");
    delay(4000);
    digitalWrite(LED, LOW);
    delay(1000);
    
    Serial.println("============================== COMECANDO ENVIO =================================");

    for (int i = 0; i < contador1; i++) { //Vai rodar ate os 272 bits

        Serial.print(binFinal[i]);
        Serial.print(" ");

        if (binFinal[i] == '1'){
          digitalWrite(LED, HIGH);
        }else{
          digitalWrite(LED, LOW);
        }
     


      

      delay(TaxaTransferencia);
      
    }

    analogWrite(LED,3); //Aviso de fim de envio
    Serial.println("Mensagem enviada!");
    Serial.println("Em 5 segundos comecara a ser enviado o CRC");
    
    delay(5000);
    
    analogWrite(LED,0);

    for (int i = contador1; i < contador1+17; i++) { //Vai rodar ate enviar todo o crc
      
      Serial.print(binFinal[i]);
      Serial.print(" ");

      if (binFinal[i] == '1'){
        digitalWrite(LED, HIGH);
      }else{
        digitalWrite(LED, LOW);
      }



      delay(TaxaTransferencia);
      
    }
    
    analogWrite(LED,3); //Aviso de fim de envio
    Serial.println("CRC Enviado!");
    Serial.println("Todos os dados enviados!");

    delay(3000);

    exit(1);
    
 }
            
}
