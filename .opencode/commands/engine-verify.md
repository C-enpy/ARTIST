---
description: Vérifie indépendamment un changement avec verdicts gradués et commandes reproductibles.
agent: rendering-verifier
subagent: true
---

Vérifie $ARGUMENTS ; si vide, le changement courant et ses critères observables.
Charge artist-build-validation. Choisis le niveau de preuve requis, confirme les
préconditions et exécute les contrôles disponibles. Ne modifie ni sources ni
références. Donne PASS/FAIL/INCONCLUSIVE par critère, commandes, nombres de tests
et limites. Zéro test ou un contexte GPU absent ne vaut jamais PASS.
