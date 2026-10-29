# Audit complet des ADR — conclusions vérifiées

## Ce qui a été décidé

L'utilisateur demandait de revoir réellement les huit ADR, au lieu de conserver
leurs états par prudence d'ingestion. L'audit est terminé à son périmètre
documentaire : décisions, migrations et critères ont chacun une disposition dans
`docs/references/adr-audit-2026-10-29.md`, avec preuves, conditions et limites.

Les décisions d'adoption et la maturité d'implémentation sont séparées :

- **0001 : Proposed/partial**, corrigé depuis not-started. Sélection compile-time,
  topologie N=2 et préparation float existent ; pas de résolution globale prouvée.
- **0002 : Proposed/partial**, confirmé et corrigé. Le type N=2 garde deux
  emprunts IPass polymorphes ; les bindings float sont partiellement livrés,
  contrairement à l'affirmation ancienne de leur absence totale.
- **0003 : Accepted/partial**, confirmé avec un manque précis de preuve :
  échecs authentiques d'extraction/fermeture du Reader non tous exercés.
- **0004 : Proposed/partial**, confirmé. Seule la tranche float est adoptée ;
  coût d'allocation non mesuré, pas de layouts ou bindings Attribute généralisés.
- **0005 : Accepted/partial**, confirmé pour le critère coverage ON exécutée
  ou son rejet ciblé exécuté. Le guard en source ne prouve pas son exécution.
- **0006 : Proposed/not-started**, confirmé pour le protocole comparatif de
  performance ; baselines de tests et oracle pixel sont des prérequis, pas une mesure.
- **0007 : Proposed/partial**, corrigé depuis not-started. La correction C++23 est
  réalisée ; les clauses conditionnelles d'un futur cooker ne sont pas des chantiers lancés.
- **0008 : Accepted/verified**, confirmé sur la baseline Windows Debug adoptée,
  sans fusion rétroactive avec les preuves Linux ultérieures.

Une plateforme future, Release, un GPU matériel Linux, un cooker ou une RAII
exhaustive ne sont pas inventés comme conditions de validation des tranches.
Aucune nouvelle adoption globale ne se déduit du seul fait qu'une tranche existe.

## Revue et traçabilité

Deux enquêtes RO ont été reçues, recoupées puis intégrées au candidat documentaire
`c49fb84`. Le collecteur d'équipe n'avait pas récupéré leurs finales ; l'API native
les a restituées, sans relance. Sources publiques/hashes dans
`.opencode/evidence/adr-audit-2026-10-29/{inputs,public-deliveries}.json`.

Contrelecture **correctness-reviewer fresh**, candidat c49fb84 : **KEEP — PASS de
lecture documentaire borné**, aucun défaut bloquant ni obligation originale omise
démontré. Ni finales des enquêteurs ni leurs rapports consultés par le reviewer.
La finale publique a été reçue et conservée verbatim dans `review.md` ; hash du
texte natif `336a3dbe54c1faa7fec6fff8de736619e729f17c9f8e886bdfbd7b109d489bd7`.
Les clauses et les huit ADR sont inchangés depuis ce candidat ; seuls attestation,
journal et rapport de clôture sont ajoutés ensuite. Aucun ré-audit de fond requis.

Les deux tâches d'enquête ont été clôturées après intégration réelle. La revue
est acceptée après persistance de sa remise. Les deux anciennes tâches Linux
deferred/inactives restent inchangées : aucune completion fictive ou réadmission.

## Contrôles et commandes exécutés

Runner lead, distinct des enquêteurs et du reviewer :

- `python -B .tmp/check-adr-audit.py` (interpréteur C:/Python313/python.exe) :
  **463 assertions documentaires**, 220 liens de fichiers, 106 dispositions uniques,
  états/origines/dates, YAML/JSON/sources, intégrité des finales et absence de
  changement des conclusions revues. Ces compteurs ne sont pas des tests moteur
  ni une preuve automatique d'exhaustivité ; la contrelecture vérifie les clauses.
- Comparaisons Git limitées : runtime/fixtures/build inchangés depuis78e7b74,
  scripts contrôlés inchangés depuisf58a49f ; `git diff --check` propre.
- `node .opencode/scripts/update-wiki.mjs` et `qmd embed -c artist-wiki` :
  candidat 1 ajout/13 mises à jour ; après attestation, 2 mises à jour/60 inchangés,
  38 fragments vectorisés, zéro retrait/skip. Collection ARTIST seule.
- MCP QMD : recherche de l'audit et du critère coverage, lecture des états
  0001/0007 puis de la section de contrelecture finale effectivement réussies.

Preuves locales : `out/logs/adr-audit/`, dont `document-checks.json`, identité/diff
du candidat et logs QMD. Manifeste local ignoré actualisé avec hashes des sources.
Scripts ponctuels et anciens temporaires restent sous `.tmp/`, hors publication.

## Portée et limites restantes

Aucune nouvelle compilation, exécution moteur/CI, image, mesure GPU ou benchmark.
Les résultats589/10/634 restent attachés à leurs candidats, runners et frontières.
Le verdict de revue ne certifie ni chaque artefact historique, ni l'enforcement
natif, ni la correction exhaustive du moteur.

Le contrôle d'ancienne empreinte de la configuration GrepAI privée a divergé :
horodatage d'index plus récent observé, ancienne configuration complète non
archivée ; identité octet historique INCONCLUSIVE, pas de réparation hors mandat.
Elle reste ignorée/non suivie et la clé locale est exclue des contenus publiables.
Ce contrôle partiel ne devient pas un PASS global d'intégrité locale.

Publication sur main par commit documentaire, fetch et push normal sans force ;
la vérification des références distantes et des chemins publiés est conservée
sous `out/logs/adr-audit/publication-final.json`. Marqueur skip-ci : aucune source
exécutable ne change. Cette clôture ne sélectionne aucune mission d'implémentation.
