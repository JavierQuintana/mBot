#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <MeMCore.h>

double angle_rad = PI/180.0;
double angle_deg = 180.0/PI;

void leerflechas();
byte pasos[50];
MeIR ir;
void leerAB();
byte n;
MeBuzzer buzzer;
void final();
MeRGBLed rgbled_7(7, 7==7?2:4);
void realizadopaso();
int mseg;                         //mseg= tiempo en milisegundos que están los motores encendidos para dar un paso
int mseggiro;                      // tiempo en milisegundos giro de 90 grados       
int msegtono;                     //mseg de los tonos
int quitarebotes;                 //mseg de espera cuando lee en el IR para que no lea más de uno
int espera;                       //mseg de espera cuando ha hecho un paso
void resetear();
double velocidadL;                 //velocidad= velocidad del motor LEFT 
double velocidadR;                 //velocidad= velocidad del motor RIGHT 


MBotDCMotor motor(0);
MeDCMotor motor_9(9);
MeDCMotor motor_10(10);
MeLEDMatrix ledMtx_1(1);   //// matriz por el puerto 1
unsigned char drawBuffer[16];
unsigned char *drawTemp;


//////////////////////////////////////////////
/// RESETEAR
// o al ppio del programa o si pulso B
//////////////////////////////////////////////
void resetear()
{
   //#######################################################################################
   //#####                                                                            ######
    //#####         velocidad= velocidad de los motores                                ######
    //#####         CAMBIAR ESTOS VALORES SI NO VAN IGUAL                                ######
    //#####                                                                            ######
    //#######################################################################################
    velocidadL=100;                 //velocidad= velocidad del motor LEFT 
    velocidadR=100;                 //velocidad= velocidad del motor RIGHT
     //#######################################################################################
   //#####                                                                            ######
    //#####         mseg= paso de 15 cm                                                ######
     //#####         mseggiro= giro de 90 grados                                        ######
    //#####         CAMBIAR ESTOS VALORES SI NO ESTÁ BIEN CALIBRADO                     ######
    //#####                                                                            ######
    //####################################################################################### 
     mseg = 897;                 //mseg= tiempo en milisegundos que están los motores encendidos para dar un paso hacia delante o hacia detrás = 15cm 
     mseggiro = 449;             //mseg= tiempo en milisegundos giro de 90 grados    
     //#######################################################################################
   //#######################################################################################
     msegtono = 300;              //mseg de los tonos 
     quitarebotes=500; 
     espera=800;
     buzzer.tone(262, 500);
     for(int i=0;i<n;i++)  pasos[i] = 0;    //resetea todo el array de pasos
     n = 0; 
    ledMtx_1.clearScreen();
    ledMtx_1.setColorIndex(1);
    ledMtx_1.setBrightness(6);   
}
void setup(){
    resetear();
    ir.begin();
    
}

