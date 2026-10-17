---
description: Développe ou corrige par tranches RED/GREEN/REFACTOR avec planification PM, routage et QA adaptés.
agent: orchestrator
subagent: false
---

Développe ou corrige $ARGUMENTS en suivant le cycle TDD d'AGENTS.md. Si le
comportement attendu manque, demande-le. Utilise le même contrat de mission que
`/engine-mission` : PM/LLM par défaut (`evaluation=jev` pour JEV direct), tâches
évaluées avant publication, complexité de développement au moins 70, routage
Ensemble et branches indépendantes en parallèle.

Pour une nouvelle mission Ensemble, passe d'abord par `/ensemble-mission <objectif>`
puis par la présentation `/ensemble-mission config` et le `start` natif de
l'utilisateur. Ce raccourci ne crée pas d'équipe legacy pour éviter l'approbation,
ne lance pas `start` à sa place et ne lève pas une pause opérateur. Tant que la
politique n'est pas approuvée, prépare seulement le contrat et les preuves à produire :
aucun worker d'exécution ni modification de source. Une mission legacy existante
conserve son état et sa politique, sans conversion implicite.

Définis les critères, constate RED pour la bonne raison avant l'implémentation,
obtiens GREEN puis refactore si utile. Charge artist-build-validation avant les
builds/tests. Fais intégrer les livrables avant la QA et les consommateurs ;
termine avec les preuves RED/GREEN, la non-régression et les limites réelles.
