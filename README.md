# Multithread-Server
ULP, Sistemas Operativos, a21805862

Os ficheiros de teste existem no diretório /home/pikku/mock/
Existem 6 ficheiros, sendo 5 deles iguais e o 6º várias vezes maior do que os restantes (para testar atrasos (que só existem quando não é utilizada concurrency))

Steps:
1.  $make
2.  $./server
3.  $./clients.bash         OR          ruby client.rb {1-6}
    ($chmod u+x program_name) caso não seja permitida a execução do bash
