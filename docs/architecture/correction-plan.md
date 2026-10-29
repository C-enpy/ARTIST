---
title: Plan de correction de l'existant
category: architecture
tags: [artist, architecture, testing]
summary: Séquence de travaux réversibles et critères de passage des ADR vers une implémentation vérifiée.
sources: [docs/_meta/session-2026-10-26-stable-reader-cleanup.json, .opencode/missions/team_mvnxps48_0000_dfde7a/reports/21-audit-final-accepte.md, .opencode/missions/team_mvnxps48_0000_dfde7a/reports/22-bilan-final-et-dette-board.md, .opencode/missions/team_mvnxps48_0000_dfde7a/reports/25-reconciliation-native-verifiee.md]
provenance:
  extracted: 1.0
  inferred: 0.0
  ambiguous: 0.0
status: proposed
created: 2026-10-03
updated: 2026-10-29
---

# Plan de correction de l'existant

## Qualification QA Linux — 29 octobre 2026

Parité exécutée du runtime `78e7b74` : 589/589 dans six configurations Windows,
WSL et CI Ubuntu, normales/ASan. Correction Attribute compatible : clés natives
exactes, classification moderne conditionnelle et contrôle d'annonce effective
GLEW ; pas de migration API/cook ou backend. Dernier renforcement du contrôleur
`f58a49f` : shaders inclus dans l'identité, RED/GREEN Python Windows/Linux et
ré-audit indépendant. [[development/linux-qa|Procédure et portée de preuve]].
Statuts ADR inchangés ; Linux matériel, Release, UBSan, performance et fuites
exhaustives ne sont pas qualifiés.

Ce plan est dérivé des [[references/research-reports|quatre rapports]] et de
[[architecture/current-state|la photographie source]]. Ce n'est pas un calendrier
de livraison. Les durées et pourcentages des rapports ne sont pas des budgets ratifiés.

## Tranche float préparé — 2026-10-27

La tranche bornée d'ADR-0004 adoptée explicitement est présente à `3b1b102` :
préparation runtime après link/réflexion d'un float strictement scalaire, accès
emprunté révocable puis updates typés sans lookup/any_cast. API par nom et miroir
CPU conservés, consommateur target préparé/decoy par nom et oracle pixel fixe.
Runner lead : **581/581 normaux et 581/581 ASan**, chacun 506 CPU + 75 OpenGL,
mêmes noms sans failure/error/skip, canari 0/86, mutants C/L/A détectés/restaurés.
Revue de sûreté indépendante sans défaut bloquant démontré ; ré-audit final après
deux compléments de tests **PASS borné**, F1/F2 fermés, historique RED incomplet
conservé. Tranche terminée à cette portée. ADR-0004 reste **Proposed/partial**, pas d'adoption
globale, d'extension aux attributes/cook ou de performance mesurée.
Preuve `_meta/session-2026-10-27-prepared-float.json` ;
[[architecture/next-mission|aucune nouvelle mission engagée automatiquement]].
Les gaps Reader historiques ci-dessous ne sont pas rouverts par cette tranche.

## Recentrage stable — 2026-10-26, nettoyage livré

Contrat fichier stable adopté avant refactoring, amendement QA local committé
`f27f690`, candidat **a96104c** : callback et instrumentation Reader retirés,
13 fichiers probes supprimés après consommateurs contrôlés. Correctifs de lecture,
fixtures isolées et preuves historiques conservés. **555/555 normaux et555/555
ASan** (484CPU+71OpenGL chacun),0failure/error/skip ; revue C++ indépendante et
audit des preuves sans finding bloquant démontré. QA locale simplifiée, pas un
nouvel audit natif test-reviewer ni une correction du harness.

B1 : clôture bornée aux deux contaminations ON_CALL causalement identifiées,
revue finale acquise ; pas replay176. B5/C2 : fermeture administrative motivée
acceptée34, pas de chasse supplémentaire. B2/B4/B6 restent acquis à leurs candidats.
B3 : nettoyage stable terminé, poursuites stdio/close annulées NOT_RUN ; erreurs
authentiques extraction/close et observation dynamique portable de l'ordre restent
des gaps documentés, pas PASS ni essais autorisés implicitement. Ce bilan remplace
les attentes exploratoires des jalons historiques ci-dessous.

