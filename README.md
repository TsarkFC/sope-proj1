-À partida todas as features estão ok, segundo aquilo que por nós foi testado (todas foram implementadas)

-Nos computadores onde o programa foi testado obtivemos sempre os mesmos resultados

-No princípio tentámos uma abordagem com fork() + exec(), mas encontrámos algumas dificuldades, tendo 
mudado a abordagem para fork() + recursividade a qual se revelou mais simples.

-Relativamente ao envio de sinais após ser recebido um SIGINT é pedido ao utilizador que envie ou 
SIGTERM ou SIGCONT através da linha de comandos para um processo com PID indicado no ecrã.
Caso o utilizador não realize qualquer ação no minuto seguinte ao envio de CTRL-C (SIGINT) o programa 
continua. 

