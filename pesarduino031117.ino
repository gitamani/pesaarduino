// Codice di Giuseppe Tamanini 11-2016
// con licenza Creative Commons BY-NC-SA

#include <Tlc5940.h>
#include <Seeed_QTouch.h>
#include "HX711.h"

HX711 scale(A1, A0);

int pesom=0; // memorizza il peso
int oldpesom=0; // vecchio valore del peso

int l=0;// variabile che serve per variare la luminosità delle striscie

int latchPin=7; // pin del shift register
int clockPin=6;
int dataPin=8;
int so=0; // shifOut value

int sensorPin = A5; // input pin analogico della fotoresistenza
int potentPin = A4; // input pin analogico del trimmer
int lightLed; // intensità uscite Tlc5940
int oldlightLed=0; // Vecchio valore lightLed
int sensorValue=0;  // memorizza il valore del sensore
int potentValue=0;  // memorizza il valore del trimmer

// definisce matrice display 7 segmenti
boolean md[11][8] { // da 
  {1,1,1,0,1,0,1,1},
  {0,0,1,0,0,0,0,1},
  {1,1,0,1,0,0,1,1},
  {0,1,1,1,0,0,1,1},
  {0,0,1,1,1,0,0,1},
  {0,1,1,1,1,0,1,0},
  {1,1,1,1,1,0,1,0},
  {0,0,1,0,0,0,1,1},
  {1,1,1,1,1,0,1,1},
  {0,1,1,1,1,0,1,1},
  {0,0,0,0,0,0,0,0},
};
boolean mdp[11][8] {
  {1,1,1,0,1,1,1,1},
  {0,0,1,0,0,1,0,1},
  {1,1,0,1,0,1,1,1},
  {0,1,1,1,0,1,1,1},
  {0,0,1,1,1,1,0,1},
  {0,1,1,1,1,1,1,0},
  {1,1,1,1,1,1,1,0},
  {0,0,1,0,0,1,1,1},
  {1,1,1,1,1,1,1,1},
  {0,1,1,1,1,1,1,1},
  {0,0,0,0,0,1,0,0},
};

int n=0; // numero visualizzato sul display del timer, della pesa e della luminosità delle strice led
int oldn; // vecchio valore di n
int s; // secondi del timer
int rig1; // riga del primo display
int rig2; // riga del secondo display
int rig3; // riga del terzo display
int col; // colonna
char buffer[4]; // stringhe per la visualizzazione dei numeri sui display
String sbuffer; // ''
String ss1;     // ''
String ss2;     // ''
String ss3;     // ''

int i; // touchNum (0-6) compreso nessuno (-1) "false"
int oldo=9; // vecchio nessuntouch (-1, 9 se è premuto un touch)
int oldi=-1; // vecchio touchNum compreso nessuno (-1) "false"
int oldit=9; // vecchio touchNum (0-6) valido "true"
unsigned long time; // variabile per misurare il tempo di premuta
int pressTime; // tempo di pressione del touch
unsigned long notime; // tempo di in cui nessun touch è premuto
unsigned long ntime; // tempo di accensione led touch
unsigned long ttime; // tempo del sistema all'avvio del countdown del timer
unsigned long ptime; // tempo di accesione pesa
unsigned long titime; // tempo di non premuta touch timer
boolean press=false; // vera quando è premuto un touch valido
int fase[]={0,0,0,0,0,0,0}; // matrice che cambia in base al tempo di premuta di ogni singolo touch
int oldfase[]={0,0,0,0,0,0,0}; // fase precedente
int fasen[]={1,1,50,50,50,1,1}; // mattrice che conta il numero di pressioni di singolo touch
int oldfasen[]={0,0,0,0,0,0,0}; // fasen precedente
boolean on[]={false,false,false,false,false,false,false}; // funzione accesa
boolean oldon[]={true,false,true,true,true,true,true}; // vecchia funzione
boolean acceso=true; // vero se i led dei touch sono stati accesi
boolean oldacceso=false; // vecchio valore acceso
boolean spento=false; // vero se ho premuto il touch 5 per la sequenza di spegnimento

