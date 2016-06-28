# MPEG-TS-LOADER

COMPILAR
---------------------------

No Linux
- Acesse a pasta pelo terminal e digite "make"
 
No Windows
- Configure seu compilador para acessar os sources na pasta "src" e os cabeçalhos na pasta "include".


EXECUTAR
---------------------------

No Linux
- Acesse a pasta pelo terminal e execute o comando "make run". Caso deseje carregar um arquivo .ts diferente, utilize o comando: "make run nome_do_arquivo"
 
No Windows
- No console execute o comando "start /bin/tsloader.exe" ou "start /bin/tsloader.exe nome_do_arquivo" para um arquivo .ts diferente. Também é possível executar diretamente compiando o arquivo video.ts para pasta do executável.

MODO DE USO
---------------------------

Após carregado o arquivo, será exibido os dados presentes no primeiro pacote TS do arquivo. Para acessar os demais pacotes basta pressionar ENTER para que o próximo pacote seja carregado e exibido.

OBS: A tabela PAT é apresentada no primeiro pacote e a tabela PMT no segundo.