Preuve `_meta/session-2026-10-26-stable-reader-cleanup.json`,
[[architecture/current-state|photographie]] et
[[development/known-incidents|limites conservées]]. Statuts ADR inchangés :
0001/2/4/6/7 Proposed,0003/5 Accepted partial,0008 Windows Debug seulement.

## Continuation QA — 2026-10-25, terminée à sa portée bornée

Remise generator39 acceptée sans nouveau patch ; source H3 initiale318bf7a,
intégration documentaire849e6d5. La campagne ASan initiale555 a donné un FAIL
réel dans le diagnostic mock de ShaderLifetimeTests ; archive rouge conservée.
Correctif local test **82bf52d**, contrat/QA avant édition, RED86 puis GREEN1/1,
revue statique correctness-reviewer fresh reçue sans défaut bloquant démontré.
**ASan final555/555 =484CPU+71GL**,0failure/error/skip,17historiques chacun une
fois ; CPU normal484/484 réexécuté et71GL normaux antérieurs réutilisés par identité.
Voir [[development/known-incidents|incidents]],
[[development/build-and-test|preuves exécutées]] et
`_meta/session-2026-10-25-qa-continuation.json`.

Le diagnostic QA Windows/Linux est livré en analyse locale : l'oracle du harness
reste incohérent avec les trois profils communicationv2 ; les diffs concurrents
ne sont pas corrigés/intégrés par cette continuation. Linux n'est pas exécuté.
**Audit final test-reviewer fresh reçu et accepté : PASS borné O1–6, aucun finding
bloquant démontré.** Admission native dédiée selon adaptation opérateur approuvée,
profil/lectures/contexte/transport vérifiés ; worktree RO toléré, sans garantie de
sandbox ou d'invisibilité technique du board. Audit statique/mutants mentaux,
exécutions runner lead conservées sans nouvelle campagne. Le lot de continuation
est terminé à cette portée, sans clôture globale moteur/harness. F8 catch INCONCLUSIVE/close
DEFER, C2 OPEN/INCONCLUSIVE, B1 historique et GCC natif non résolus. ADR0001/2/4/6/7
Proposed,0003/5 Accepted/partial,0008 Windows Debug seulement ; pas de nouveau
backend, RAII exhaustive ou gain mesuré. Les historiques ci-dessous restent datés.

Après un refus initial, les dépendances importées ASAN→AUDIT→CLOTURE ont été
réconciliées sous verrou natif sur choix opérateur : **7/7completed**, aucun audit
relancé. Équipe conservée/non archivée, politique non approuvée sous verrou.
Voir [[references/session-2026-10-25-qa-continuation|synthèse de continuation]] pour
les preuves, la résolution du statut et les dettes techniques toujours ouvertes.

**Reprise initiale B1/B3/B5 ensuite approuvée (révision3,25octobre)** : enquêtes
parallèles nouvelles, anciennes tâches non rouvertes. B1 : deux contaminations
ON_CALL historiques isolées, tests réduits négatifs puis nominaux actuels verts,
sans nouveau correctif ; preuve `_meta/session-2026-10-25-b1-causal-validation.json`.
B3 : contrat transactionnel stdio proposé mais non adopté, calibration/modes et
catch/close à arbitrer ; B5 : provenance native du défaut de fixture retrouvée,
fermeture motivée encore soumise à l'opérateur. Le scope initial reste en cours,
sans transfert automatique des preuves ASan555 sur un futur nouveau diff.

## Mission F5 du 2026-10-15 — terminée dans son périmètre

[[architecture/missions/2026-10-15-f5-gl-diagnostics|Diagnostics GL vides et
nettoyage sûr]] : RED causal (`0xc0000409`, précondition du vecteur vide sur log
zéro), correction bornée des deux loaders et validation indépendante ;
**143/143 normaux distincts** (112 unit dont 6 nouveaux diagnostics + 31
OpenGL) et **121 ASan distincts** (112 + 9 GL lifetime), zéro échec/skip.
Preuves sur le diff intégrée au HEAD `3144beb`, committé avec provenance et
empreintes dans `b80c353` (commits auteur RED/GREEN étiquetés `f5-red`/
`f5-green`) — empreintes
`20139b4a…`, `9e26477a…`, `a91aa012…`, `daca4475…` :
[[references/session-2026-10-15-f5-gl-diagnostics|synthèse de mission]]. ADR-0003
reste Accepted/partial et Q2/Q4/Q5 restent info ; la passage de suite ouverte
(Q1, Q3, F4, F8, F10) est traitée par la sélection ci-dessous.

