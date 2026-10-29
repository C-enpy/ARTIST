---
title: "ADR-0008 — Conan 2 et baseline Windows reproductible"
adr_id: ADR-0008
category: adr
tags: [artist, architecture, build, testing, adr]
aliases: [ADR-0008]
status: Accepted
implementation: verified
decision_origin: Demande utilisateur de build complet, réévaluation de Conan 2 et autorisation de moderniser les toolchains le 2026-10-03.
summary: Migrer vers Conan 2 sans changer les versions des dépendances et établir une baseline Windows MSVC C++23 exécutée.
created: 2026-10-03
updated: 2026-10-29
---

# ADR-0008 — Conan 2 et baseline Windows

## Preuve Linux séparée — 29 octobre 2026

La mission [[development/linux-qa|QA Linux]] apporte une preuve bornée GCC14 /
Ubuntu WSL et CI avec Conan2, distincte de la baseline adoptée ici. Le statut
**Accepted/verified reste limité à Windows Debug dans cet ADR** ; cette note
n'adopte pas une validation générale multi-plateforme, Release ou performance.

## Contexte

[[adr/0005-build-reproductible-et-validation|ADR-0005]] proposait initialement une
baseline Conan 1. L'utilisateur a demandé de réévaluer Conan 2 : les cinq versions
exactes déclarées sont désormais disponibles sur ConanCenter 2 et leur installation
réelle a réussi. Ce constat remplace l'hypothèse d'une incompatibilité actuelle.

Le GCC MSYS2 13.2 présent n'expose pas `std::stacktrace`. MSVC 14.44/19.44 a passé
une sonde de compilation, linkage et exécution de `std::format`/`std::stacktrace`.
La présence de toolchains plus récentes n'est pas, à elle seule, une validation.

## Décision adoptée

- Conan **2.12.2**, `CMakeDeps` et `CMakeToolchain` ; installation avant CMake.
- Conserver `glm/cci.20230113`, `glfw/3.3.8`, `glew/2.2.0`, `opengl/system` et
  `magic_enum/0.9.5`. Aucun changement de format/runtime motivé par cette migration.
- Baseline exécutée : Windows x64, MSVC **14.44**, standard public **C++23**.
- Tooling/cache locaux sous `out/`, profil versionné et script restaurant l'environnement.
- Dépendances et exigence C++23 portées par la cible `ArtistLib` ; sonde des
  fonctionnalités STL à la configuration, coverage optionnel, CTest à la racine.
- Conserver les prédicats de validation C++ : désambiguïsateurs `template`,
  concept nommé pour la contrainte d'`Uniform`, constantes littérales portables.
- Tests de fixtures depuis la racine source et noms/sérialisation explicites.

## Alternatives et conséquences

Conserver Conan 1 ajouterait une chaîne obsolète alors que le graphe exact est
installable en Conan 2. Installer un compilateur expérimental sans vérifier sa
STL ne résout pas ce contrat. MSVC 19.50 a également reproduit le défaut réduit
de contrainte dépendante ; la forme nommée a été compilée avec MSVC et GCC.

Les anciennes commandes Conan 1 sont remplacées. Les presets x86 qui désignaient
le même GCC x64 ont été retirés. Les autres plateformes doivent repasser les
sondes et tests ; elles ne deviennent pas validées par la réussite Windows.

## Preuves et portée

**PASS Windows Debug** : le script `scripts/build-windows.ps1` termine avec le
code zéro, les deux exécutables sont liés, **87 tests unitaires et 22 tests OpenGL
passent sans skip**. Les assertions de `UniformContractTests.cpp` acceptent l'API
valide et sa classe dérivée, rejettent une API sans validator et un validator faux.

Contexte : RTX 3090, OpenGL 3.3.0, driver NVIDIA 616.92. Les logs, versions et
commandes sont dans [[development/build-and-test|le guide]] et
`_meta/build-validation.json`. La CI Linux est adaptée mais non exécutée ici.
Release, images, performances et sanitizers ne font pas partie de cette preuve.

Retour : [[adr/README|Registre]] · [[architecture/correction-plan|Plan de correction]].
