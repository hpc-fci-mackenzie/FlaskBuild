# FlaskBuild
* Este projeto utiliza o Algoritmo de Seleção Negativa (ASN) para encontrar Flask Bulds.
* Este projeto utiliza CMake para sua configuração.

# Instruções
É recomendável criar um diretório específico para seu build.
```
$ mkdir build
$ cd build
```
Estando nele, você deve executar um desses dois comandos:
* Para configurar um ambiente de compilação para o compilador da ```Intel```:
```
$ cmake ..
```
* Para deixar o script de configuração decidir o melhor ambiente para você:
```
$ cmake .. -DORIGINAL=OFF
```
Feito uma dessas duas operações, o próximo passo agora é gerar os binários:
```
$ make
```
Pronto. Agora os binários devem estar gerados nas pastas especificadas pelo ```Makefile```

Para executar, dado o setup exemplificado, basta inserir algum desses comandos, em qualquer ordem:
```
$ ./A/barra < ../A/in.1
$ ./B/soma < ../B/in.1
```

# Citação
Para citar o projeto, utilize uma das formas abaixo:
```
J. C. de Lima Costa, L. N. de Castro and C. de Paula Bianchini, "Sensitivity Analysis of the Negative Selection Algorithm Applied to Anomalies Identification in Builds," 2019 XLV Latin American Computing Conference (CLEI), Panama, Panama, 2019, pp. 1-6, doi: 10.1109/CLEI47609.2019.235087.
```

```
@INPROCEEDINGS{9073958,
author={J. C. {de Lima Costa} and L. N. {de Castro} and C. {de Paula Bianchini}},
booktitle={2019 XLV Latin American Computing Conference (CLEI)},
title={Sensitivity Analysis of the Negative Selection Algorithm Applied to Anomalies Identification in Builds},
year={2019},
volume={},
number={},
pages={1-6},}
```