int tp; // tasto premuto (accendi)
int tn; // ultimo tasto premuto (accendi)
boolean nt=false; // nessun tasto premuto
int oldtn=9;  // ultimo tasto di comando
int oldtp=-1; // ultimo tasto sequenza accensione
int oldts=-1; // ultimo tasto sequenza spegnimento
int st=0; // sequenza tasti
unsigned long timeacc; // tempo inizio di premuta tasti sequenza accensione (accendi)
unsigned long tt; // tempo di premuta sequenza accendi()

boolean c=false; // vera se il contdoun è iniziato

int led[]={50,50,50}; // intensità strisce led
boolean ledup[]={true, true, true}; // luminosità delle strisce è in crescita
boolean ledon[]={false, false, false}; // stato delle strisce led
boolean oldledon[]={false, false, false}; // stato delle strisce led
int nled=0; // indice striscia led su cui agisce il touch
unsigned long ltime; // tempo per per spegnere il display quando si accendono le stricie led

int nc=0; // numero ciclo lavaverdura
int oldnc=-1; // vecchio numero ciclo lavaverdura
boolean r1on=false; // il rele1 è acceso
boolean r2on=false; // il relè 2 è acceso
boolean lav1=false; // lava verdura primo ciclo è on r1on
boolean lav2=false; // lava verdura secondo ciclo è on r2on
unsigned long ttime_r; // tempo primo ciclo lava verdura
unsigned long ttime_l; // tempo primo ciclo lava verdura

boolean proj=false; // il proiettore è acceso

int pinrele1=4; // pin rele1
int pinrele2=5; // pin rele2
int pinrele3=A3; // pin rele3

int BUZZER=2; // piedino a cui è collegato il buzzer
unsigned long btime; // tempo di pausa del buzzer
int j; // variabili del ciclo del bip bip
int k;
int oldj;
boolean b=false; // vera se sta suonando il buzzer

void setup() {
  pinMode(latchPin, OUTPUT); // setta in uscita i canali del shift register
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);

  Serial.begin(38400);
  
  Tlc.init(0); // inizializza il TLC5940 e setta tutti i canali su off
  delay(250);
  Tlc.clear();
  Tlc.update();
  
  // setta il tempo massimo della durata del touch prima di resettarlo (160ms * 50 = 8 sec)
  QTouch.setMaxDuration(50);
  
  // setta la sensibilità dei touch il valore consigliato è 20 (0 è il valore minimo)
  QTouch.setNTHRForKey(20,0);
  QTouch.setNTHRForKey(10,1);
  QTouch.setNTHRForKey(10,2);
  QTouch.setNTHRForKey(10,3);
  QTouch.setNTHRForKey(10,4);
  QTouch.setNTHRForKey(10,5);
  QTouch.setNTHRForKey(20,6);

  scale.set_scale(418.f); // calibra la bilancia
  scale.tare();

  pinMode(pinrele1, OUTPUT); // setta i pin dove sono collegati i relè come OUTPUT
  pinMode(pinrele2, OUTPUT);
  pinMode(pinrele3, OUTPUT);

  pinMode(BUZZER, OUTPUT); // Imposta la porta di collegamento per il buzzer


}

