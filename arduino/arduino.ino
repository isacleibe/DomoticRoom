// --------------------------------------------------------------------------------------
//                                DOMOTIC ROOM
// --------------------------------------------------------------------------------------
#include <IRremote.h>
IRsend irsend;
// Estados del sistema
const int NOCONTROL = 0;
const int LUZCONTROL = 1;
const int TEMPCONTROL = 2;
const int ALLCONTROL = 3;
const int ONLUZ = 4;
const int OFFLUZ = 5;
const int ONTEMP = 6;
const int OFFTEMP = 7;
// --------------------------------------------------------------------------------------
//                                Entrada Analogica Periodica
// --------------------------------------------------------------------------------------
struct EntradaAnalogica{
  byte pin;
  float estado;
  unsigned long perms;
  unsigned long lastms;
};
void setup_EntradaAnalogica (struct EntradaAnalogica& e, byte pin, unsigned long perms, unsigned long currms) {
  e.pin = pin;
  e.perms = perms;
  e.lastms = currms - e.perms;
  pinMode(e.pin, INPUT);
  e.estado = analogRead(e.pin);
}

void loop_EntradaAnalogica (struct EntradaAnalogica& e, unsigned long currms) {
  if (currms - e.lastms >= e.perms) {
    e.lastms += e.perms;
    e.estado = analogRead(e.pin);
  }
}
// --------------------------------------------------------------------------------------
//                                      Monitor
// --------------------------------------------------------------------------------------
struct Monitor{
  unsigned long perms;
  unsigned long lastms;

  char mensaje;
  float umbralTemp;
  float umbralLuz;
};
void setup_Monitor (struct Monitor& m, unsigned long perms, unsigned long currms) {
  Serial.begin(9600);
  m.perms = perms;
  m.lastms = currms - m.perms;  
  m.umbralLuz = UMBRALLUZ;
  m.umbralTemp = UMBRALTEMP;
}

void loop_Monitor(struct Monitor& m, const struct EntradaAnalogica& eluz, const struct EntradaAnalogica& etemp, unsigned long currms) {
  if(currms - m.lastms >= m.perms) {
    //blink(3);
    m.lastms += m.perms;
    
    // RECIBE DATOS
    if (Serial.available()>0) {
      blink(4);
      m.mensaje = Serial.read();      
    }
        
    // ENVIA DATOS: temperatura y luminosidad.     
    Serial.println(eluz.estado);
    Serial.println(etemp.estado);
  }
  
}
// --------------------------------------------------------------------------------------
//                                      Sistema
// --------------------------------------------------------------------------------------
struct Sistema {
  int estado;
  int estadoant;
  unsigned long cambioms;
  unsigned long perms;
  unsigned long lastms;

  boolean luzencender;
  boolean tempencender;
};
void setup_Sistema(struct Sistema& s, unsigned long perms, unsigned long currms){
  s.estado = NOCONTROL;
  s.perms = perms;
  s.lastms = currms - s.perms;
  s.luzencender = false;
  s.tempencender = false;
}

void loop_Sistema(struct Sistema& s, 
                  const struct EntradaAnalogica& eluz, 
                  const struct EntradaAnalogica& etemp, 
                  const struct Monitor& m, 
                  unsigned long currms){
                    
  if(currms - s.lastms >= s.perms) {
    s.lastms += s.perms;
    s.estadoant = s.estado;

    if(eluz.estado < m.umbralLuz){
      s.luzencender = true;
    } else {
      s.luzencender = false;
    }
    if(etemp.estado < m.umbralTemp){
      s.tempencender = true;
    } else {
      s.tempencender = false;
    }

    // El estado solo va a cambiar dependiendo de un caracter que envia el monitor.
    switch (m.mensaje) {
      case 'a':
        s.estado = NOCONTROL;
      break;
      case 'b':
        s.estado = LUZCONTROL;
      break;
      case 'c':
        s.estado = TEMPCONTROL;
      break;
      case 'd':
        s.estado = ALLCONTROL;
      break;
    }
    //s.estado = m.mensaje; Terminariamos más rapido así si s.estado fuese un char.
    
    if(s.estadoant != s.estado) {
      s.cambioms = currms;
    }
  }
}

// --------------------------------------------------------------------------------------
//                                      SETUP
// --------------------------------------------------------------------------------------
void setup() {
  // put your setup code here, to run once:

}


// --------------------------------------------------------------------------------------
//                                       MAIN
// --------------------------------------------------------------------------------------
void loop() {
  // put your main code here, to run repeatedly:

}
