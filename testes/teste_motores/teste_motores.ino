#define ENA 25
#define IN1 13
#define IN2 12
#define ENB 26
#define IN3 27
#define IN4 14
#define GARFO1 32
#define GARFO2 33
#define PWM_TESTE 180
void pararTudo() {
  analogWrite(ENA, 0); analogWrite(ENB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  digitalWrite(GARFO1, LOW); digitalWrite(GARFO2, LOW);
}
void setup() {
  Serial.begin(115200);
  int pinos[] = {ENA, ENB, IN1, IN2, IN3, IN4, GARFO1, GARFO2};
  for (int pino : pinos) pinMode(pino, OUTPUT);
  pararTudo();
  Serial.println("1 Esquerda re | 2 Esquerda frente | 3 Direita re | 4 Direita frente | 5 Garfo 1 | 6 Garfo 2 | 0 Parar");
}
void loop() {
  static unsigned long inicio = 0;
  static bool ativo = false;
  if (Serial.available()) {
    char c = Serial.read();
    if (c >= '0' && c <= '6') {
      pararTudo();
      switch(c) {
        case '1': digitalWrite(IN1, HIGH); analogWrite(ENA, PWM_TESTE); break;
        case '2': digitalWrite(IN2, HIGH); analogWrite(ENA, PWM_TESTE); break;
        case '3': digitalWrite(IN3, HIGH); analogWrite(ENB, PWM_TESTE); break;
        case '4': digitalWrite(IN4, HIGH); analogWrite(ENB, PWM_TESTE); break;
        case '5': digitalWrite(GARFO1, HIGH); break;
        case '6': digitalWrite(GARFO2, HIGH); break;
      }
      inicio = millis(); ativo = c != '0';
      Serial.print("Comando: "); Serial.println(c);
    }
  }
  if (ativo && millis() - inicio >= 300) {
    pararTudo(); ativo = false; Serial.println("Parado");
  }
}
