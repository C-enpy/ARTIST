# 21 — Deux compléments de l'audit compilés et passants

Les propositions F1/F2 réelles du test-engineer ont été reçues via les finales
natives de la même session, lues intégralement et persistées avant application.
Le lead a appliqué les deux tests dans PreparedFloatUniformTests.cpp seulement.
Aucun header moteur, fixture globale, script ou CMake modifié.

Build unitaire ciblé : exit0. Les deux nouveaux tests sont découverts, exécutés
et passants, sans skip : contexte nul/autre refusé pendant préparation avant
requête native, et révocation observée dès le premier détachement puis après
échec partiel. Cleanup des owners restaurés conservé. Aucune réparation du patch
nécessaire : budget healer inchangé à1/2.

Propositions : test-engineer pool-5, session ses_e824039f2ffeU9dNg3WyIL1eV5,
finale msg_17dc3717c001rH2QUeETm3rSLD puis confirmation msg_17dc44e2d001HPuyFsUgey0VBB.
Exécutant réel : lead. Artifacts out/qa-prepared-float/lead-followup-20261027/,
01-nominal-build et02-nominal-focused (commandes, logs, JUnit unique).

Les mutants F1/F2, restaurations, campagne complète renouvelée et ré-audit restent
à produire. Le nominal vert seul n'est pas déclaré fermeture des findings.
[Oracles retenus](../qa-followup-proposals.md) ·
[Audit initial](20-audit-deux-frontieres-negatives-a-completer.md).
