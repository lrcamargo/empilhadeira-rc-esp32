#include <Bluepad32.h>
#define ENA 25
#define IN1 13
#define IN2 12
#define ENB 26
#define IN3 27
#define IN4 14
#define GARFO1 32
#define GARFO2 33
#define LED_INTERNO 2

const int VELOCIDADE_MAXIMA = 255;
const int VELOCIDADE_GARFO = 200;
const int ZONA_MORTA = 35;
const bool INVERTER_GARFO = false;
const bool INVERTER_DIRECAO = false;
const unsigned long TIMEOUT = 500;
ControllerPtr controle = nullptr;
bool habilitado = false;
bool startAnterior = false;
unsigned long ultimoDado = 0;
unsigned long ultimoPrint = 0;

void pararMotores() {
  analogWrite(ENA, 0); analogWrite(ENB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  analogWrite(GARFO1, 0); analogWrite(GARFO2, 0);
}
void desabilitar() {
  habilitado = false;
  digitalWrite(LED_INTERNO, LOW);
  pararMotores();
}
void onConnectedController(ControllerPtr ctl) {
  if (controle == nullptr && ctl->isGamepad()) {
    controle = ctl; desabilitar(); startAnterior = true;
    ultimoDado = millis();
    Serial.println("Controle conectado! Pressione START para habilitar.");
  }
}
void onDisconnectedController(ControllerPtr ctl) {
  if (controle == ctl) {
    controle = nullptr; desabilitar();
    Serial.println("Controle desconectado!");
  }
}
void motor(int enable, int re, int frente, int pwm) {
  pwm = constrain(pwm, -255, 255);
  analogWrite(enable, 0);
  if (pwm > 0) {
    digitalWrite(re, LOW); digitalWrite(frente, HIGH);
    analogWrite(enable, pwm);
  } else if (pwm < 0) {
    digitalWrite(frente, LOW); digitalWrite(re, HIGH);
    analogWrite(enable, -pwm);
  } else {
    digitalWrite(re, LOW); digitalWrite(frente, LOW);
  }
}
int lerAnalogico(int valor) {
  valor = constrain(valor, -512, 512);
  if (abs(valor) <= ZONA_MORTA) return 0;
  int pwm = map(abs(valor), ZONA_MORTA, 512, 0, VELOCIDADE_MAXIMA);
  return valor > 0 ? pwm : -pwm;
}
void controlarGarfo(bool subir, bool descer) {
  if (INVERTER_GARFO) { bool aux = subir; subir = descer; descer = aux; }
  if (subir && !descer) {
    analogWrite(GARFO2, 0); analogWrite(GARFO1, VELOCIDADE_GARFO);
  } else if (descer && !subir) {
    analogWrite(GARFO1, 0); analogWrite(GARFO2, VELOCIDADE_GARFO);
  } else {
    analogWrite(GARFO1, 0); analogWrite(GARFO2, 0);
  }
}
void processarControle(ControllerPtr ctl) {
  bool start = ctl->miscStart();
  bool select = ctl->miscSelect();
  int vertical = lerAnalogico(-ctl->axisY());
  int horizontal = lerAnalogico(ctl->axisX());
  bool subir = (ctl->buttons() & 0x0008) != 0; // X fisico NS009-S
  bool descer = (ctl->buttons() & 0x0001) != 0; // B fisico NS009-S
  if (INVERTER_DIRECAO) horizontal = -horizontal;
  if (select) desabilitar();
  else if (start && !startAnterior) {
    if (vertical == 0 && horizontal == 0 && !subir && !descer) {
      habilitado = true; digitalWrite(LED_INTERNO, HIGH);
      Serial.println("Empilhadeira habilitada!");
    }
  }
  startAnterior = start;
  if (!habilitado) { pararMotores(); return; }
  int esquerda = vertical + horizontal;
  int direita = vertical - horizontal;
  int maior = max(abs(esquerda), abs(direita));
  if (maior > VELOCIDADE_MAXIMA) {
    esquerda = esquerda * VELOCIDADE_MAXIMA / maior;
    direita = direita * VELOCIDADE_MAXIMA / maior;
  }
  motor(ENA, IN1, IN2, esquerda);
  motor(ENB, IN3, IN4, direita);
  controlarGarfo(subir, descer);
}
void setup() {
  Serial.begin(115200);
  int pinos[] = {ENA, ENB, IN1, IN2, IN3, IN4, GARFO1, GARFO2, LED_INTERNO};
  for (int pino : pinos) pinMode(pino, OUTPUT);
  desabilitar();
  BP32.setup(&onConnectedController, &onDisconnectedController);
  Serial.println("EMPILHADEIRA RC - Aguardando controle...");
}
void loop() {
  BP32.update();
  if (controle && controle->isConnected()) {
    if (controle->hasData()) {
      ultimoDado = millis();
      processarControle(controle);
    }
    if (millis() - ultimoPrint >= 250) {
      ultimoPrint = millis();
      Serial.printf("X: %ld | Y: %ld | Botoes: 0x%04X | Misc: 0x%02X\n",
        (long)controle->axisX(), (long)controle->axisY(),
        (unsigned int)controle->buttons(), (unsigned int)controle->miscButtons());
    }
    if (millis() - ultimoDado > TIMEOUT) {
      desabilitar(); startAnterior = true;
    }
  } else desabilitar();
  delay(10);
}
