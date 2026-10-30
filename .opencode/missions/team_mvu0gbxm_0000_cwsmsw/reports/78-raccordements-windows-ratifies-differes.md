# 78 — Raccordements Windows ratifiés, écriture encore différée

Le coordinateur retient la proposition architecte complète : run direct par le
runner commun, et un unique keyword `bundle_root` pour décrire la préparation.
Le sink possède la matérialisation et l'empreinte du petit transport Windows,
sans écriture par les descriptions ni nouveau schéma public.

Le helper-run opaque n'est pas retenu pour la qualification commune : ses options
ne seraient pas visibles à l'admission du cœur et il doublerait la capture. Les
fonctions privées déjà livrées restent source-only hors de ce critère. Le helper
prepare/build sera raccordé sur les garanties existantes, pas réimplémenté comme
un framework général de commandes.

L'ordre reste strict : collecteur intégré, puis complément QA du delta commité,
puis auteur unique pour l'interface/matérialisation, puis intégration avant les
adaptateurs consommateurs. La signature du code actuel n'est pas annoncée changée
et aucun auteur ne peut commencer ce raccordement depuis une branche isolée non
intégrée. Cette séquence évite deux écrivains sur `evidence.py`.

La note ratifiée `windows-adapter-binding-v1.md` fixe les chemins privés, les
responsabilités et les invariants no-clobber/opt-in. Proposition indépendante :
`result_mvugyqsg_003r_m4rxgf`, engine-architect en lecture seule. Pas de nouvelle
ADR globale ni changement de politique modèle.

Aucun test supplémentaire, helper complet, MSVC, compilation moteur, OpenGL,
rendu, benchmark, installation ou cloud exécuté lors de cette décision.
