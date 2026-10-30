# 97 — RED Linux observé, modes helper ratifiés avant correction

Le coordinateur a relu les 34 tests, les fixtures, le scaffold et la proposition de surface helper, puis exécuté le seul filtre nouveau : **34 tests, 31 échecs d'assertion avec sous-cas, aucune erreur ou skip**. Empreintes stables. Helper, presets et tests historiques restent inchangés.

Les causes attendues sont atteintes : descriptions vides du scaffold, préparation sans commande et nouveaux flags positifs non reconnus. Le scaffold évite un faux RED d'import ; les rejets d'arguments inconnus déjà verts ne démontrent pas la sémantique des nouveaux modes. Plusieurs assertions de composition restent masquées tant que la préparation n'est pas implémentée.

La surface minimale est ratifiée : build-only avec cibles explicites sans configure/Conan/CTest ; option verification-workloads réservée à configure-only avec le nom CMake existant. Le défaut historique reste inchangé. Le GREEN peut traiter uniquement adaptateur Linux et ces modes, en conservant les 34 tests, les six Release et les 19 historiques.

Les collectes de versions sont une frontière séparée des descriptions, doublée dans les tests ; elles ne prouvent ni toolchain fonctionnelle ni GPU. Le serveur X reste une précondition opérateur de qualification future, pas une ressource créée par le pipeline. Aucun fichier core, CMake parent ou Windows ne sera modifié par ce propriétaire.

## Preuves

- Candidat `678b21502f6d993ed2bc5d9244caa63d9d4adc18`, auteur pool-24 ; runner : coordinateur.
- Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_linux -p test_binding_*.py -v`, worktree pool-24.
- Journaux et empreintes : `out/verification-synthetic/linux-binding-red-2026-10-30T06-35-03-147Z/`.
- Décision locale persistée : `.opencode/evidence/mission-uniform-pipeline/linux-adapter-binding-v1.md`, sans nouveau schéma ou ADR globale.

Aucun enfant réel, helper comme processus, Conan/CMake/CTest, moteur, compilation, Xvfb, OpenGL, rendu, benchmark, installation ou cloud exécuté. Linux natif reste NON_EXECUTE.
