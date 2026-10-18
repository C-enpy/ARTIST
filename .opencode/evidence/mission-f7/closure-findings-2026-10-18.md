# F7 — findings avant clôture (2026-10-18)

État : clôture retenue, pas de PASS global. Candidat moteur `18dcf31`,
barrière historique `17019e4`, tests TE intégrés `1dd7cd1`, correction test
T1-1 du lead `176d167`. Les résultats historiques ne sont pas une preuve des
corrections encore à venir.

## F1 — diagnostic AttributeContext

Le texte du manifeste T4 qui déclarait le mismatch inatteignable était faux.
Le changement du type stocké reste susceptible de lever SET::TYPE_MISMATCH ;
`typeid(m_value)` décrit std::any, pas son contenu. Correction documentaire
appliquée au manifeste, JSON parsé via Context Mode. Aucun correctif du message
C++ exécuté : le défaut diagnostique reste ouvert, pas résolu par la documentation.

## F2 — cardinalité des uploads d'uniforms

Finding fresh T5 : le Reader T3 publie désormais la taille réfléchie N, tandis
que les setters vector/matrix peuvent fournir une seule valeur à glUniform* avec
count=N. Risque de lecture hors limites si N>1. Ne pas traiter ce défaut comme
inatteignable ni le clore par report implicite à ADR-0004 (toujours Proposed).
Consultation lecture seule `f2-architect` lancée (C60/R65 PM/LLM, fresh, sans
worktree), en parallèle du ré-audit T1-1. Contrat minimal et RED sûr requis avant
implémentation ; ne pas provoquer volontairement un accès driver hors limites.

## T1-1 — oracle de lifetime des gap-tests

Proposition initiale du test-reviewer : LifeProbe avec compteur de destructions.
Exécutant : lead ; premier essai deaths==0 échoue parce que l'ancien payload de
destination est détruit au move, conformément au contrat. Premier FAIL conservé
dans `out/mission-f7/te/t1-1-patch.junit.xml`.

Correction lead `176d167` : deaths==1, puis suite native 271/271 (226 unit +45 GL),
zéro skip rapporté ; preuves `t1-1-v2.junit.xml` et `after-t11-v2.junit.xml`.
Ces succès ne clôturent pas le finding : ré-audit indépendant requis, notamment
pour le mutant combinant fuite ancien payload et destruction du transféré, et
pour l'absence d'accès après détection d'un lifetime invalide (ASSERT avant lecture).
Option soumise au reviewer : compteurs séparés pour les deux payloads.

### Correction v3 exécutée par le lead

Le reviewer a invalidé son KEEP v2 après examen du mutant combiné : fuite17 et
destruction23 peuvent satisfaire le total 1. Proposition texte corrigée appliquée
par le lead : oldDeaths/transferredDeaths séparés, ASSERT avant les lectures
post-move et post-destruction de la source. Production inchangée. Le patch garde
les contrôles identité, copies, valeur et source FREED_CONTEXT.

Commandes via Context Mode, logs complets sous `out/mission-f7/te/` :
chargement `out/tooling/load-msvc.cmd` puis build preset windows-msvc-debug
`--parallel 1` (exit0), CTest ciblé `-R MoveAssignmentTransfersLoadedContextWithoutCopyingBaseValue`
et suite `-L unit`, tous avec `--no-tests=error --output-on-failure` et JUnit absolus.
Résultats : 1/1 ciblé, 226/226 unit, zéro failure/disabled/skip, exit0.
Une erreur de syntaxe initiale du script Context Mode a été corrigée avant build ;
elle n'est ni RED ni panne d'infrastructure.

- TU testé SHA256 : `c89b9e93eb5d00b518244648b2882b8ae6bf171f7a140ed3ff898ab3b1bcad2a`.
- JUnit ciblé `t11-v3-selected.xml` : `cfebf8f8a9d85b00a401f13070fd48ac7f3de55d2c28bdb568a6d7cffde33bc7`.
- JUnit unit `t11-v3-unit.xml` : `040ed492bce8a00d6409a3a8aeea3c8cdd402206be0230271254df04511b3d7d`.

Pas de mutation fautive de production exécutée, pas de nouveau run GPU/ASan pour
v3. Ré-audit requis sur ce candidat ; aucun verdict global.

### Ré-audit final v3 — KEEP au périmètre

Rapport fresh `test-reviewer-offboard`, candidat `1db306a` : les deux compteurs
distincts et les ASSERT avant lecture sont vérifiés ligne par ligne, après move
et après destruction de la source. Le mutant combiné fuite17+destruction23
déclenche l'ASSERT sur transferredDeaths avant toute lecture du payload.
Le précédent KEEP v2 du reviewer est explicitement invalidé et remplacé par ce
verdict v3. Findings T1-1 clos : CPU mock Windows Debug, cas Loaded→Loaded.