## Mission C « bornage contrats d'échec » — terminée dans son périmètre

[[architecture/missions/2026-10-15-failure-contracts|Archive de la mission C]] : **terminée
dans le périmètre Windows x64 Debug à `e736bba`**. Q1 et Q3 sont traités ; F4 est traité pour
l'accesseur seul ; F8 est traité partiellement ; F10 est conservé (caractérisation, pas une
correction) ; le concept `OpenGLPassFlow` vérifie l'appelabilité seulement ; `getStackTraceString`
reste ouvert (dormant). RED causal : C1 8/25 échecs, C2 11/16. Normal 165/165, ASan 143
distincts. Revue R : PASS de lecture avec réserves ; QA V : 12 PASS et 2 INCONCLUSIVE sur
14 critères. ADR-0003 reste Accepted / partial. Manifeste :
`docs/_meta/session-2026-10-15-failure-contracts-validation.json`.

Sélection initiale, conservée :

[[architecture/missions/2026-10-15-failure-contracts|Bornage des contrats d'échec des
loaders et accesseurs]] : Q1 (longueur de log aberrante, contrat + cleanup), Q3 (ruling
publication différée de l'attacher), F4 (bornes des accesseurs), F8 (erreur de
lecture fichier distincte de l'erreur de compilation) et F10 (contrat
`glUseProgram(0)`). Sélectionnée le 2026-10-15 à la demande de l'utilisateur
après PM/architect fresh ; développement réalisé le même jour (voir l'archive). L'ordre architecte :
C avant la tranche Pipeline A (figer les contrats d'échec avant le churn), puis
B (ADR-0004, Adoption explicite requise). Author C64→70/R66 et C44→70/R58 en
PM/LLM, RED causal requis, empreintes des fichiers cibles relevées et invariants
préservés. Fiche complète : [[architecture/missions/2026-10-15-failure-contracts|mission C
archivée]] ; manifeste machine : `docs/_meta/next-mission-2026-10-15-c-selection.json`.

Mission Pipeline exécutée : sélectionnée le 2026-10-16 ([[architecture/missions/2026-10-16-pipeline-ownership|Pipeline A1+A2 —
ownership du Pipeline, curseur et contexte unique — contrat préalable G-A de l'architecte]]),
**terminée dans son périmètre le 2026-10-16** : A1a→A1c, F8-SEAM, A2/F6 intégrés et vérifiés (C++ `f914d77` :
215 normales (176 unit + 39 GL) / 198 ASan sélection), H-RESTORE `1a75c75` et H-FIXTURE `95c2b9e`
vérifiés indépendamment ; QA indépendante 24 critères — 14 PASS, 3 FAIL, 7 INCONCLUSIVE, aucun PASS
global (oracle GMock préexistant reten FAIL). ADR-0003 reste Accepted / partial ;
La fiche courte d'alors ne sélectionnait aucune nouvelle mission et pointait
seulement l'amendement GMock Attribute terminé au périmètre, autorisé par
l'utilisateur le 2026-10-17 : `b9c90fd`, RED causal et contrôle négatif préservés,
revue fresh PASS, QA176CPU/176ASan indépendante conforme. La fragilité d'ordre
Loader/ShaderLoader est démontrée préexistante sous deux seeds et reste un suivi
séparé, sans nouvelle mission choisie. ADR-0004 reste une
sélection séparée, à adoption explicite. Micro-incidents latéraux traités : ASan incrémental
(H-ASan), `getStackTraceString` (S-GSS), refresh documentaire (DOC-REF), F8-EXTRACTION (seam
I0, F8 encore partial).

## Mission loaded-owner du 2026-10-15 — terminée dans son périmètre

[[architecture/missions/2026-10-15-loaded-owner-move|Assignation move vers un Shader/Pass chargé, puis ASan]] :
RED causal, correction et revue indépendante ; **137/137 normaux** (106 CPU,
31 OpenGL) et **106 CPU + 9 lifetime GL sous ASan**, sans échec ni skip.
[[references/session-2026-10-15-loaded-owner-move|Preuves et limites]] sur le diff
D+H intégré au HEAD `e589aee` (pas un nouveau commit principal). ADR-0003 reste
Accepted/partial. Baseline historique `4d29842` et 122/122 préservée.
Les deux missions du 2026-10-15 sont archivées ; Pipeline reste hors tranche et
aucune mission suivante n'est sélectionnée ici.

## 0 — Obtenir une baseline exécutable

**Réalisé sous Windows Debug le 2026-10-03** : build complet et 109/109 tests
(87 CPU, 22 OpenGL). Voir [[adr/0008-conan2-et-baseline-windows|ADR-0008]] et
[[development/build-and-test|les preuves]]. Coverage et autres plateformes
restent à qualifier. L'étape 1 peut désormais partir de cette baseline.

Appliquer le contrat de [[adr/0005-build-reproductible-et-validation|ADR-0005]] :
Conan compatible et toolchain identifiée, coverage optionnel, découverte CTest
fiable. Capturer les résultats CPU, puis les préconditions OpenGL. Un contexte
absent ne bloque pas les preuves CPU, mais laisse le critère GPU INCONCLUSIVE.

**Sortie** : commandes propres reproductibles, nombre non nul de tests, premier
écart causal documenté. Aucun changement d'architecture attribué à un simple
problème de dépendances.

## 1 — Fermer les contrats de la tranche

Choisir une petite chaîne OpenGL représentative et ses fixtures. Définir
comportement, ordre des passes, erreurs, ownership et équivalence visuelle selon
[[adr/0001-resolution-multi-etapes|ADR-0001]],
[[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]] et
[[adr/0006-preuves-performance-et-determinisme|ADR-0006]].

**Tranche Shader→Pass — réalisée le 2026-10-03** : migration franche d'ownership
(owner unique `unique_ptr` sur `IShader`/`PassContext::m_shaders`, emprunts en
références, composants GL `on(Context&)`, `Pass::shared()` supprimé). Suites :
95/95 unit + 27/27 OpenGL PASS, après les corrections de revue et le rejet des
re-loads sur handles vivants. Voir [[adr/0003-ownership-et-lifetime-cpu-gpu|ADR-0003]]
et [[development/build-and-test|les preuves]]. Restent dans la tranche : le contrat
visuel/équivalence (ADR-0006, images figées) et le relevé workload — la tranche
couvre les cas d'ownership et d'erreurs testés, pas tous les chemins de lifetime.
L'assignation vers un owner chargé et ASan sont désormais qualifiés dans le
[[references/session-2026-10-15-loaded-owner-move|périmètre Windows Debug du 2026-10-15]],
pas sur toutes les plateformes ni tous les chemins de lifetime.
Les [[development/known-incidents|findings non traités]] ne sont pas annulés par
ces suites vertes. La suite recommandée par l'avis d'architecture —
`PipelineContext::m_passes` en ownership unique, `forPass` par emprunt, contexte
de `IPass` unique — est **réalisée et vérifiée le 2026-10-16**
([[architecture/missions/2026-10-16-pipeline-ownership|mission Pipeline]]) ;
la tranche [[architecture/missions/2026-10-18-f7-uniform-attribute|F7]] est désormais
vérifiée à `6d6dcb7` (2026-10-18) : maps et contextes Uniform/Attribute propriétaires
uniques, emprunts et concept négatif UniformReader, rejet des uploads arrays via
set single-value.299natifs Windows Debug, ASan245historique T4 distinct. Les
étapes proposées ci-dessous ne sont pas automatiquement adoptées par cette clôture.

**Cadrage après F7, 2026-10-18** : la mission alors proposée, conservée dans
[[references/session-2026-10-18-attribute-gl|la rétrospective Attribute-GL]], fermait le contrat local réflexion/configuration Attribute OpenGL
(12 formes CPU canoniques, validation et cible d'upload). Le lifetime VBO reste
une tranche distincte. Analyses architecte/PM seulement, aucun RED nouveau ni
autorisation d'exécution ; ADR-0004 reste Proposed. L'ancien pointeur est
[[architecture/missions/2026-10-18-next-mission-f7-closed|sauvegardé à l'identique]].

**Lifetime OpenGL, 2026-10-19** : cette tranche distincte est exécutée et close
au périmètre Windows Debug sur `71dacbb` (466/466 unit + 64/64 OpenGL, audit V
borné COVERED). ADR-0003 reste Accepted/partial. Le pointeur clos est
[[architecture/missions/2026-10-19-next-mission-lifetime-closed|archivé]].
Voir [[references/session-2026-10-19-opengl-lifetime|la session]] et
[[architecture/missions/2026-10-19-opengl-lifetime-audit-v|l'audit V]].

**Chemin d'exécution, 2026-10-20 — exécuté et close au périmètre** :
[[architecture/missions/2026-10-19-pipeline-execution-path|la mission de l'étape 2]],
un chemin concret pour la chaîne OpenGL Classic à deux passes en coexistence avec
`IPipeline`, est livrée : type fermé `ClosedExecution<API>`, **546/546 natifs**
Windows Debug à `fdbce15` (477 unit + 69 OpenGL), RED→GREEN tracé, revue R et
audits G/V indépendants PASS, décision humaine sur topologie/coexistence
enregistrée. ADR-0002/0004/0006 restent **Proposed** ; adoption d'ADR-0002/0004
non demandée par la tranche. Preuves :
[[references/session-2026-10-20-pipeline-execution-path|session]].

**Étape 3 (ADR-0004) : candidate, NON sélectionnée** — adoption explicite
requise, décision humaine.

**Sélection du 2026-10-20** : la mission **couche documentation d'usage** est
retenue pour la session suivante
([[architecture/missions/2026-10-20-usage-doc-layer|fiche]]) — audit d'audience
du vault : mémoire d'ingénierie à jour, couche consommation absente, touchpoints
dérivés. La tranche 3 reste candidate d'après.

**Sortie** : baseline caractérisée, configurations valides/invalides, données et
workload figés ; aucune performance inventée faute de mesure.

## 2 — Introduire un chemin d'exécution spécialisé

Suivre [[adr/0002-preparation-et-execution-specialisee|ADR-0002]] : coexistence
contrôlée avec l'API existante, types concrets pour la topologie fermée, context
emprunté, dispatch externe par famille/lot si nécessaire. Migrer un consommateur,
pas toutes les primitives en une seule réécriture.

**Sortie** : mêmes comportements voulus, erreurs vérifiées, appels générés inspectés
si une suppression de dispatch est revendiquée. Les anomalies de l'ancien
comportement sont traitées explicitement, pas figées parce qu'un mock les reproduit.

## 3 — Préparer les données et bindings

Suivre [[adr/0004-layouts-types-et-donnees-preparees|ADR-0004]] : noms/réflexion à
la préparation, layouts typés et IDs validés, invalidation lors du relink/reload.
Définir les coûts autorisés lors d'une création et ceux autorisés à chaque frame.

**Sortie** : preuve de compatibilité CPU/GPU, erreurs de type et handle périmé,
profil d'allocation/lookup du chemin migré.

## 4 — Décider à partir du coût total

Comparer ancien chemin, runtime optimisé par lots et spécialisation statique
selon [[adr/0006-preuves-performance-et-determinisme|ADR-0006]]. Conserver les
données brutes, qualité, code size et coûts de build/cook. Une expérience
scene-to-code est bornée ; elle n'est pas automatiquement le produit livré.

**Sortie** : gains et régressions observés, ou INCONCLUSIVE. La question d'un
prototype Rust ne s'ouvre que sur une baseline C++ corrigée et selon
[[adr/0007-cpp23-et-cooker-independant|ADR-0007]].

## À chaque étape

- Relier le diff et les résultats à l'ADR, mettre à jour `implementation` seulement
  pour le périmètre prouvé et corriger [[architecture/current-state|l'état réel]].
- Garder un chemin de comparaison/retour ; ne retirer une compatibilité qu'après
  migration de ses consommateurs et décision explicite sur l'API publique.
- Mettre à jour [[log|le journal]] et l'index QMD selon
  [[_meta/wiki-maintenance|la procédure]].
