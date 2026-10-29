# 15 — Décision déléguée : audit natif dédié, approbation native préservée

2026-10-25. L'utilisateur demande au coordinateur de décider des détails triviaux. Le lead tranche donc l'optionB du rapport14 et prépare la révision, sans nouvelle question de choix de workflow.

## Décision et différence exacte

Le brouillon courant `../model-policy.json`, verification.acceptance, est modifié pour un **test-reviewer natif fresh sur tâche dédiée avec claim**, au lieu d'une exigence littérale hors board/worktree:false que le runtime ne représente pas dans cette mission. Un worktree isolé RO est toléré si l'entrée source/snapshot autorisée et son identité sont vérifiées.

Maintien : profil restrictif effectif et transport vérifiés avant analyse ; aucun shell/test/édition/délégation ; brief contrat/candidat/preuves, sans rapports de justification auteurs ni consultation du board d'implémentation ; refus fermé si admission incompatible, aucun profil générique de remplacement ou élargissement des droits. Cette discipline de contexte ne prouve ni invisibilité technique du board ni sandbox. La limite est explicitement acceptée dans le brouillon, pas masquée par un faux « hors board ».

QA proportionate, catalogue/providers, seuils, scores et commandes de campagne inchangés. Ni F8/C2/B1/GCC ni Linux/Release/image/performance ne sont requalifiés. Aucune nouvelle campagne n'est justifiée par cette révision de workflow : les preuves H4 restent réutilisables tant que l'identité est stable.

## Mise en œuvre après approbation de la révision

Publier/affecter un audit dédié avec **agent test-reviewer explicite**, contexte fresh et entrées locales autorisées. Ne pas compter sur l'affinité conseillée en prose de l'ancienne tâche importée. Contrôler ce qui est effectivement admis ; fournir les données nécessaires sans retenter les accès worktree refusés, comme résolu pour la revue correctness au rapport12. Le test-reviewer ne reçoit pas les justifications des auteurs ni la revue correctness comme oracle.

Le lead restera runner et intégrateur ; les résultats ASan/CPU et le diagnostic13 restent disponibles. La clôture et la disposition des tâches natives attendront le vrai livrable d'audit accepté, pas une fausse complétion pour contourner le dispatch. Aucun worker nouveau n'est admis par l'édition du JSON.

## Limite non déléguable

La correction de chat est persistée, **pas approuvée nativement**. La politique du projet exige une nouvelle présentation par `/ensemble-mission config`, puis le `/ensemble-mission start` **de l'utilisateur** pour approuver cette version. Le lead ne lance pas start à sa place et ne confond pas délégation de décisions de workflow avec cette commande d'approbation.

Il n'y a donc plus de question « A ou B » à trancher. Prochaine étape opérateur uniquement : présentation/config puis start natif. Les workers déjà admis conservent leur révision ; aucun modèle forcé, grant nouveau, source ou runtime modifié.
