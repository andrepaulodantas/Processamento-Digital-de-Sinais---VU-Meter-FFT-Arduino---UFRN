#include <Fix_fft.h>
/*
*@ANDRE
*/

//Configuracoes de pinos
#define AUDIO_PIN   A0
#define SEG_1_PIN   5
#define SEG_2_PIN   7
#define SEG_3_PIN   6
#define SEG_4_PIN   8
#define SEG_5_PIN   9
#define SEG_6_PIN   10
#define SEG_7_PIN   11

int sensorValue = 0;   
int outputValue = 0; 

//Configuração da lib FFT 
#define SAMPLES  128 //tamanho da matriz da transformada
char im[SAMPLES]; //matriz imaginaria
char data[SAMPLES]; //amostras de audio

//Funcao para acender o VU
void setVU(int valor ){
 
  //Calcula as divisoes do valor (max / faixas)
  int div = 256 / 7;
  
  //TODO: achar uma forma mais elegante para escrever esse bloco condicional
  if( valor > 0 )
    digitalWrite( SEG_1_PIN, HIGH );
  else
    digitalWrite( SEG_1_PIN, LOW );
    
  if( valor > div )
    digitalWrite( SEG_2_PIN, HIGH );
  else
    digitalWrite( SEG_2_PIN, LOW );
  
  if( valor > (div*2) )
    digitalWrite( SEG_3_PIN, HIGH );
  else  
    digitalWrite( SEG_3_PIN, LOW );
    
  if( valor > (div*3) )  
    digitalWrite( SEG_4_PIN, HIGH );
  else  
  digitalWrite( SEG_4_PIN, LOW );
    
  if( valor > (div*4) )    
    digitalWrite( SEG_5_PIN, HIGH );
  else
    digitalWrite( SEG_5_PIN, LOW );
  
  if( valor > (div*5) )
    digitalWrite( SEG_6_PIN, HIGH );
  else  
    digitalWrite( SEG_6_PIN, LOW );
  
  if( valor > (div*6) )  
    digitalWrite( SEG_7_PIN, HIGH ); 
  else
    digitalWrite( SEG_7_PIN, LOW ); 
}

void setup() {
  
  Serial.begin( 9600 ); 

  pinMode( SEG_1_PIN, OUTPUT );
  pinMode( SEG_2_PIN, OUTPUT );
  pinMode( SEG_3_PIN, OUTPUT );
  pinMode( SEG_4_PIN, OUTPUT );
  pinMode( SEG_5_PIN, OUTPUT );
  pinMode( SEG_6_PIN, OUTPUT );
  pinMode( SEG_7_PIN, OUTPUT );

  //Esse lado acende e apaga o VU 2 vezes para testar
  for( int y=0; y < 2; y++) {
    int x = 1;
    for (int i = 0; i > -1; i = i + x){
      setVU( i );
      delay(1);
      if (i == 255) x = -1;
    } 
  }
  
  pinMode( AUDIO_PIN, INPUT );    
}

int static i = 0;
static long tt;
int amostra;

void loop() {
  
  //Bloco de captura do audio
  if (i < SAMPLES){
    amostra = analogRead( AUDIO_PIN ); 
    //converte a amostra para 8 bits e remove o "dc bias"
    data[i] = (amostra / 4) - 128; 
    im[i] = 0;
    i++;   
    
  } else {

     //Se ja capturou todas as amostrar, executa a funcao FFT 
    fix_fft(data,im, 7 ,0);
        
    for (i=0; i< SAMPLES;i++){
       //Extrai somente os valores absolutos de cada faixa
       //calculando a hipotenusa do triangulo formado pela amostra
       //e o valor da transformada de cada faixa
       data[i] = sqrt(data[i] * data[i] + im[i] * im[i]); 
    }
    
    //Usei o valor da segunda banda e amplifico o valor multiplicando 
    //por uma constante (16). Essa constante pode vir de um potenciometro
    //preferi colocar fixo
    //TODO: realizar a escala dinamicamente calculando 
    //o maximo e o minimo
    outputValue = data[1] * 16;
    i = 0;

    //atualiza o VU
    setVU( outputValue );
  }
 
}
