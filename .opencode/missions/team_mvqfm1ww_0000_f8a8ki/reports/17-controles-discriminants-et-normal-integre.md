# 17 — Trois régressions détectées séparément, suite normale intégrée verte

## Résultat

Les trois contrôles prévus détectent chacun une régression différente réintroduite
dans l'update : recherche CPU par nom, résolution native, extraction type-erased.
Après chaque expérience, le header a été restauré à son empreinte initiale.
Le candidat final n'inclut aucun mutant ni changement de production supplémentaire.

Le runner lead a ensuite reconstruit entièrement la configuration normale et
exécuté les contrôles convenus : découverte 579 noms uniques, 504 tests CPU et
75 OpenGL passants. ASan final et audit indépendant des tests restent à produire.

## Contrôles causaux exécutés par le lead

- Nominal avant expériences : 20 tests PreparedFloatUniform passants.
- C : véritable getUniform réintroduit dans set, trois updates et trois uploads
  conservés ; compteur CPU 10 au lieu de 4, test en échec attendu, CTest 8.
- L : glGetUniformLocation réintroduit dans set ; attente Times(0) dépassée
  successivement une, deux et trois fois, test en échec attendu, CTest 8.
- A : getValue<GLfloat> réintroduit ; compteur extractions 4 au lieu de 1,
  test en échec attendu, CTest 8.

Chaque variante a compilé avant le test ; commandes/diffs/JUnit/binaire et source
empreintés séparément. Ces mutations démontrent la sensibilité des témoins, pas
un nouveau RED historique ni un gain de performance.

## Dépendances de build : essai exclu et correction de méthode

Le premier essai L utilisait encore le binaire C : Ninja annonçait « no work to
do », empreinte EXE identique et échec du compteur C, pas de L. Cet essai est
conservé mais EXCLU de la preuve L. Diagnostic discriminant : ninja -t deps sur
l'objet PreparedFloatUniformTests donne zéro dépendance, malgré un header plus
récent. Le log contient les inclusions MSVC localisées ; leur causalité précise
n'est pas corrigée ni proclamée ici.

Le lead a retiré seulement cet objet généré pour forcer les compilations L puis A,
obtenu des binaires distincts et les échecs attendus. Pour le final, clean-first
normal a reconstruit les 69 étapes avec exit 0. ASan sera aussi reconstruit
entièrement. Aucun fichier CMake, toolchain, profil ou harness modifié : le défaut
d'incrémentalité reste une limite documentée, pas une réparation livrée.

Un chemin CTest supposé sous le venv n'existait pas ; aucune commande n'avait
démarré. Get-Command a identifié C:/Program Files/CMake/bin/ctest.exe, utilisé
ensuite. Ce constat n'est ni un test RED ni une panne de Context Mode.

## Contrôles normaux finaux réellement exécutés

- Script build-windows Debug Jobs1 TestLabel unit : exit 0, 504/504 après clean.
- CTest --test-dir out/build/windows-msvc-debug -N : exit 0, 579 découverts.
- Découverte supplémentaire --show-only=json-v1 : 579 noms uniques.
- CTest label unit, --output-on-failure --no-tests=error -j1 : exit 0, 504/504.
- CTest label opengl avec les mêmes options : exit 0, 75/75.

Les commandes CTest de run ajoutent uniquement --output-junit vers un chemin
unique. Métadonnées et logs sous out/qa-prepared-float/lead-runner-20261027/ :
préfixes 06,07b,08 pour les mutants valides ; 07 exclu comme binaire périmé ;
09 pour clean-first ; 10–14 pour les contrôles finaux normaux. Les XML et noms
seront encore recoupés ensemble avec ASan et l'identité finale avant audit.

Source candidat : 3d0e1452cd9bd7b126b5fd1e43c73a5d0b1b6e23 ; production
identique au snapshot relu indépendamment, seulement deux tests et assertions QA
ajoutés depuis H2. SHA256 header restauré :
a791eb6bb0ebd092112e3557a00a41416d59baf3df48455d738e9650e2b1c02d.

## Suite

ASan préflight/clean-first/suite complète sous bail exclusif, puis audit fresh
hors board avec contrat/candidat/preuves neutres. Enfin photographie documentaire
bornée au livré. Aucun nouveau GO ou changement d'architecture demandé.