void loop() {
  if (acceso) {
    if (acceso!=oldacceso) {
      potentValue = analogRead(potentPin); // legge il valore del trimmer che regola i tempi del lavaverdura
      sensorValue = analogRead(sensorPin); // legge il valore della fotoresistenza che regola la luminosità dei led sui touch e della pesa
      lightLed=(sensorValue-512)*8; // calcola la luce dei led
      if (lightLed>4096) lightLed=4095;
      acceso=true;
      oldacceso=false;
      Tlc.set(26, lightLed); // accende i led del touch 25, 26, 27
      Tlc.set(27, lightLed);
      Tlc.set(28, lightLed);
      Tlc.update();
      so=0;
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, so);  //accende il led fucsia del touch premuto
      digitalWrite(latchPin, HIGH);
      oldacceso=true;
    }
    // legge il numero del touch premuto (0-6) -1 se nessuno 
    i=QTouch.touchNum();
    so=0;
    if (i==0) so=so+1; // in base al touch premuto calcola il datapin da (01000000,00100000... a 00000001 in binario)
    if (i==1) so=so+2;
    if (i==2) so=so+4;
    if (i==3) so=so+8;
    if (i==4) so=so+16;
    if (i==5) so=so+32;
    if (i==6) so=so+192;
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, so);  // accende il led fucsia del touch premuto
    digitalWrite(latchPin, HIGH);
    if (i==-1 && oldo==9) {
      oldo=-1;
      notime=millis();
    }
    if (i==-1 && oldi==-1 && spento && millis()-notime>500) {
      spento=false; // se non è stato premuto nessun touch per più di mezzo secondo azzera la sequenza di spegnimento
      oldo=9;
      Serial.println("fallito");
    }
    if (i!=oldi) {
      Serial.println(i);
      ntime=millis();
      if (i>=0) { // se uno dei touch è premuto
        oldo=9;
        oldi=i;        
        oldit=i;
        time=millis();
      } else {
        if (i!=oldit) { // se il touchNum è diverso dal predente (1-6) valido "true"
          pressTime=millis()-time;
          
          tp=oldit;
          
          spegni(); // avvia la sequenza di spegnimento
              
          if (oldit==0 || oldit==1 || oldit==5 || oldit==6) { // per i touch 0,1,5,6
            if (pressTime>1000) fase[oldit]=fase[oldit]+1; // se il touch è stato premuto per più di 1 secondi fase viene incrementata di 1
            //buzz(BUZZER, 2093, 50); // emissione suono a 2 KHz
          } else if (oldit==2 || oldit==3 || oldit==4) { // per i touch delle strice led 2,3,4
             if (pressTime>1000) fase[oldit]=1; // se il touch è stato premuto per più di 1 secondi fase=1
             if (pressTime>3000) fase[oldit]=2; // se il touch è stato premuto per più di 3 secondi fase=2
          }
          if (spento==false && fase[oldit]==1 && pressTime>200 && pressTime<500 && i==-1) { // se ho premuto lo stesso touch per più di una volta
            if (oldit>1 && oldit<5 && ledup[oldit-2]==false) { // se si usano i touch 2-3-4 che comandano le strisce led e ledup è false
              fasen[oldit]=fasen[oldit]-5; // fasen viene diminuita di 5
            } else if (oldit>1 && oldit<5 && ledup[oldit-2]) { // se ledup è true
              fasen[oldit]=fasen[oldit]+5; // fasen viene incrementata di 5
            } else { // con gli altri touch 
              fasen[oldit]=fasen[oldit]+1; // fasen viene increamentata di 1
            }
          }
          oldi=i;
          on[oldit]=true; 
          /*Serial.println(pressTime);
          Serial.print("fase[");
          Serial.print(oldit);
          Serial.print("]=");
          Serial.println(fase[oldit]);
          Serial.print("fasen[");
          Serial.print(oldit);
          Serial.print("]=");
          Serial.println(fasen[oldit]);*/
        }
      }
    } else {
      //Serial.println(millis()-ntime);
      if (millis()-ntime>20000) { // se non viene premuto nessun touch per 20 sec
        Tlc.set(26, 0); // spegne i led dei touch 26, 27, 28
        Tlc.set(27, 0);
        Tlc.set(28, 0);
        Tlc.update();
        Tlc.update();
        acceso=false;
        oldi=-1;
        oldit=-1;
      }
    }
    if (on[0]) timer();
    if (on[1] && fase[1]==1 || fase[1]>1) leggipeso();
    if (on[2] && fase[2]==1) {
      oldon[2]=true;
      nled=0;
      ledon[0]=true;
      accendiLed();
    } else if (on[2] && fase[2]==2) {
      Tlc.set(29, 0);
      spegnidisplay();
      ledon[0]=false;
      oldledon[0]=true;
      fase[2]=led[2];
      on[2]=false;
      if (on[0]) scrivi(n);
    }
    if (on[3] && fase[3]==1) {
      oldon[3]=true;
      nled=1;
      ledon[1]=true;
      accendiLed();
    } else if (on[3] && fase[3]==2) {
      Tlc.set(30, 0);
      spegnidisplay();
      ledon[1]=false;
      oldledon[1]=true;
      fase[3]=led[3];
      on[3]=false;
      if (on[0]) scrivi(n);
     }
      if (on[4] && fase[4]==1) {
      oldon[4]=true;
      nled=2;
      ledon[2]=true;
      accendiLed();
    } else if (on[4] && fase[4]==2) {
      Tlc.set(31, 0);
      spegnidisplay();
      ledon[2]=false;
      oldledon[2]=true;
      fase[4]=led[4];
      on[4]=false;
      if (on[0]) scrivi(n);
     }
    if (on[5] && fase[5]==1) {
      proj=1-proj;
      if (proj==true) {
        digitalWrite(pinrele3, HIGH);
        Serial.println("acceso");
      } else {
        digitalWrite(pinrele3, LOW);
        Serial.println("spento");
      }
      on[5]=false;
      fase[5]=0;
    }
    if (on[6] && fase[6]==1) { //accende
      lavaverdura();
    } else if (on[6] && fase[6]==2) { // spegne il ciclo lavaverdura
      r1on=false; // spegne i relè
      r2on=false;
      accendirele();
      lav1=false;
      lav2=false;
      nc=0;
      oldnc=-1;
      fase[6]=0;  // risponde al programma principale che ha finito
      on[6]=false;
      spegnidisplay();
      if (on[0]) scrivi(n);
    }
    delay(10);
  } else {
    accendi();
    if (on[0]) timer();
    if (on[6] && fase[6]==1) lavaverdura();
    if (on[1]) leggipeso();
  }
}

