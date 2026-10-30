# 29 — Protocole vert, corrections natives bornées

## Résultats vérifiés

Le coordinateur a relu l'extension de cible graphique et exécuté les **29 tests du protocole : tous passent**, sans error ni skip. Les assertions sont inchangées depuis le RED sémantique. Les empreintes restent stables pendant le run.

La correction CPU emploie maintenant un tag local pour spécialiser `UniformContext`, sans nouveau backend ni changement moteur. Le diff a été relu ; le ré-audit indépendant est demandé avant levée du défaut source. Aucune compilation n'est revendiquée.

Les deux limites de détection des pixels sont conservées dans les documents du propriétaire : permutation complète de cellules identiques et absence de preuve de chaque phase intermédiaire mesurée.

## Décision locale et prochaine étape

La proposition Unicode est retenue pour les deux seuls workloads Windows : entrée `wmain` UTF-16, conversion UTF-8 stricte, chemins natifs explicites, manifeste UTF-8 et option source locale. Le Reader historique reste inchangé ; avant son utilisation GL, une précondition doit vérifier le mode effectif des API de fichiers et la page UTF-8. Son absence produit une indisponibilité explicite, pas un repli ASCII ni une modification du système.

Ce choix résout la frontière d'encodage dans le périmètre approuvé. Il n'ajoute pas de dépendance, backend, campagne ou changement moteur. Le support du manifeste est documenté Windows 1903+ ; sa présence ne prouve pas son effet sans contrôle natif futur. La correction sera livrée et révisée en source seulement.

L'auteur peut aussi raccorder les contrôles matériels natifs au protocole Python désormais vert. Le résultat brut des préconditions refusées devra être conservé et distingué d'une panne du pipeline. Aucun C++ compilé, rendu, benchmark, installation ou cloud.

## Preuves

Candidat testé `82e6cc849de2fee9555c745863d9e0bf9e226e9b`, auteur uniform-scenarios, runner coordinateur. Commande : `C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_scenarios -p test_*.py -v`.

Journaux et empreintes : `out/verification-synthetic/scenario-hardware-green-2026-10-30T00-10-40-796Z/`. Diff tests inchangé depuis `1834af6f`. Correction CPU source `72a382e1959fab80e6d83952719bf029352c6f7e` ; proposition et références Microsoft dans `ARTIST/verification/SOURCE_REVIEW_FIX_READY.md` du worktree auteur.