void loop(){
    leerflechas();
    leerAB();
    ir.loop();    
}
////////////////////////////////////////////////////////////////////
////  LEER FLECHAS ////////////////////////////////////////////////
//  Lee las flechas del mando infrarrojos
//  Y guarda en el array pasos
//  nomnenclatura: (la misma que la instrucción motor.move
//  1=hacia delante
//  2=hacia atrás
//  3=hacia izquierda  
//  4=hacia derecha
//  n va de 1 hasta máximo 50
//  n=0 es que no hay nada grabado
///////////////////////////////////////////////////////////////////
void leerflechas()
{
    ledMtx_1.setColorIndex(1);
    ledMtx_1.setBrightness(6);
    if(ir.keyPressed(64)){
      pasos[n] = 1;
      n += 1;
      drawTemp = new unsigned char[16]{0,0,0,0,16,48,112,255,255,112,48,16,0,0,0,0};
      memcpy(drawBuffer,drawTemp,16);
      free(drawTemp);
      ledMtx_1.drawBitmap(0,0,16,drawBuffer);
      delay(quitarebotes);
      buzzer.tone(262, 500);
    }
    if(ir.keyPressed(25)){
      pasos[n] = 2;
      n += 1;
      drawTemp = new unsigned char[16]{0,0,0,0,8,12,14,255,255,14,12,8,0,0,0,0};
      memcpy(drawBuffer,drawTemp,16);
      free(drawTemp);
      ledMtx_1.drawBitmap(0,0,16,drawBuffer);
      delay(quitarebotes);
      buzzer.tone(262, 500);
    }
    if(ir.keyPressed(9)){ 
      pasos[n] = 3;
      n += 1;
      drawTemp = new unsigned char[16]{0,24,24,24,24,24,24,255,126,126,60,60,24,24,0,0};
      memcpy(drawBuffer,drawTemp,16);
      free(drawTemp);
      ledMtx_1.drawBitmap(0,0,16,drawBuffer);
      delay(quitarebotes);
      buzzer.tone(262, 500);
    }
    if(ir.keyPressed(7)){
      pasos[n] = 4;
      n += 1;
      drawTemp = new unsigned char[16]{0,0,24,24,60,60,126,126,255,24,24,24,24,24,24,0};
      memcpy(drawBuffer,drawTemp,16);
      free(drawTemp);
      ledMtx_1.drawBitmap(0,0,16,drawBuffer);
      delay(quitarebotes);
      buzzer.tone(262, 500);
    }
    /////////////////////// si n ha llegado al máximo que de un pitido de error
    if((n)==(50)){
            buzzer.tone(87, msegtono);
    }
    memcpy(drawBuffer,pasos,16);
    ledMtx_1.drawStr(0,7-0,String(n).c_str());   
}
//////////////////////////////////////////////////////
///   LEER A Y B
//
// nomenclatura
//
//  A (69) o botón setup (21) = que ejecute los pasos grabados
//  B (70) o botón O (22)    = que resetee
//
//
///////////////////////////////////////////////////////////
void leerAB()
{
     ///////////////////////LEER B /////////////////////////
     if((ir.keyPressed(70)) | (ir.keyPressed(22))){
        resetear();
    }
    ////////////////////////LEER A ///////////////////////////
   
    if((ir.keyPressed(69)) | (ir.keyPressed(21))){
        if((((n)==(0))  | ((n)==(50)))){
            buzzer.tone(87, msegtono);
        }else{
            int numpasos = n;
            for(int i=0;i<numpasos;i++)
            {
                if (pasos[i]==1){    ////  //  1=hacia delante
                      motor_9.run((9)==M1?-(velocidadL):(velocidadL));
                      motor_10.run((10)==M1?-(velocidadR):(velocidadR));
                }
                if (pasos[i]==2){   /////  //  2=hacia atrás
                      motor_9.run((9)==M1?-(-velocidadL):(-velocidadL));
                      motor_10.run((10)==M1?-(-velocidadR):(-velocidadR));
                }
                 if (pasos[i]==3){    //// //  3=hacia izquierda  
                      motor_9.run((9)==M1?-(-velocidadL):(-velocidadL));
                      motor_10.run((10)==M1?-(velocidadR):(velocidadR));
                }
                 if (pasos[i]==4){    /////  //  4=hacia derecha
                      motor_9.run((9)==M1?-(velocidadL):(velocidadL));
                      motor_10.run((10)==M1?-(-velocidadR):(-velocidadR));
                }
                
                realizadopaso(i);
            }
        }
        final();
    }
    
}
////////////////////////////////////////////////////////
//    REALIZADO PASO 
// unos tonos que ha terminado el paso leído
/////////////////////////////////////////////////////
void realizadopaso(int j)
{
    ledMtx_1.setColorIndex(1);
    ledMtx_1.setBrightness(6);
    
    if ((pasos[j])==(1)) drawTemp = new unsigned char[16]{0,0,0,0,16,48,112,255,255,112,48,16,0,0,0,0};      //  1=hacia delante
    if ((pasos[j])==(2)) drawTemp = new unsigned char[16]{0,0,0,0,8,12,14,255,255,14,12,8,0,0,0,0};          //  2=hacia atrás
    if ((pasos[j])==(3)) drawTemp = new unsigned char[16]{0,24,24,24,24,24,24,255,126,126,60,60,24,24,0,0};  //  3=hacia izquierda  
    if ((pasos[j])==(4)) drawTemp = new unsigned char[16]{0,0,24,24,60,60,126,126,255,24,24,24,24,24,24,0};  //  4=hacia derecha
    memcpy(drawBuffer,drawTemp,16);
    free(drawTemp);
    ledMtx_1.drawBitmap(0,0,16,drawBuffer);
      
    rgbled_7.setColor(0,0,255,0);
    rgbled_7.show();    
    if ((pasos[j])==(1)) delay(mseg);       //  1=hacia delante
    if ((pasos[j])==(2)) delay(mseg);        //  2=hacia atrás
    if ((pasos[j])==(3)) delay(mseggiro);   //  3=hacia izquierda  
    if ((pasos[j])==(4)) delay(mseggiro);    //  4=hacia derecha
 
    
    ledMtx_1.drawStr(0,7-0,String(j).c_str());
    buzzer.tone(262, msegtono);    
    rgbled_7.setColor(0,0,0,0);
    rgbled_7.show();
    motor.move(1,0);
    delay(espera);
    
}
////////////////////////////////////////////////////////
//    FINAL
// unos tonos que ha terminado el camino leído
/////////////////////////////////////////////////////
void final()
{
    drawTemp = new unsigned char[16]{0,0,0,0,108,10,10,10,10,108,0,0,0,0,0,0};
    memcpy(drawBuffer,drawTemp,16);
    free(drawTemp);
    ledMtx_1.drawBitmap(0,0,16,drawBuffer);
    
    buzzer.tone(392, msegtono);
    
    rgbled_7.setColor(0,0,255,0);
    rgbled_7.show();
    
    delay(1000*0.5);
    
    buzzer.tone(220, msegtono);
    
    rgbled_7.setColor(0,0,0,255);
    rgbled_7.show();
    
    delay(1000*0.5);
    
    buzzer.tone(392, msegtono);
    
    rgbled_7.setColor(0,0,255,0);
    rgbled_7.show();
    
    delay(1000*0.5);
    rgbled_7.setColor(0,0,0,0);
    rgbled_7.show();
    
}



