# TP1 : Programmation Système - Mini-GLIBC & Commandes Shell

Ce projet implémente une bibliothèque standard C minimale (`mini_memory`, `mini_string`, `mini_io`) sans utiliser la GLIBC, ainsi qu'une suite de commandes système (`touch`, `cp`, `cat`) basées sur cette bibliothèque.

L'objectif est de gérer manuellement la mémoire, les chaînes de caractères et les entrées/sorties bufferisées via des appels système directs (`brk`, `read`, `write`, `open`, `close`).

## Architecture du Projet

- **`lib/`** : Contient la bibliothèque.

  - `mini_memory.c` : `malloc` (liste chaînée), `free`, `calloc`.
  - `mini_string.c` : Manipulation de chaînes et conversion.
  - `mini_io.c` : Gestion des fichiers bufferisée (`MYFILE`).

- **`cmds/`** : Contient les Commandes.
  - `main.c` : Suite de tests unitaires.
  - `mini_touch.c` : Création de fichier.
  - `mini_cp.c` : Copie de fichier.
  - `mini_cat.c` : Affichage de fichier.
  - `mini_echo.c` : Affichage d'arguments.
  - `mini_head.c` : Affichage du début de fichier.
  - `mini_tail.c` : Affichage de la fin de fichier.
  - `mini_clean.c` : Remise à zéro d'un fichier. mini_grep
  - `mini_grep.c` : Affichage des lignes d'un fichier contenant un mot donné.
  - `mini_wc.c` : Affichage du nombre de mots d'un fichier.
  - `mini_ls.c` : Affichage des fichiers d'un dossier.
  - `mini_chmod.c` : Changement des droits d'un fichier.
  - `mini_ln.c` : Création d'un lien symbolique.
  - `mini_quickdiff.c` : Affichage des différence entre deux fichiers.
  - `mini_mv.c`: Déplcament d'un fichier vers un dossier cible.

* **`Makefile`** : Script de compilation automatisé.
* **`Fichiers Executable`** : Les fichiers commandes compilés (après avoir make).

## Compilation

J'utilise un Makefile pour gérer les dépendances et l'édition de liens.
Pour tout compiler (librairie + exécutables) :

```bash
make
```

Cela génère les exécutables : app.exe, mini_touch, mini_cp, mini_cat.

Pour nettoyer le projet (fichiers objets):

```bash
make clean
```

Pour nettoyer le projet (objets + executable):

```bash
make fclean
```

Pour rebuild le projet :

```bash
make rebuild
```

## Protocole de Test (Guide d'évaluation)

### 1. Tests Unitaires & Bibliothèque (app.exe)

app.exe exécute une batterie de tests sur l'allocation mémoire, la réutilisation des blocs (malloc/free), la gestion des chaînes et les IO bufferisées. Commenter/Décommenter les tests à lancer dans le main.

```bash
./app.exe
```

**Commenter la ligne `#define DEBUG` dans `mini_string.c` & `mini_memory.c`**

```C
//#define DEBUG
```

Puis rebuild le projet

```bash
make rebuild
```

### 2. Test des Commandes Système

A. Commande mini_touch
Création d'un fichier avec mini_touch et vérification de son utilisation.

```bash
./mini_touch test_file
ls -l test_file
echo "Donnees" > test_file
./mini_touch test_file
cat test_file
```

B. Commande mini_cp

Génération d'un fichier binaire de 50 Mo puis mesure du temps et vérification de l'intégrité.

```bash
dd if=/dev/urandom of=source.bin bs=1M count=50
time ./mini_cp source.bin dest.bin
diff source.bin dest.bin
```

C. Commande mini_echo

```bash
./mini_echo test avec plusieurs mots
```

D. Commande mini_cat

```bash
./mini_cat cmds/mini_cat.c
```

E. Commande mini_head

```bash
./mini_head cmds/mini_head.c
./mini_head -n 5 cmds/mini_head.c
```

F. Commande mini_head

```bash
./mini_tail cmds/mini_tail.c
./mini_tail -n 5 cmds/mini_tail.c
```

G. Commande mini_clean

```bash
./mini_touch test_file
echo "DATA" > test_file
./mini_cat test_file
./mini_clean test_file
ls -l test_file
```

H. Commande mini_grep

```bash
./mini_grep "main" cmds/mini_grep.c
```

I.Commande mini_wc

```bash
echo "   un    deux   trois   " > test_wc.txt
./mini_wc test_wc.txt
wc test_wc.txt
```

J. Commande mini_ls

```bash
./mini_ls
./mini_ls cmds
```

K. Commande mini_chmod

```bash
./mini_touch test
./mini_chmod 777 test
./mini_ls
```

L. Commande mini_ln

```bash
./mini_touch source
./mini_ln source destination
echo "test_source" > source
gedit destination
```

M. Commande mini_quickdiff

Affiche les lignes différentes entre deux fichiers, rien si identique, gère aussi les fichiers de taille différentes.

```bash
./mini_quickdiff <file1> <file2>
```

O. Commande mini_quickdiff

```bash
mkdir dossier_test
./mini_touch fichier_test
./mini_mv fichier_test dossier_test
```

## Réponses aux Questions du Sujet

### Exercice 20 : Pourquoi cette boucle dans mini_scanf ?

Problème : L'entrée standard (stdin) est bufferisée par ligne. Si l'utilisateur saisit une chaîne plus longue que le buffer fourni à mini_scanf, les caractères excédentaires restent dans le flux d'entrée. A l'appel suivant de lecture, le programme "aspirera" ces résidus au lieu d'attendre l'utilisateur. Solution : Si le buffer est plein et ne finit pas par \n, on lance une boucle de "nettoyage" qui consomme les caractères restants un par un jusqu'à trouver le \n ou EOF. On a donc un flux propre pour la prochaine lecture.

### Exercice 22 : Problèmes de sécurité de mini_strcpy et mini_strcmp

Ces fonctions sont vulnérables aux Buffer Overflows parce qu'elles ne prennent pas en paramètre la taille maximale du buffer de destination. Elles se regardent uniquement la présence du caractère nul \0 dans la source. Donc le risque est que si la chaîne source est plus longue que la destination, la fonction écrase la mémoire adjacente (variables, adresses de retour), ce qui peut mener à des crashs (Segfault) ou à des failles d'exécution de code arbitraire. Pour résoudre cela il faudrait imposer une taille limite.

### Exercice 29 : Stratégie de lecture bufferisée (mini_fread)

La structure MYFILE ne contient pas de champ pour la quantité de données valides présentes dans le buffer de lecture. Donc on ne peut pas distinguer facilement si le buffer contient 2048 octets valides ou seulement 10 (cas de la fin de fichier). Mon implémentation de mini_fread recharge le buffer uniquement lorsque l'index de lecture atteint la taille totale du buffer ou lors de l'initialisation. Cela impose une lecture par blocs complets.

### Exercice 40 : Gestion du binaire dans mini_cp

Lors de la copie de fichiers binaires, l'octet 0xFF (255) est fréquent. Si mini_fgetc retourne cet octet stocké dans un char (signé), il vaut -1. Ce qui correspond à la valeur de retour -1 qui veuet aussi dire la fin de fichier (EOF) ou une erreur. Il faut donc caster le caractère lu en unsigned char au moment de le retourner, et le 255 est vraiment retourné comme un 255 et pas -1.
