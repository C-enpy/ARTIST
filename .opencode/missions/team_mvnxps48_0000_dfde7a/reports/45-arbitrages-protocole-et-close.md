# 45 — Protocoles intégrés à examiner : TDD/calibration et close

2026-10-26. Étape : enquêtes RO S/C terminées et remises, aucune source de sonde ni essai exécuté.

## Nouveaux éléments

S7/7 reçu au44 : M0texte/brut, M1open→verrou→témoin33→fread snapshot avant cleanup, M2recoveryfluxneuf et cleanup possédé. Proposition TDD réelle limitée : adapter nativeRead hérité, actuellement un seul ReadFile8192, au nouveau besoin exhaustif12325. Test d'abord inchangé sur helper hérité :12325→8192 attendu rouge avec lecture réussie, témoins0/4096/8192 verts ; ensuite boucle/GREEN mêmes assertions. L'ancien usage4608 n'était pas fautif. Le reste de l'observationCRT est exploratoire et ne doit pas fabriquer RED pour imposer une réponse favorable.

C3/3 intégré ae14798/rapport43 : hypothèse bufferentrée/seek non soutenue par sourceUCRT26100 lue ; pas de close authentique identifié. Identité DLLexécutée et conditionOSde refus/récupération restent manquantes. Pas impossibilité universelle ni obligation close traitée par preuve. Piste distincte protection de handle valide signalée mais non étudiée/autorisée.

Source82bf52d inchangée ; isolation auteur toujours bloquante, aucun code autorisé par le seul GO RO41. Futur Reader/messages/modes/catch non adoptés. B1preuves acquises, B2/B4/B6non rejoués, C2dispositionacceptée.

## Choix recommandés et compromis

1. **Adopter le protocole de calibration44**, avec tranche TDD nativeRead exhaustif et observationsCRT explicitement caractérisées. Recommandé : helper est nécessaire à l'oracle brut complet, pas ajout artificiel pour afficher du rouge. Ne pas étendre cette preuveTDD à toute l'instrumentation ni au moteur ; contrôles/identités/cleanup restent à valider et REDReader toujours nécessaire avant correctif. Pas de source avant isolation ; CLI/codes et commandes complètes à figer avant auteur/runner. Alternative : rester RO et demander autre conception sans ce helper, sans prétendre RED disponible.
2. **Autoriser une étude documentaire RO bornée de HANDLE_FLAG_PROTECT_FROM_CLOSE**, comme nouvelle hypothèse distincte, pas l'application du flag ou un essai. Recommandé pour évaluer la dernière piste concrète avant disposition résiduelle : vérifier qu'il s'agit d'un refusOS sur handle valide, sa propagationCRT et surtout récupération sûre après FILE*consommé. Son admissibilité face à l'interdiction des flags fabriquant preuve serait soumise séparément AVANT expérience ; aucune seam/flag appliqué implicitement. Alternative : accepter dès maintenant une disposition résiduelle motivée sur M5, avec absence de preuve/impact/propriétaire/condition de réouverture explicitement archivés ; pas «close couvert».

## Décisions exactes

- Adopter44 avec cette séparation TDD/caractérisation, ou maintenir RO pour autre conception ? Ce choix n'autorise pas l'écriture tant que l'isolation native n'est pas vérifiée.
- Autoriser la seule nouvelle étude documentaire de protection de handle, ou accepter explicitement la disposition résiduelle motivée M5 ? Aucun test, application de flag, correction Reader ou abandon implicite d'autre obligation n'est autorisé par ces choix.
