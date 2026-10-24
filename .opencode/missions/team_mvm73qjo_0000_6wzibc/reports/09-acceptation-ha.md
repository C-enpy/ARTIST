# 09 — Contrat H_A accepté, avant exécution

Date : 2026-10-24. Étape : contrat de preuve accepté ; nouvelle présentation de politique attendue.

## Décision et changement

L'opérateur a choisi **« Sonde ciblée + close différé »** après le [rapport 08](08-proposition-frontiere-f8.md). Ce rapport constitue le contrat H_A accepté, sous réserve de son commit avant auteur.

Le lead a persisté cette décision dans `verification.acceptance` du [brouillon de mission](../model-policy.json) : calibration Windows OS→ifstream par verrou de plage sans nouvelle seam/API/infrastructure ; fermeture réellement échouée DEFER explicite, pas de clôture globale F8 ; plan QA et H_A commis avant auteur, fixture exclusivement possédée, INCONCLUSIVE si précondition ou expérience non discriminante.

QA reste `proportionate` ; pas de commandes fictives ni de résultats anticipés. Le retrait de Cursor du rapport 07 reste présent. Le catalogue, ses métriques et le seuil ne sont pas changés à cette étape. Le schéma installé accepte `verification.acceptance` comme tableau de chaînes non vides ; aucun champ arbitraire ajouté.

## État des preuves et barrières

Aucun essai/build/test ou correctif effectué. L'acceptation n'est pas une preuve de faisabilité, ni une autorisation d'étendre l'API si la sonde échoue. H_A accepté mais non commité ; H_Q toujours absent ; sélection des 17 GL toujours absente. Aucun auteur débloqué, aucune tâche d'implémentation déclarée complète.

## Suite recommandée

Faire représenter la politique corrigée par `/ensemble-mission config`, puis approbation native utilisateur par `/ensemble-mission start`. Le commit ciblé H_A et le plan QA restent nécessaires avant les auteurs, sans embarquer le diff GrepAI préexistant. Aucune nouvelle question QA par tâche : niveau et frontière ont été décidés.
