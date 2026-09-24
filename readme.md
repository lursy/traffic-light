# queue-traffic — Simulação de Semáforo com Fila

Trabalho de ECOI2204 (Algoritmos e Estrutura de Dados, UNIFEI) — Tema 2.

Simulação em terminal de uma via única controlada por semáforo: os veículos
entram no fim de uma **fila dinâmica encadeada, implementada manualmente**
(sem contêineres da STL) e são liberados em ordem FIFO quando o sinal abre.

## Como executar

```bash
make run                      # compila e executa
make debug && ./app           # versão com AddressSanitizer (verifica vazamentos)
docker compose run --rm app   # via Docker
```

## Estrutura

```
include/utils/queue.h          fila dinâmica Queue<T> (lista encadeada)
src/utils/queue.cpp            implementação da fila (instanciada para Veiculo)
include/vo/placa.vo.h          placa validada (ABC1234 / ABC1D23)
include/entities/veiculo.h     veículo: placa, tipo, ordem de chegada
include/services/semaforo.h    regras do semáforo (única fila do programa)
include/ui/menu.h              menu textual
```

Relatório técnico (descrição da solução, representação da estrutura, roteiro de
testes e análise de complexidade): [docs/relatorio.md](docs/relatorio.md).