Le reviewer a recoupé les logs et JUnit 1/1 et 226/226, zéro failure/disabled/skip.
Les hashes restent recalculés par le lead, pas par le reviewer. Analyse des
mutants par lecture seulement ; aucun mutant exécuté, aucun nouveau run GPU/ASan.
Un crash secondaire pendant l'unwind d'une implémentation mutante invalide reste
possible ; le test ne promet pas une exécution sûre d'un double-free volontaire.
Cette clôture ne ferme ni F1 (message C++) ni F2 (cardinalité d'upload).

## F2 — clôture après correction et deux revues indépendantes

Candidat corrigé et intégré `1690bac` (base d24340f). Production : garde
single-value aux34 setters OpenGL, count littéral1 aux30 sites vector/matrix,
init déterministe size1/id0/type0. Aucun Reader/Pass/Attribute/T1-1/mock modifié.
Contrat/QA commités avant worktree auteur ; RED CPU sûrs22échecs puis six oracles
count1 échouant avant patch. Aucun driver OOB volontaire. Revalidation lead
après merge :297/297 =249unit+48GL, failures0/disabled0/skipped0.

Revues fresh lecture seule `f2-correctness-review` et `f2-rendering-verifier` :
aucun FAIL du contrat F2. Sept sources/fixtures et34artefacts re-hashés et
recoupés, manifeste40d30bb9…, identité auteur/intégré inchangée. Helper placé
avant GL vérifié dans les34spécialisations,30counts littéraux1, aucun count
réfléchi résiduel. Réflexion conserve tailles arrays, stockage avant dispatch
conservé (GPU non-altération, pas rollback CPU promis).

Readbacks natifs : scalar float/vec3/mat3, tableauvec3[2] actif avec seed six
floats valide et lecture séparée des deux locations avant/après rejet ; doubles
double/dvec3/dmat2 sur extension fp64 ET entrées observées. Zéro skip observé,
28autres spécialisations INCONCLUSIVE driver (revue statique, pas couverture
native exhaustive). RTX3090, GL3.3 core/NVIDIA617.42 ; aucune portabilité fp64
générale affirmée. Pas de nouveauASan/performance/image/Release/autreOS.

Hashes du run intégré recoupés par le verifier :
- lead-integrated-native.xml : `6d989440fb57a5c1829b5180c6124594ecb43581798d7fdf6f51be2c9cab5ea0`.
- lead-integrated-native.log : `f42b191f0cfcc96e5c4ae7d519e32b55bd621685ac2af7d0b95243c56bf0b9e0`.

Nuances de preuve : ancien résumé inventory-green comptait2occurrences getGLSize
(garde/diagnostic), pas deux countsGL ; inventaire final et source=0count réfléchi.
Le nom UniformContractTests désigne les static_asserts du fichier, pas un cas
CTest de ce nom. Init directe testée après fix pour ne pas lire un indéterminé
au RED. Les revues n'ont pas réexécuté : elles recoupent les campagnes auteur et
lead distinctes. Finding F2 clos à ce périmètre, pas de PASS global de mission.

Questions préexistantes distinctes conservées : gate fp64 en production absent
sur appareil sans extension (gate actuel dans les tests seulement) ; types
unsupported rejetés à la compilation selon HasComponent ; stockages CPU conservés
après exception du Setter. Aucun de ces points n'est présenté comme corrigé.

## F1 — clôture après correction et revue indépendante

Candidat `6d6dcb7` (auteur bfd94a0, planQA113411d). Delta production strictement
deux expressions, type stocké diagnostiqué via m_value.type().name(). Type
TraceableException<runtime_error>, motif SET-in-GET historique, champs1 des
messages et freeze/payload inchangés. Deux testsCPU dédiés, noms typeid calculés
dans le même binaire. RED2/2 champ2 avant patch, GREEN2/2 et251/251unitaires.

Lead : sauvegarde25artefacts avant shutdown, hashes recoupés sans divergence,
sources auteur/postmerge identiques, re-glob/build1job puis251/251CPU sur intégré.
Revuefresh `f1-correctness-review` : aucun finding démontré, tous critères O1–O5/O7
PASS, O6empty→nullptr GAP accepté inchangé. Les25artefacts et manifeste re-hashés
par le reviewer, test inchangé RED/GREEN ; comparaison de preuves, pas sa propre
réexécution. FindingF1 clos au périmètre diagnostiqueCPU Windows Debug.

## Clôture de mission au périmètre

Run finallead du2026-10-18, candidat `6d6dcb7` :299/299 (251CPU+48OpenGL),
failure0/disabled0/skipped0, exit0. JUnit `out/mission-f7/final/native.xml`,
SHA256 `a9eee192161b405cd3dd00a39c2a02c0cb22d812e8bdb4f98d3ab236a50de575`.
ASan245àT4 reste un historique antérieur, pas un run du candidatfinal.
T1-1/F2/F1 traités et revus ; limites précédemment consignées conservées.
Archive documentaire et manifeste : docs/architecture/missions/2026-10-18-f7-uniform-attribute.md
et docs/_meta/session-2026-10-18-f7-final.json. Pas de nouvelle mission sélectionnée.

## Addendum d'archivage de session demandé par l'opérateur

Export public natif retrouvé après clôture : les deux test-engineer avaient
terminé avec des réponses locales BLOCKED (contrat execute/team_message déclaré
inaccessible), non reçues sur team_results. Le diagnostic « aucun tour exécuté »
était trop fort ; pool-7 possède aussi des lectures natives. Aucun refus d'outil
supplémentaire exécuté par l'archivage, pas de preuve sandbox ni de grant ajouté.
Voir docs/references/session-2026-10-18-f7.md et les29exports/271preuves sous
research/sessions/2026-10-18-f7/, manifestes SHA256 et exclusions explicites.

## Limites de preuve

- Aucune nouvelle campagne ASan n'est revendiquée pour les tests TE/T1-1.
- L'identité des trois TU TE auteur `f8dc61d` / intégration `1dd7cd1` a été
  attestée par diff Git vide ; elle ne remplace pas l'exécution du nouveau patch.
- Le diagnostic précédent « profil QA mort/inert » n'est pas établi par un
  transcript natif : worktree vierge et absence de message ne prouvent pas la cause.
- Les règles QA restent inchangées ; aucune autorisation ajoutée ou présumée.
