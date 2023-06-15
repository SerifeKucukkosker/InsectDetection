#include "arduinoFFT.h"
#define SAMPLES 128
#define SAMPLING_FREQUENCY 9000
arduinoFFT FFT = arduinoFFT();
unsigned int samplingPeriod;
unsigned long microSeconds; // sampling için milroişlemcinin o anlık değerini tutacak
double vReal[SAMPLES]; //sample mag için tutuluyor
double vImag[SAMPLES]; //sample mag için tutluyor
const int interruptPin = 2;  // Interrupt pini (Değiştirebilirsiniz)
volatile bool interruptFlag = true;
double amsmin=0.0,amsmax=500.0,acbmin=1700.0,acbmax=2900,cbmin=3500,cbmax=4600;
int Countams = 0, Countacb = 0, Countcb = 0;
double peakAMS = 332 , peakCB = 2070,peakACB=4235;
void setup() {
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), interruptHandler, RISING);
  samplingPeriod = round(1000000*(1.0/SAMPLING_FREQUENCY));
  Serial.begin(9600);
}

void loop() {
  if (interruptFlag) {
  fonksiyon();
  // Interrupt flag'i sıfırla
    interruptFlag = false;
   }
}
 void fonksiyon(){
  int AMS = 0 , CB = 0 , UD = 0 , ACB=0; 
  for(int i=0;i<30;i++){
    double peak=fft();
    if(peak < amsmax){
       AMS++;
     }
    else if (peak> acbmin && peak<acbmax){
      ACB++;
    }
    else if(peak>cbmin && peak<cbmax){
      CB++;
    }
    else{
          if(abs(peak-peakAMS) < abs(peak-peakCB) && abs(peak-peakAMS) < abs(peak-peakACB)){
           AMS++;
          }
          else if(abs(peak-peakAMS) > abs(peak-peakCB) && abs(peak-peakCB)<abs(peak - peakACB)) {
           CB++;
          }
          else if(peak > acbmax && peak <1.5*acbmax){
           ACB++;
          }                  
    }
  }
  if(AMS>CB && AMS>ACB){
    Serial.println("Mediterrenenan fruit fly has been detected.");
  }
  else if (AMS < CB && ACB<CB){
    Serial.println("Cricket has been detected.");
  }
  else if(ACB>AMS && ACB > CB){
    Serial.println("Tree cricket has been detected.");
  }
  Serial.println("------------------------------------------------------------");
  
 }
 double fft(){
  for(int i=0; i<SAMPLES; i++){
        microSeconds = micros();    // anlık kaç saniye geçtiğini tutyor
        vReal[i] = analogRead(0);   // gelen sinyalin analog büyüklük değeri 0-1023
        vImag[i] = 0; // imajiner büyüklükleri 0 yaptık
        while(micros() < (microSeconds + samplingPeriod))
       {
         // sampling periyodun bittiğine emin olmak için yapıldı
        }
    }
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD); // fft için hamming type window kullanıdk ileri yönde 
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);//sqrt(r^2+im^2)
    double peak = FFT.MajorPeak(vReal,SAMPLES, SAMPLING_FREQUENCY); // dominant frekans değerini döndürdü
    peak = peak * 92 / 100;
    Serial.println(peak);
    return peak;
 }


void interruptHandler() {
  interruptFlag = true;
}