void accendi() {
  //Serial.println("accendi");
  if (acceso==false) {
    if (acceso!=oldacceso) {
     oldacceso=false;
    }
    tp = QTouch.touchNum();
    if (tp>0 && tp!=oldtp) {
      /*Serial.print("tasto: ");
      Serial.print(tp);
      Serial.println(oldtp);*/
    }
    if (tp==2 && oldtp!=2 && tn==0) { // sequenza di accensione con sfioramento da sinistra a destra sui touch 5-4-3-2
      timeacc=millis();
      oldtp=2;
    } else if (tp==2 && oldtp==2) {
      tt=millis()-timeacc;
      if (tt>50 && tt<500) {
        tn=1;
      }
    }
    if (tp==3 && oldtp==2 && tn==1) {
      timeacc=millis();
      oldtp=3;
    } else if (tp==3 && oldtp==3) {
      tt=millis()-timeacc;
      if (tt>50 && tt<500) {
        tn=2;
      }
    }
    if (tp==4 && oldtp==3 && tn==2) {
      timeacc=millis();
      oldtp=4;
    } else if (tp==4 && oldtp==4) {
      tt=millis()-timeacc;
      if (tt>50 && tt<500) {
        tn=3;
      }
    }
    if (tp==5 && oldtp==4 && tn==3) {
      timeacc=millis();
      oldtp=5;
    } else if (tp==5 && oldtp==5) {
      tt=millis()-timeacc;
      if (tt>50 && tt<500) {
        tn=0;
        acceso=true;
        Serial.println("acceso");
        Tlc.set(26, lightLed); // accende i led del touch
        Tlc.set(27, lightLed);
        Tlc.set(28, lightLed);
        Tlc.update();
        delay(500);
        ntime=millis(); // fa ripartire il tempo che causa lo spegnimento automatico dei led dei touch
      }
    } else if (tp>0) { // se viene premuto un touch diverso da 2-5 aggiorna comunque oldtp
      oldtp=tp;
    }
  }
}

void accendiLed() {
  //Serial.println("accendiLed");
  if (on[2+nled]) { // se è stato premuto un touch di comando strice led 
    if (fase[2+nled]==1 && fasen[2+nled]!=oldfasen[2+nled]) { // se fase è 1 premo ulteriormente il touch
      ltime=millis(); // memorizza il tempo del sistema
      if (fasen[2+nled]>100) {
        fasen[2+nled]=100; // impedisce che fasen superi il valore 100
       ledup[nled]=false; // la variazione sarà negativa
      }
      if (fasen[2+nled]<5) {
        fasen[2+nled]=5; // impedisce che fasen non sia inferiore di 5
        ledup[nled]=true; // la variazione sarà positiva
      }
      l=fasen[2+nled]; // variabile di scambio
      Tlc.set(29+nled, 0.01*l*l*40.95); // per simulare il comportamento di una lampada ad incondescenze il valore viene calcolato con la formula di una parabola  y=a*x^2 
      Serial.println(l);
      scrivi3(l); // scrivi il valore l sul display a 3 cifre
      led[nled]=l; // memorizza l in led[]
      oldfasen[2+nled]=fasen[2+nled];
    } else if (millis()-ltime>2000) { // se non premo nessun touch per più di 3 sec
      spegnidisplay(); // spegne il display
      on[2+nled]=false; // segnala al programma principale che il touch è stato rilasciato
      oldfasen[2+nled]=0; // permette che una nuova premuta del touch permetta di variare la luminosità
      if (on[0]) scrivi(n);
    }
  }
}

