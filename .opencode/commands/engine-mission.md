---
description: Planifie avec le PM, évalue les tâches puis coordonne leur exécution parallèle, TDD et QA indépendante.
agent: orchestrator
subagent: false
---

Prends en charge la mission $ARGUMENTS. Si l'objectif manque, demande-le avant
de choisir un travail arbitraire. Applique AGENTS.md et le contrat de
project-manager : découpage, ownership, dépendances, scores et critères observables.

Le mode d'évaluation est PM/LLM par défaut ; `evaluation=jev` demande JEV direct,
`evaluation=llm` choisit explicitement le PM. Une demande de plan seul s'arrête
après le plan. Sinon, valide et publie les tâches évaluées avec opencode-ensemble,
en respectant le plancher développement à 70, le routage opérateur et les contextes
explicites. Exploite les branches parallèles sans conflit et organise l'intégration
avant leurs consommateurs. Conduis les tranches de développement en TDD, puis la
QA adaptée ; rapporte les preuves et limites du résultat intégré.
