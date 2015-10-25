O projeto contem 9 códigos-fontes:
servidor.cpp contem o processo servidor de execução de processos

executa_postergado.cpp contem o processo de solicitação de execução
lista_postergado.cpp contem o processo que lista a estrutura que contem os processos postergados
remove_postergado.cpp contem o processo de remoção de processo escalonado para execução postergada
shutdown_postergado.cpp contem o processo utilizado para desativar o servidor de execução postergada

comunication.cpp e comunication.hpp contem a biblioteca utilizada para a comunicação entre o servidor e os processos
task.cpp e task.hpp contem a biblioteca utilizada para o controle de tarefas no servidor

Obs.1: cada código-fonte é compilado individualmente utilizando makefiles próprios.

Obs.2: também está incluso o arquivo teste.cpp que, após compilado, foi utilizado para fazer testes no gerente de processos.