void lavaverdura() {
  //Serial.println("lavaverdura");
  //Serial.print("nc=");
  //Serial.println(nc);
  if (fase[6]==1 && nc!=oldnc) { // accende il relè1
    ttime_r=millis();
    r1on=true;
    r2on=false;
    lav1=true;
    oldnc=nc;
    accendirele(); // esegue la procedura che accende i relè
    Tlc.set(0, 0);
    Tlc.set(1, 0);
    Tlc.set(2, lightLed);
    Tlc.set(3, 0);
    Tlc.set(4, 0);
    Tlc.set(5, 0);
    Tlc.set(6, 0);
    Tlc.set(7, 0);
    Tlc.set(8, lightLed);
    Tlc.set(9, 0);
    Tlc.set(10, lightLed);
    Tlc.set(11, lightLed);
    Tlc.set(12, 0);
    Tlc.set(13, 0);
    Tlc.set(14, 0);
    Tlc.set(15, 0);
    Tlc.set(16, 0);
    Tlc.set(17, 0);
    Tlc.set(18, 0);
    Tlc.set(19, 0);
    Tlc.set(20, 0);
    Tlc.set(21, 0);
    Tlc.set(22, 0);
    Tlc.set(23, 0);
    Tlc.set(24, 0);
    Tlc.update();
  }
  if (lav1==true && millis()-ttime_r>6000*(float)potentValue/256) { // in base al tempo in funzione del trimmer lascia acceso il rele1 e poi lo spegne per accendere il relè2
    ttime_l=millis();
    r1on=false;
    r2on=true;
    lav1=false;
    lav2=true;
    accendirele();
    Tlc.set(0, lightLed);
    Tlc.set(1, lightLed);
    Tlc.set(2, lightLed);
    Tlc.set(3, lightLed);
    Tlc.set(4, 0);
    Tlc.set(5, 0);
    Tlc.set(6, 0);
    Tlc.set(7, 0);
    Tlc.set(8, lightLed);
    Tlc.set(9, lightLed);
    Tlc.set(10, lightLed);
    Tlc.set(11, 0);
    Tlc.set(12, 0);
    Tlc.set(13, 0);
    Tlc.set(14, 0);
    Tlc.set(15, 0);
    Tlc.set(16, lightLed);
    Tlc.set(17, 0);
    Tlc.set(18, 0);
    Tlc.set(19, lightLed);
    Tlc.set(20, lightLed);
    Tlc.set(21, 0);
    Tlc.set(22, 0);
    Tlc.set(23, 0);
    Tlc.set(24, 0);
    Tlc.update();
  }
  if (lav2==true && lav1==false && millis()-ttime_l>4000*(float)potentValue/256) { // in base al tempo in funzione del trimmer lascia acceso il rele2
    lav1=true;
    lav2=false;
    nc=nc+1;
    if (nc==3) { // quando il ciclo è stato eseguito 3 volte
      r1on=false; // spegne i relè
      r2on=false;
      accendirele();
      lav1=false;
      lav2=false;
      nc=0;
      oldnc=-1;
      fase[6]=0;  // risponde al programma principale che ha finito
      on[6]=false;
      spegnidisplay();
      if (on[0]) scrivi(n);
    }
  }
}

void accendirele() {
  //Serial.println("accendirele");
  if (r1on==true) {
    digitalWrite(pinrele1, HIGH);
  } else {
    digitalWrite(pinrele1, LOW);
  }
  if (r2on==true) {
    digitalWrite(pinrele2, HIGH);
  } else {
    digitalWrite(pinrele2, LOW);
  }
}

