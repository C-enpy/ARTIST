# QA Linux — livraison bornée et PR ouverte

## Livraison

La mission technique est terminée dans le périmètre approuvé. **589 tests passent
sur Windows, Ubuntu WSL et Ubuntu CI, normal et ASan**, 513 CPU et 76 OpenGL,
sans échec, erreur ou skip. Les deux findings indépendants ont été corrigés après
RED/GREEN puis fermés par ré-audit. Le contrôleur final passe 10/10 tests Python
sur chaque OS ; ses campagnes Linux renforcées vérifient 169 empreintes dont 20 shaders.

La [PR nº3](https://github.com/C-enpy/ARTIST/pull/3) est ouverte **en brouillon**,
empilée sur la référence exacte de départ de cette mission. Elle exclut ainsi
les anciennes missions non encore intégrées à main. Aucun merge distant ni push
de main : son hash distant est inchangé au dernier contrôle. Le retarget vers main
attend l'intégration séparée de cet historique, hors mandat Linux.

## Fichiers et preuves utiles

- Procédure : `docs/development/linux-qa.md`, commandes communes et différences
  Windows/Linux ; photographies build-and-test et current-state mises à jour.
- Preuve portable : `docs/_meta/session-2026-10-29-linux-qa.json`.
- Production bornée : `AttributeReader.hpp` ; clés natives legacy intactes,
  classification moderne conditionnelle avec annonce effective GLEW.
- Build/CI : `scripts/build-linux.py`, `scripts/linux-evidence.py`, profil/presets
  Linux, `cmake/AddressSanitizer.cmake`, workflow et tests du contrôleur.
- [CI finale 40841460327](https://github.com/C-enpy/ARTIST/actions/runs/40841460327),
  deux jobs réussis, artefacts téléchargés et recoupés par le lead. Les résultats
  Windows au runtime78e7b74 sont réutilisés par identité, pas rebaptisés runsf58a49f.
- Revues : `.opencode/evidence/mission-linux-qa/final-test-audit.md` et
  `final-test-reaudit.md`. Rapports28 et preuve portable détaillent les commandes
  réellement exécutées et l'attribution runner/reviewer.

## Vérification de clôture

Diff intégré complet depuis la base de mission : diffcheck propre. Métadonnées PR
et liste de chemins recoupées avec Git ; base et source sont exclusivement des
branches de mission. Les derniers commits de rapports sont marqués skip-ci car
ils ne modifient ni runtime, shaders, tests exécutables, configuration ou helper
depuis les campagnes qualifiées. Aucun contrôle requis d'un changement exécutable
n'est sauté.

Documentation :8frontmatters/tags et153liens vérifiés, preuveJSON/hashs vérifiés,
QMD artist-wiki seul actualisé, nouvelle note retrouvée et lue. Les travaux locaux
préexistants sont conservés : staging partiel des trois notes déjà modifiées,
preuve des segments exclus et fichiers de travail inchangés par ce staging.

Le runtime Ensemble ne permet pas de clôturer une tâche de plan deferred sans
la publier. Pour éviter un nouvel audit/auteur inutile, deux anciennes lignes
audit/documentation restent **inactives** après leur livraison réelle. Board
observé :11completed,1pending,1blocked, ces deux dernières lignes deferred.
Ce reliquat administratif n'est pas un audit manquant, ni une complétion fabriquée.
Aucun worker actif au contrôle final ; équipe et worktrees préservés sans cleanup.

## Limites conservées

Linux utilise Mesa llvmpipe, pas un GPU matériel. Pas de Release, UBSan, coverage,
performance ou recherche exhaustive de fuites ; detect_leaks=0. Dépendances
système/Conan non réinstrumentées. Premier delta RED classification non hashé
intégralement à chaud ; échecs incrémentaux Windows et cause de dépendances encore
incomplètement expliquée conservés. OpenGL3.3 ambigu sans route moderne non
universellement qualifié. Deux relevés d'identité, pas isolation transactionnelle
contre un changement transitoire annulé. Aucun statut ADR global promu.

L'acceptation est donc celle des obligations exécutables bornées de cette mission,
pas une certification exhaustive du moteur, de tous les pilotes ou du harnais.
