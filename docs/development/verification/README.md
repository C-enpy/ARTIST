---
title: Vérification — guide développeur
category: development
tags: [artist, build, testing, guide]
summary: "Point d'entrée pour utiliser le pipeline de vérification, lire ses preuves et préparer l'ajout de scénarios dans une nouvelle session."
created: 2026-10-30
updated: 2026-10-30
sources: [scripts/verify.py, scripts/verification/catalogue.py, docs/development/verification-pipeline.md, ARTIST/verification/OpenGLFloatWorkload.cpp]
provenance:
  extracted: 1.0
  inferred: 0.0
  ambiguous: 0.0
---

# Vérification — guide développeur

Ce dossier est le point d'entrée pour **utiliser le pipeline et développer ses
scénarios**, y compris avec un agent dans une nouvelle session.
Les guides sous [[usage/getting-started|usage/]] concernent plutôt la consommation
de la bibliothèque ARTIST ; ici, il s'agit de son infrastructure de vérification.

- **Utiliser un scénario** : parcours ci-dessous, puis
  [[development/verification-pipeline|guide opérateur de qualification]].
- **En implémenter un** : [[development/verification/adding-scenarios|procédure d'ajout, exemple CTest et brief de reprise]].
- **Retrouver les preuves et décisions initiales** :
  [[references/session-2026-10-30-uniform-pipeline|session et sous-agents]].

> [!warning] Livré ne signifie pas qualifié nativement
> Le pipeline a des preuves synthétiques et des revues bornées. Son exécution
> native Windows/Linux et les mesures de performance restent à qualifier.
> Lire une commande dans ce guide n'autorise pas une installation, une campagne
> GPU, un workflow ou une réservation cloud.

## 1. Choisir ce qu'on veut prouver

Les cinq scénarios actuels sont listés dans `scripts/verification/catalogue.py` :

- `cpu.float-mirror` : miroir de valeur CPU, **pas un upload GPU**.
- `gl.float-routes-correctness` : voies classique et préparée, readback et pixels.
- `gl.float-preparation` : sélection exacte des tests de préparation.
- `gl.float-lifetime` : sélection exacte des tests de contexte et durée de vie.
- `gl.float-submission-cpu` : coût CPU de soumission, **Release seulement**.

Pour la comparaison float, la référence est l'API actuelle du même candidat :
`pass.withUniform(scalarName, value)` contre `access.set(value)` après
`prepareFloatUniform`. Le workload `OpenGLFloatWorkload.cpp` utilise le même
kernel update+draw et les mêmes valeurs préparées pour les deux voies.
Les données sont **produites lors des futures exécutions**, pas tirées d'une
table historique ou d'un autre moteur. Les coûts de préparation sont consignés
séparément ; `cpu_wall_submission` n'est pas une durée GPU.
Le chronomètre couvre le kernel update+draw avec ses gardes de production, pas
uniquement l'appel du setter d'un uniform.

## 2. Consulter sans exécuter

Depuis la racine du dépôt, avec un Python préparé (`python3` si nécessaire sous
Linux) :

```text
python -B scripts/verify.py list --json
python -B scripts/verify.py plan --platform windows --profile cpu-only --configuration debug --scenario cpu.float-mirror
```

`list` et `plan` ne construisent/lancent aucun workload, ne sondent aucun GPU et
ne chargent pas les services de plateforme. Un plan décrit une intention ; ses
entrées `unresolved` ne constituent pas des préconditions déjà satisfaites.

Pour préparer une comparaison matérielle, exemple **consultatif** :

```text
python -B scripts/verify.py plan --platform windows --profile opengl-hardware --configuration release --scenario gl.float-submission-cpu --route pass-name --route prepared-float --iterations 1000 --repetitions 5 --batch-size 16 --measurement-requested --expected-gl-vendor VENDOR_OBSERVE --expected-gl-renderer RENDERER_OBSERVE
```

Remplacer les deux placeholders par les chaînes exactes de la machine avant une
campagne réelle. Les nombres illustrent la syntaxe, **pas un protocole statistique
qualifié ni une durée suffisante garantie**. Les lots actuellement admis sont
1, 16, 256. Une mesure exige un calibrage et une politique de bruit explicites.
Le pipeline conserve les échantillons de chaque route ; il ne transforme pas
automatiquement leur présence en un verdict « la voie préparée est plus rapide ».
Fixer avant campagne l'hypothèse, le seuil, l'ordre A/B, l'échauffement et le
traitement de la dispersion. La simple sélection des deux routes n'organise pas
automatiquement une alternance statistiquement qualifiée.

## 3. Qualifier l'environnement, puis exécuter les phases séparées

Suivre les préconditions et commandes du [[development/verification-pipeline|guide opérateur]] : Python, Conan2, CMake/Ninja, compilateur/STL C++23 et contexte réel
si GL. Windows GL logiciel n'est pas supporté ; Linux llvmpipe ne mesure pas le
GPU matériel cible. Un libellé de runner ne prouve pas le renderer observé.

Ordre strict : **prepare → build → run → report**. Chaque phase est une invocation
distincte. `prepare` exige l'opt-in de résolution de dépendances ; il peut avoir
des effets réels. Garder le chemin exact du bundle retourné et ne poursuivre
qu'avec un reçu précédent compatible réussi, sur le même candidat et la même
machine. `report` relit les preuves, sans réparer/reconstruire/réexécuter.

Une modification des sources, fixtures ou shaders après préparation impose une
nouvelle tentative ; ne pas copier ou retoucher le bundle pour le rendre valide.
Un seul build par arbre et exécutions GL partageant les ressources sérialisées.

## 4. Lire un résultat sans extrapoler

Le bundle contient notamment plan/manifeste, identités, reçus, environnement,
commandes, journaux et résultats bruts. Lire séparément :

- `pipeline_state` : orchestration terminée, bloquée ou en panne ;
- `test_verdict` : PASS/FAIL/INCONCLUSIVE/NOT_EXECUTED ;
- `measurement_validity` : NOT_REQUESTED/ABSENT/VALID/INVALID ;
- `freshness` : CURRENT/STALE/UNVERIFIED.

Un code0 de `plan` n'est pas un test réussi ; un test PASS n'est pas une mesure
valide ; une mesure valide n'établit pas à elle seule un gain reproductible.
Zéro test, contexte absent, mesure manquante ou shader non attribuable ne valent
jamais PASS. Conserver première cause et logs bruts, pas seulement un résumé.

## 5. Préparer la session suivante

Pour ajouter un scénario, partir du [[development/verification/adding-scenarios#Brief à transmettre à un agent|brief copiable]], définir l'oracle et les droits d'exécution avant le code. Ne pas
réouvrir l'ancienne équipe clôturée. Une nouvelle mission Ensemble suit le cycle
de préparation/configuration puis `start` demandé par l'utilisateur selon AGENTS.
Les statuts [[adr/README|ADR]] restent inchangés ; ce dossier n'ajoute ni backend
ni autorisation implicite de benchmark.

## Vérification de cette documentation

Le 30 octobre 2026, les trois commandes consultatives ci-dessus ont été exécutées
par le coordinateur avec Python3.13 : catalogue de cinq scénarios et deux plans
valides, codes0. Les placeholders matériels sont restés des chaînes d'exemple :
aucun renderer n'a été sondé. YAML, liens et structure AST de l'exemple d'ajout
ont été contrôlés ; ce nouvel exemple n'a été ni enregistré ni exécuté.
Preuves : `out/documentation/verification-developer-guide-2026-10-30/`.
Ce contrôle documentaire ne vaut ni build, ni test moteur, ni campagne GL/performance.