void leggipeso() {
  //Serial.println("leggipeso");
  if (fase[1]==1 && oldon[1]==false) {
    ptime=millis();
    scale.tare();
    Serial.println("tara");
    oldon[1]=true;
    oldfase[1]=fase[1];
  }
  if (fase[1]>1 && oldfase[1]!=fase[1]) {
    scale.tare();
    oldfase[1]=fase[1];
    Serial.println("tara");
  }
  pesom=scale.get_units(10), 1;
  if (abs(pesom-oldpesom)>10) {
    ptime=millis();
    Serial.println(ptime);
    oldpesom=pesom;
  }
  sprintf(buffer,"%04d",pesom);
  sbuffer=buffer;
  if (pesom<0) {
    Tlc.set(24, lightLed);
    Tlc.update();
  } else {
    Tlc.set(24, 0);
    Tlc.update();
  }
  if (pesom<1000) {
    ss1=sbuffer.substring(1,2);
    if (ss1=="0") {
      rig1=10;
    } else {
      rig1=ss1.toInt();  
    }
    ss2=sbuffer.substring(2,3);
    if (ss1=="0" && ss2=="0") {
      rig2=10;
    } else {
      rig2=ss2.toInt();  
    }
    ss3=sbuffer.substring(3,4);
    rig3=ss3.toInt();  
  } else {
    ss1=sbuffer.substring(0,1);
    rig1=ss1.toInt();  
    ss2=sbuffer.substring(1,2);
    rig2=ss2.toInt();
    ss3=sbuffer.substring(2,3);
    rig3=ss3.toInt();
    if (sbuffer.substring(3,4).toInt()>4) {
      rig3=rig3+1;
    }
  }
  for (col=0; col<8; col++) {
    if (md[rig1][col]!=0) {
      Tlc.set(col, lightLed);
    } else {
      Tlc.set(col, 0);
    }
  }
  if (pesom>999) {
    for (col=0; col<8; col++) {
      if (mdp[rig1][col]!=0) {
        Tlc.set(col, lightLed);
      } else {
        Tlc.set(col, 0);
      }
    }
  } else {
    for (col=0; col<8; col++) {
      if (md[rig1][col]!=0) {
        Tlc.set(col+8, lightLed);
      } else {
        Tlc.set(col, 0);
      }
    }
  }
  for (col=0; col<8; col++) {
    if (md[rig2][col]!=0) {
      Tlc.set(col+8, lightLed);
    } else {
      Tlc.set(col+8, 0);
    }
  }
  for (col=0; col<8; col++) {
    if (md[rig3][col]!=0) {
      Tlc.set(col+16, lightLed);
    } else {
      Tlc.set(col+16, 0);
    }
  }
  Tlc.update();
  scale.power_down();
  delay(10);
  scale.power_up();
  //Serial.print("ptime=");
  //Serial.println(millis()-ptime);
  if (millis()-ptime>15000) {
    spegnidisplay();
    if (on[0]) scrivi(n); // se è acceso il timer visualizza i minuti
    fase[1]=0;
    fasen[1]=1;
    oldfasen[1]=0;
    on[1]=false; // la spesa è spenta
    oldon[1]=false; // vecchia on
    time=millis(); // fa ripartire il tempo per impedire di spegnere i led dei touch
  }
}

