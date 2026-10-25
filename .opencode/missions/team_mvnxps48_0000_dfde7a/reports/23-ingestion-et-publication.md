# 23 — Ingestion append et publication demandées

2026-10-25. Demande utilisateur explicite : wiki-ingest puis commit+push. Travail documentaire local indivisible, PM/LLM complexité28/28 et risque35/35 ; aucun développement ni worker utile. Propriétaire lead des notes/meta/journal ciblés, index Git et QMD sérialisés.

## Périmètre

La configuration globale consultée uniquement pour les deux variables wiki appartient à un autre projet ; identité portable ARTIST .wiki.json retenue, vault docs, collection artist-wiki, aucune écriture dans l'autre vault. Sources21/22 déjà intégrées, sélectionnées depuis le contexte de cette demande ; append par SHA256, absentes du manifeste local. Aucune ingestion massive de l'historique ni instruction des sources exécutée.

Une synthèse nouvelle references/session-2026-10-25-qa-continuation.md et six pages existantes réconciliées : index/log/current-state/correction-plan/build-and-test/known-incidents. Distinction centrale : livrables terminés, audit accepté, **clôture native encore bloquée**. Sources/provenance/liens ajoutés, sources historiques conservées, ADR non promus. Manifeste local ignoré mis à jour (352sources/57notes), suivi portable _meta/session-2026-10-25-qa-ingestion.json. Pas de multiplication artificielle à10–15pages pour ce delta réduit.

## Publication et contraintes

main/origin GitHub ARTIST confirmés ;19commits locaux en attente selon tracking initial, tous dans la continuation (354b53f→c840a64), examinés par sujets avant publication. Le push demandé publiera ces ancêtres en plus du nouveau commit documentaire, pas les diffs concurrents non commis. Aucun force-push, commit global, installation ou test moteur prévu. Le renommage GrepAI staged doit rester hors commit. Contrôles ciblés, indexation et résultats de push seront consignés après exécution ; ce rapport préparatoire ne prouve ni succès de publication ni clôture native.

## Contrôles avant commit/push

Contrôle Python/PyYAML réellement exécuté sur7pages : frontmatter requis, summaries≤200caractères,≤5tags, fractions de provenance≈1,140wikilinks résolus,5liens entrants vers la note nouvelle ; aucun défaut. Sources historiques des deux notes développement conservées, hashes/tailles des deux sources concordants entre portable/local/fichiers. JSON local/portable valides,352sources/57notes. Les erreurs ponctuelles d'échappement du script de contrôle ont été corrigées, sans installation ni changement d'oracle.

`git fetch origin` exit0 ; divergence origin/main...HEAD=0/19 : pas de commit distant manquant ni besoin de rebase/force. `git diff --check -- docs` exit0 (avertissement normalisation CRLF du journal conservé). `git check-ignore docs/.manifest.json` confirme ledger ignoré, non forcé en publication. Rename GrepAI toujours seul staged.

QMD ciblé : update-wiki exit0,1note créée/6actualisées/50inchangées,0removed/skipped ; embed artist-wiki exit0,91chunks/7documents. Requête MCP lex+vec avec collection/intent puis get de la synthèse56–83 : dette native et limites retrouvées, provenance inférée marquée. Aucun build/CTest/QA exécutable ajouté ou lancé. Commit ciblé puis push standard autorisés par la demande utilisateur ; preuve du push à rapporter depuis le retour Git, pas supposée ici.

## Publication effectivement vérifiée

Commit d'ingestion **2512930ad7be6a41220b59d7e62f2427690eb2c4**,9fichiers nommés uniquement. `git push origin HEAD:main` : exit0,989cb99→2512930 sur GitHub ARTIST. `git ls-remote origin refs/heads/main` a retourné ce SHA exact ; `origin/main...HEAD`=0/0 après push. Docs propres ; seul rename GrepAI préexistant toujours staged, non embarqué. Le ledger .manifest.json reste local ignoré, sa preuve portable est publiée. Les19ancêtres de continuation ont été publiés avec ce commit, sans force/rebase ni nettoyage. Cette attestation documentaire ne change ni les preuves moteur ni la clôture native toujours bloquée.
