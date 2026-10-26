# 40 — Décision d'admission S/C après contrôle natif

2026-10-26. Étape : contrat et QA intégrés, deux branches prêtes fonctionnellement mais admission suspendue.

## Résultats acquis

- Graphe PM37 et contrat de sonde38 intégrés au commit4b0626e. Pas besoin de refaire le plan ou les enquêtes. Mêmes scores approuvés S70/66, C64/69.
- Tâches S task_mvpdowje_000b_13u1km et C task_mvpdowje_000c_1g59xr ont été auto-affectées aux anciennes sessions pool-5 et b3-contract. Contrôle session/context confirme les historiques E1/E3 et location canonique, pas des sessions fresh. Suspensions explicites envoyées avant tout GO auteur/source.
- Une tentative native fresh sans réclamer les tâches déjà affectées a été refusée : **Autonomous workers require claim_task and collaborative reporting**. Aucun nouveau worker créé, refus non contourné. task_add n'expose pas contexte/worktree/réaffectation dans ce catalogue.
- Aucun nouveau code, build, test ou calibration. B5/C2 disposition acceptée ; B1 preuves acquises ; catch/close B3 restent ouverts.

## Options et compromis

1. **Autoriser uniquement la continuation RO avec historiques conservés** pour le protocole S et la recherche C. Recommandation proportionnée : les deux spécialistes connaissent déjà ces sujets, aucune indépendance de reviewer n'est revendiquée. Corriger explicitement la décision de contexte du plan37, sans modèle/score/permission modifié. L'écriture de sonde reste bloquée jusqu'à une isolation native effectivement vérifiée ; audit final fresh demeure obligatoire et inchangé. Cela permet d'avancer les deux enquêtes, pas d'autoriser un auteur canonique ou un correctif Reader.
2. **Maintenir fresh pour ces enquêtes aussi** : affectations restent suspendues jusqu'à une réaffectation native autorisée avec profils/contexte/isolation vérifiables. Aucune API/commande de réaffectation non exposée n'est inventée ; le besoin d'intervention opérateur reste précis. Plus strict sur séparation des contextes, mais bloque le protocole et la recherche sans valeur d'indépendance obligatoire ici.

## Décision exacte demandée

Autorisez-vous la continuation **lecture seule** de S/C dans leurs contextes conservés, explicitement non indépendants, écriture toujours bloquée ; ou maintenez-vous fresh en attendant la réaffectation native ? Aucun abandon d'obligation, correction Reader, nouveau provider ou élargissement de permissions dans ce choix.
