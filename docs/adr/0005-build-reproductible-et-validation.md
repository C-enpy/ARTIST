---
title: "ADR-0005 — Baseline de build reproductible et preuves graduées"
adr_id: ADR-0005
category: adr
tags: [artist, build, testing, adr]
aliases: [ADR-0005]
status: Accepted
implementation: partial
decision_origin: Demande utilisateur de build complet le 2026-10-03, avec réévaluation explicite de Conan 2.
summary: "Accepted/partial : baseline et QA Windows/Linux prouvées ; coverage ON ou rejet ciblé exécuté non prouvé, sans exiger une matrice de plateformes idéale."
created: 2026-10-03
updated: 2026-10-29
audit_date: 2026-10-29
sources: [docs/_meta/session-2026-10-29-linux-qa.json, research/sessions/2026-10-29-linux-qa-session/index.json, .opencode/evidence/mission-linux-qa/final-test-reaudit.md, .opencode/evidence/adr-audit-2026-10-29/inputs.json, .opencode/evidence/adr-audit-2026-10-29/public-deliveries.json]
provenance:
  extracted: 1.0
  inferred: 0.0
  ambiguous: 0.0
---

# ADR-0005 — Build reproductible et validation

## Audit au 29 octobre 2026

**Accepted / partial** confirmé pour un critère précis : l'exécution avec
coverage ON sur GNU, **ou** l'exécution du rejet ciblé prévu sur une toolchain
non supportée, n'est pas établie par les preuves retrouvées. Le guard et ses
flags sont observés (`CMakeLists.txt:26–32`), pas validés par leur seule présence.
Coverage OFF, découverte, tests/contextes/fixtures, versions/ABI et reproduction
aux environnements consignés disposent de preuves. Linux matériel, Release et
verrou complet des révisions Conan ne sont pas inventés comme conditions de
passage de la baseline à verified.
[[references/adr-audit-2026-10-29#ADR-0005|Audit de toutes les obligations et critères]].

## Preuve complémentaire Linux — 29 octobre 2026

**Accepted/partial inchangé.** Six configurations Windows, Ubuntu WSL et CI
Ubuntu exécutent chacune les mêmes 589 cas au runtime `78e7b74`, normal/ASan,
sans failure/error/skip ; Conan2, CMake/CTest et fixtures mutualisés. Le contrôleur
`f58a49f` inclut ensuite les shaders dans ses identités après RED/GREEN Python
10/10 sur chaque OS et ré-audit indépendant. Quatre nouvelles campagnes WSL/CI
normal/ASan passent chacune 589/589 avec 169 empreintes dont 20 shaders ; Windows
est réutilisé par identité, pas rebaptisé run du nouveau contrôleur.

La [[references/session-2026-10-29-linux-qa|session et ses quatorze sous-agents]]
attribue propositions, revues en lecture seule et runners. L'audit avait identifié
une omission de couverture d'identité, pas une modification shader démontrée
pendant les campagnes précédentes. Les premiers FAIL CI/WSL et la provenance RED
classification incomplète restent archivés. Ingestion documentaire, aucun test
relancé. Voir [[development/linux-qa|commandes, candidats distincts et limites]].
Pas de qualification universelle des toolchains, Release, fuites ou performance.

## Preuve complémentaire Windows Debug — 2026-10-26

Ingestion du27octobre : [[references/session-2026-10-26-stable-reader|sources des
sous-agents et retour d'expérience]]. Distinguer comptage, identité de population,
preuve exécutée, revue statique et clôture du board ; les nouveaux exports ne sont
ni une nouvelle campagne ni une validation native du harness. Statut inchangé.

**Accepted/partial inchangé.** Nouveau candidat Reader stable `a96104c` :
555/555 normaux puis555/555 ASan (484CPU+71OpenGL chacun), zérofailure/error/skip,
sélections pré-run et identités recoupées avec JUnit/LastTest ;17historiques une
foisASan, diagnostics tardifs contrôlés. Trois tests retirés/trois ajoutés :
cardinalité identique n'est pas identité de population historique. PréflightASan
antérieur réutilisé après contrôle d'identité, pas nouvelle installation/config
propre ni validation Release/GCC/Linux/CI. Preuve
`_meta/session-2026-10-26-stable-reader-cleanup.json` et
[[development/build-and-test|commandes réellement exécutées]].

## Contexte et faits

Le `CMakeLists.txt` racine télécharge conditionnellement `CodeCoverage.cmake`
mais l'inclut toujours. `conanfile.txt` et `conan_cmake_run` appartiennent au flux
Conan 1 ; la CI déclare 1.64.0, alors que l'hôte du bootstrap dispose de Conan 2.
`enable_testing()` est local aux CMake de tests. Les presets x86/x64 partagent
les mêmes chemins GCC MSYS2. Voir [[development/build-and-test|le guide actuel]].

## Décision adoptée

- Restaurer une baseline avec une toolchain et une STL vérifiées. La proposition
  initiale Conan 1 est remplacée par [[adr/0008-conan2-et-baseline-windows|ADR-0008]]
  après installation réelle des cinq versions exactes avec Conan 2.
- Rendre coverage véritablement optionnel : téléchargement, include et flags
  conditionnels et diagnostics explicites en cas d'échec.
- Déclarer la découverte des tests au niveau approprié afin qu'une commande CTest
  de référence recense toutes les suites voulues ; conserver une sélection CPU/GPU.
- Définir des presets honnêtes sur compilateur, ABI et configuration. Une cible
  x86 non réellement exercée est retirée de la matrice validée ou rendue effective.
- Séparer les arbres par toolchain/configuration/plateforme. Produire une vraie
  base de compilation pour l'outillage lorsqu'elle est supportée.
- Rendre les préconditions GPU et fixtures explicites. Un échec d'initialisation
  ne doit pas continuer comme si le contexte existait ; distinguer test en échec
  et indisponibilité d'environnement selon le contrat de la suite.

La migration Conan 2 est réalisée dans le périmètre d'ADR-0008. Un verrouillage
complet des révisions de recettes et une matrice de plateformes restent à instruire.

## Alternatives et conséquences

Un unique grand changement build + langage + architecture rend les régressions
difficiles à attribuer. Un cache prérempli ou coverage activé par détour masque
le défaut d'une configuration propre. Une baseline isolée coûte une étape, mais
permet des comparaisons et diagnostics crédibles.

## Migration

Corriger CMake et fixtures en diff limité, exécuter CPU sur une configuration
propre, puis GPU réel lorsque disponible. Mettre à jour les commandes du guide
et la CI d'après les résultats, sans revendiquer toutes les plateformes.

## Critères de validation

- Configuration propre coverage OFF sans fichier coverage préexistant.
- Configuration coverage ON sur une toolchain supportée, ou erreur ciblée claire.
- Build de `ArtistLib_tests` et découverte d'un nombre non nul de tests ;
  exécution avec `--no-tests=error` et nombre de skips rapporté.
- Pour `ArtistLib_component_tests` : contexte réel identifié, fixtures vérifiées
  et résultat indépendant du succès des mocks.
- Toolchain/ABI enregistrées et reproduction depuis un arbre propre.

**Preuves du 2026-10-03** : baseline Windows Debug complète, coverage OFF,
109/109 tests réussis dont 22 sur OpenGL NVIDIA réel. Découverte racine, fixtures,
préfixes de suites et chemins d'échec du contexte corrigés. Voir
[[development/build-and-test|les commandes et preuves]]. `implementation: partial`
conserve le critère coverage non prouvé ; les plateformes désormais exercées sont
qualifiées séparément ci-dessus, sans exiger toutes les plateformes possibles.

## Références

[[architecture/correction-plan|Étape zéro]] · [[development/build-and-test|Commandes]] ·
[[adr/0006-preuves-performance-et-determinisme|Mesures]] · [[adr/README|Registre]].
