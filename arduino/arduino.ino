// --------------------------------------------------------------------------------------
//                                DOMOTIC ROOM
// --------------------------------------------------------------------------------------

//#include <IRremote.h>
//IRsend irsend;
// Estados del sistema
const char NOCONTROL = 'a';
const char LUZCONTROL = 'b';
const char ONLUZ = 'c';
const char OFFLUZ = 'd';

// PINES
const byte tempPin = A0;
const byte luzPin = A1;
const byte releluzPin = 2;

// PERIODOS
const unsigned long anPer = 1000;
const unsigned long monitorPer = 3000;
const unsigned long sistemaPer = 3000;
const unsigned long relePer = 3000;

//UMBRALES
const float UMBRALLUZ = 400;

// --------------------------------------------------------------------------------------
//                            ENTRADA ANALOGICA PERIODICA
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
//                                    MONITOR
// --------------------------------------------------------------------------------------
struct Monitor{
  unsigned long perms;
  unsigned long lastms;

  char mensaje;  
  float umbralLuz;
};
void setup_Monitor (struct Monitor& m, unsigned long perms, unsigned long currms) {
  Serial.begin(9600);
  m.perms = perms;
  m.lastms = currms - m.perms;  
  m.umbralLuz = UMBRALLUZ;  
}


void loop_Monitor(struct Monitor& m, const struct EntradaAnalogica& eluz, unsigned long currms) {
  if(currms - m.lastms >= m.perms) {
    //blink(3);
    m.lastms += m.perms;
    
    // RECIBE DATOS
    if (Serial.available()>0) {
      blink(1);
      m.mensaje = Serial.read();      
    }
        
    // ENVIA DATOS: temperatura y luminosidad.     
    Serial.println(eluz.estado);   
  }
  
}

// --------------------------------------------------------------------------------------
//                                    SISTEMA
// --------------------------------------------------------------------------------------
struct Sistema {
  int estado;
  int estadoant;
  unsigned long cambioms;
  unsigned long perms;
  unsigned long lastms;

  boolean luzencender;  
};
void setup_Sistema(struct Sistema& s, unsigned long perms, unsigned long currms){
  s.estado = NOCONTROL;
  s.perms = perms;
  s.lastms = currms - s.perms;
  s.luzencender = false;  
}


void loop_Sistema(struct Sistema& s, 
                  const struct EntradaAnalogica& eluz,                   
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

    // El estado solo va a cambiar dependiendo de un caracter que envia el monitor.    
    s.estado = m.mensaje;
    
    if(s.estadoant != s.estado) {
      s.cambioms = currms;
    }
  }
}

// --------------------------------------------------------------------------------------
//                                 RELE LUZ ESTUDIO
// --------------------------------------------------------------------------------------
struct ReleLuz{
  byte pin;
  byte estado;
  unsigned long perms;
  unsigned long lastms;
  unsigned long cambioms;
};
void setup_ReleLuz(struct ReleLuz& r, byte pin, unsigned long perms, unsigned long currms) {
  r.pin = pin;
  r.perms = perms;
  r.lastms = currms - r.perms;
  
  pinMode(r.pin, OUTPUT);
  r.estado = LOW;
  digitalWrite(r.pin, r.estado);
}


void loop_ReleLuz(struct ReleLuz& r, const struct Sistema& sis, unsigned long currms) {
  if(currms - r.lastms >= r.perms) {
    r.lastms += r.perms;
    // AJUSTE POR ESTADOS
    if((sis.estado == LUZCONTROL) && sis.luzencender == true) {
      r.estado = HIGH;
      r.cambioms = currms;
    } else {
      r.estado = LOW;
      r.cambioms = currms;
    }    
    if(sis.estado == ONLUZ) {
      r.estado = HIGH;
      r.cambioms = currms;
    } else if(sis.estado == OFFLUZ) {
      r.estado = LOW;
      r.cambioms = currms;
    }
  }
  if(r.cambioms == currms){
    digitalWrite(r.pin, !r.estado);
  }
}

// --------------------------------------------------------------------------------------
//                                      DECLARACION
// --------------------------------------------------------------------------------------
struct EntradaAnalogica sensorL;
struct Monitor monitor;
struct Sistema sistema;
struct ReleLuz releluz;

// --------------------------------------------------------------------------------------
//                                      SETUP
// --------------------------------------------------------------------------------------
void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  unsigned long currms = millis();  
  setup_EntradaAnalogica(sensorL, luzPin, anPer, currms);  
  setup_Monitor(monitor, monitorPer, currms);
  setup_Sistema(sistema, sistemaPer, currms);
  setup_ReleLuz(releluz, releluzPin, relePer, currms);  
}


// --------------------------------------------------------------------------------------
//                                       MAIN
// --------------------------------------------------------------------------------------
void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currms = millis();  
  loop_EntradaAnalogica(sensorL, currms);  
  loop_Monitor(monitor, sensorL, currms);
  loop_Sistema(sistema, sensorL, monitor, currms);
  loop_ReleLuz(releluz, sistema, currms);  
}



// funciones para depurar usando leds
void blink(unsigned int numberTimes) {
  blink(numberTimes, 100);
}

void blink(unsigned int numberTimes, int delayT) {
  for ( int i = 0; i < numberTimes; i++) {
    digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(delayT);              // wait for a second
    digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
    delay(delayT);
  }
}