void timer() {
    //Serial.println("timer");
    if (on[0]) {
      if (fase[0]!=oldfase[0]) { // se è cambiata la fase
        titime=millis(); // azzera il tempo di non premuta
        oldfase[0]=fase[0];
      }
      if (millis()-titime>3000 && titime!=0 && fase[0]==1) fase[0]=2; // se non viene premuto il touch 0 nei casi indicati spegne
      if (fase[0]==1 && fasen[0]!=oldfasen[0]) {
        titime=millis();
        n=fasen[0];
        scrivi(n);
        oldfasen[0]=fasen[0];
      } else if (fase[0]==2) { // countdown
        if (c==false) { // c se false
          ttime=millis(); // prende il tempo del sistema
          s=n*120; // dai minuti (n) calcola i mezzi secondi (per far lampeggiare il punto)
          c=true; // il ciclo è iniziato
        } else {
          if (millis()-ttime>500 && n>0) { // fa lampeggiare il punto del display a destra canale 21 ogni secondo
            ttime=millis();
            s=s-1; // decrementa i mezzi secondi
            if (float(s)/2.0==int(s/2)) { // se s/2 è pari 
              Tlc.set(21,lightLed); // accende il punto
            } else {
              Tlc.set(21,0); // spegne il punto
            }
            Tlc.update();
            if (float(s)/120.0==s/120) { // se è passato un minuto decrementa i minuti
              n=n-1;
              if (on[1]==false || on[6]==false) {
                scrivi(n); // visualizza il nuovo valore sul display
              }
            }
          }
          if (n==0) {
            if (b==false) {
              Serial.println("bip bip");
              if (on[1]==false || on[6]==false) {
                 scrivi(n); // visualizza il nuovo valore sul display
              } else {
                Tlc.set(21, 0); // spegne il punto
                Tlc.update();
              }
              j=0; // conta i cicli di bip bip bip
              oldj=-1;
              Serial.println("spegnitimer");
              on[1]=false;
            }
            b=true;
            if (j!=oldj && j<10) {
              for (int k=0; k <= 2; k++) { // 3 bip
                buzz(BUZZER, 2093, 100); // emissione suono a 2 KHz
                    delay(100); // attesa tra un bip e l'altro
              }
              btime=millis();
              oldj=j;
            }
            if (j==oldj && millis()-btime>450) { // fa passare mezzo secondo senza suono
              j=j+1;
            }
            if (j==9) {
              b=false;  // fa ripartire il buzzer in un nuovo ciclo 
              c=false;  // azzera il tempo in un nuovo ciclo
              fase[0]=3; // quando termina la sequenza di bip avvia la fase di spegnimento
            }
          }
        }
       } else if (fase[0]==3) { // spegne il timer
        n=0;
        c=false;
        fase[0]=0;
        fasen[0]=1;
        oldfasen[0]=0;
        on[0]=false; // pesa spenta
        oldacceso=false;
        titime=0;
        if (on[1]=false || on[6]==false) {
          spegnidisplay();
        }
        if (on[0]) scrivi(n); // se è acceso il timer visualizza i minuti
      }
   }
}


void scrivi(int n) {
    Serial.print("scrivi ");
    Serial.println(n);
    sprintf(buffer,"%02d",n);
    sbuffer=buffer;
    ss2=sbuffer.substring(0,1);
    if (ss2=="0") {
      rig2=10;
    } else {
      rig2=ss2.toInt();  
    }
    ss3=sbuffer.substring(1,2);
    rig3=ss3.toInt();  
    for (col=0; col<8; col++) {
      if (md[rig2][col]!=0) {
        Tlc.set(col+8, lightLed);
      } else {
        Tlc.set(col+8, 0);
      }
    }
    for (col=0; col<8; col++) {
      if (md[rig3][col]!=0) {
        Tlc.set(col+16, lightLed);
      } else {
        Tlc.set(col+16, 0);
      }
    }
    Tlc.update();
}

void scrivi3(int l) {
  //Serial.println("scrivi3");
  sprintf(buffer,"%03d",l);
  sbuffer=buffer;
  //Serial.println(sbuffer);
  ss1=sbuffer.substring(0,1);
  if (ss1=="0") {
    rig1=10;
  } else {
    rig1=ss1.toInt();  
  }
  ss2=sbuffer.substring(1,2);
  if (ss1=="0" && ss2=="0") {
    rig2=10;
  } else {
    rig2=ss2.toInt();  
  }
  ss3=sbuffer.substring(2,3);
  rig3=ss3.toInt();  
  for (col=0; col<8; col++) {
    if (md[rig1][col]!=0) {
      Tlc.set(col, lightLed);
    } else {
      Tlc.set(col, 0);
    }
  }
  for (col=0; col<8; col++) {
    if (md[rig1][col]!=0) {
      Tlc.set(col+8, lightLed);
    } else {
      Tlc.set(col, 0);
    }
  }
  for (col=0; col<8; col++) {
    if (md[rig2][col]!=0) {
      Tlc.set(col+8, lightLed);
    } else {
      Tlc.set(col+8, 0);
    }
  }
  for (col=0; col<8; col++) {
    if (md[rig3][col]!=0) {
      Tlc.set(col+16, lightLed);
    } else {
      Tlc.set(col+16, 0);
    }
  }
  Tlc.update();
}

