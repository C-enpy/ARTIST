# 150 — Couture intégrée, précondition du harnais en erreur

Le coordinateur a relu les trois nouveaux fichiers et leurs substitutions. Ils
étaient non suivis : l'auteur a interprété l'interdiction ambiguë des effets Git
du brief comme interdisant leur commit. Le lead les a persistés sans modification,
puis intégrés par shutdown/merge natifs. Les trois empreintes reçues sont identiques
à la remise. L'auteur reste pool-41 ; persistance et exécution sont celles du lead.

La syntaxe des deux fichiers Python et du driver PowerShell passe sans import ou
évaluation du candidat. Le rôle python-pip est réellement demandé par la source
Initialize-VerificationTooling même avec tooling présent : le plan est corrigé pour
le compter explicitement. Ce rôle est simulé, sans pip ni installation exécutés.

Les six baselines donnent **trois succès et trois erreurs de harnais**, sans
failure d'assertion ni skip. Empreintes stables. La source composée est refusée
par la garde de commandes avant son évaluation ; les quatre caractérisations
restent NON_EXECUTE. G1 n'est pas validé et ce refus ne constitue pas un RED produit.

Une expérience discriminante a ensuite parsé, sans l'exécuter, le programme déjà
installé d'un cas conservé. Les seuls noms refusés sont Skip-PayloadWhitespace,
Require-PayloadCharacter, Read-PayloadString et Read-PayloadStringArray. Le lead
a recoupé leurs quatre définitions imbriquées dans Read-VerificationPayloadFile
de la source pinée. Il manque ces noms internes précis à la liste du harnais,
pas une nouvelle permission de lancer un outil ou d'évaluer une commande dynamique.

## Correction bornée avant nouvelle tentative

Autoriser une correction de harnais uniquement dans g1_windows_memory_driver.ps1 :
admettre ces quatre noms exacts déjà issus de la source pinée ; garder les gardes
d'opérateur, de population/hash, de primitives et d'environnement intactes. Un
diagnostic peut donner le seul nom sûr et l'opérateur, jamais les arguments,
valeurs d'environnement ou source complète. Aucun wildcard ou découverte dynamique
de noms autorisés. Les dix tests, fixtures Python, production et pins restent
inchangés. Première réparation de harnais de cette tranche ; auteur distinct du
runner, aucun test auteur. Nouvelle relecture avant baselines puis caractérisations.

Le contrat de réparation a été committé en7a77eac avant publication de
`task_mvuys3y9_008t_2u3hxr`, build-engineer fresh/worktree isolé. PM/LLM brut55,
complexité retenue70, risque52. Un seul fichier autorisé ; commit auteur de ce
fichier explicitement permis, exécution toujours réservée au lead.

## Preuves et identités

- Fichiers auteur persistés par le lead : `eb0294dd0b550cd6922a7881c605760728261d6c`.
  Intégration pour validation : `0d837316e483bba18e4ab62858413e4d2580879e`.
- Syntaxe : `out/verification-synthetic/windows-seam-preflight-2026-10-30T12-40-03-617Z/`.
- Baselines : `out/verification-synthetic/windows-seam-baseline-2026-10-30T12-41-09-592Z/`,
  avec manifestes, stdout/stderr, hashes et command-audit-diagnostic.json.
- Commande tests : Python3.13 `-B -m unittest scripts.tests.verification_seams.test_g1_windows_helper_seam.G1BaselineTests -v`, cwd racine ARTIST.
- Diagnostic : parser AST PowerShell sur les seuls fichiers du cas conservé,
  sortie limitée aux noms/opérateurs/lignes ; pas de nouveau lancement du driver.
- Source recoupée : build-windows.ps1:214/218/225/268 (fonctions imbriquées),
  :839–842 (rôle python-pip). Deux tentatives de patch documentaire avec un
  contexte incorrect ont été rejetées sans modification, puis le hunk précis appliqué.

Aucun helper complet, outil natif, cmd/vcvars, moteur même CPU, OpenGL, rendu,
benchmark, installation ou cloud exécuté. Les19 tests Windows du rapport149
gardent leur preuve séparée ; réception finale et audit restent en attente.
