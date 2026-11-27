# Testes individuais

Grave `teste_motores/teste_motores.ino` e abra o Monitor Serial em 115200 baud. Com rodas suspensas e garfo sem carga, envie um comando por vez. Cada pulso dura 300 ms.

| Comando | Resultado validado |
|---|---|
| 1 | Esquerda ré |
| 2 | Esquerda frente |
| 3 | Direita ré |
| 4 | Direita frente (após troca do jumper) |
| 5 | Garfo sentido 1 |
| 6 | Garfo sentido 2 |
| 0 | Parada |

Depois grave o firmware Bluetooth e teste habilitação, locomoção, X/B, soltura dos botões, desabilitação e perda de conexão com rodas suspensas. Não considere o teste de segurança aprovado até verificar fisicamente a parada em todos esses casos.