void spegnidisplay() {
    Tlc.set(0, 0); // spegne il display
    Tlc.set(1, 0);
    Tlc.set(2, 0);
    Tlc.set(3, 0);
    Tlc.set(4, 0);
    Tlc.set(5, 0);
    Tlc.set(6, 0);
    Tlc.set(7, 0);
    Tlc.set(8, 0);
    Tlc.set(9, 0);
    Tlc.set(10, 0);
    Tlc.set(11, 0);
    Tlc.set(12, 0);
    Tlc.set(13, 0);
    Tlc.set(14, 0);
    Tlc.set(15, 0);
    Tlc.set(16, 0);
    Tlc.set(17, 0);
    Tlc.set(18, 0);
    Tlc.set(19, 0);
    Tlc.set(20, 0);
    Tlc.set(21, 0);
    Tlc.set(22, 0);
    Tlc.set(23, 0);
    Tlc.set(24, 0);
    Tlc.update();
}

void buzz(int targetPin, long frequency, long length) {
  long delayValue = 1000000/frequency/2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length/ 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to 
  //// get the total number of cycles to produce
  for (long i=0; i < numCycles; i++){ // for the calculated length of time...
    digitalWrite(targetPin,HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin,LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait againf or the calculated delay value
  }
}

void spegni() {
  if (tp==5 && pressTime>50 && pressTime<500) { // È stato premuto il touch 5 per la sequenza di spegnimento
    tn=1;
    spento=true;
  } else {
    if (tp==4 && tn==1 &&pressTime>50 && pressTime<500) {
      tn=2;
    } else if (tp==3 && tn==2 &&pressTime>50 && pressTime<500) {
      tn=3;
    } else if (tp==2 && tn==3  &&pressTime>50 && pressTime<500) {
      //spegnitutto();
      pressTime=0;
      acceso=false;
      oldacceso=false;
      spento=false;
      Tlc.set(26, 0);
      Tlc.set(27, 0);
      Tlc.set(28, 0);
      spegnidisplay();
      Tlc.update();
      oldi=-1;
      oldit=-1;
     } else if (tp>0) { // se viene premuto un touch per un tempo più lungo o diverso da 2-5 annulla la sequenza
      Serial.println("fallito");
      spento=false;
      tn=0;
    }
  }
  Serial.print(tp);
  Serial.print(tn);
  Serial.println(spento);
}

void spegnitutto() {
      //Serial.println("spegni tutto");
      acceso=false;
      oldacceso=false;
      spento=false;
      tn=0;
      n=0;
      c=false; // indica che il countdown è finito
      digitalWrite(latchPin, LOW); // spegne i led fucsia
      shiftOut(dataPin, clockPin, MSBFIRST, 0);
      digitalWrite(latchPin, HIGH);
      r1on=false; // spegne i relè
      r2on=false;
      accendirele();
      lav1=false;
      lav2=false;
      nc=0;
      oldnc=-1;
      on[6]=false; // segnale al programma principale che il lavaverdura è stato spento
      oldtp=-1; // nessun touch premuto*/
      fase[0]=0;
      fase[1]=0;
      fase[2]=0;
      fase[3]=0;
      fase[4]=0;
      fase[5]=0;
      fase[6]=0;
      oldfase[0]=0;
      oldfase[1]=0;
      oldfase[2]=0;
      oldfase[3]=0;
      oldfase[4]=0;
      oldfase[5]=0;
      oldfase[6]=0;
      fasen[0]=1;
      fasen[1]=1;
      fasen[2]=50;
      fasen[3]=50;
      fasen[4]=50;
      fasen[5]=1;
      fasen[6]=1;
      oldfasen[0]=0;
      oldfasen[1]=0;
      oldfasen[2]=0;
      oldfasen[3]=0;
      oldfasen[4]=0;
      oldfasen[5]=0;
      oldfasen[6]=0;
      on[0]=false;
      on[1]=false;
      on[2]=false;
      on[3]=false;
      on[4]=false;
      on[5]=false;
      on[6]=false;
      oldon[0]=true;
      oldon[1]=true;
      oldon[2]=true;
      oldon[3]=true;
      oldon[4]=true;
      oldon[5]=true;
      oldon[6]=true;
      Tlc.set(25, 0);
      Tlc.set(26, 0);
      Tlc.set(27, 0);
      Tlc.set(28, 0);
      Tlc.set(29, 0);
      Tlc.set(30, 0);
      Tlc.set(31, 0);
      spegnidisplay();
      led[0]=50;
      led[1]=50;
      led[2]=50;
      delay(500);
}

