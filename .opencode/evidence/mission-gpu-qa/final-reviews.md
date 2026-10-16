# Revues finales indépendantes — candidat 245f1f3

Restitutions reçues le 2026-10-17, persistées et relues par le coordinateur.
Candidat des deux revues : `245f1f30e7a668619a12399b0334473ce4a1c225`, branche
`mission/gpu-qa-integration-e52f73a`, propre avant/après. Contextes fresh,
hors board d'implémentation, deux enquêtes indépendantes en parallèle.

## QA — final-qa-review

Profil réellement admis **correctness-reviewer**, méthode test-reviewer lue en
entier (230 lignes) : substitution signalée, pas activation du profil QA natif.
PM/LLM brut/retenu C78/R80. Lecture/filtrage/hachages et Git lecture seule ;
aucun candidat/test/canari/GPU/API modèle/grant/sous-agent exécuté, aucune écriture.
Diff partagé 1821af6..245f1f3 : 19 chemins, moteur/build/politique/ADR inchangés.

Recoupements indépendants :

- Plan 4287cb7, contrat 40a7cc3 et inputs R2/T13 présents dans l'ascendance ;
  intégration 0727bbc puis admission 73feaf5 et proposition T14/245f1f3.
- **16/16 blobs** du producteur T13 encore identiques, 14 empreintes source,
  5 empreintes d'entrées et 12 empreintes de logs du rapport auteur conformes ;
  43 logs auteur conservés. Base opérateur **6/6** identique au manifeste et
  attribution séparée. Profils QA, helper Git et R2 inchangés ; pas ré-audit
  exhaustif de F1–F6.
- SHA-256 et comptage TAP lead conformes : **63 + 355 + 170 = 588**, zéro fail/
  skip/cancel. Ce sont les runs du **lead sur les blobs intégrés avant commit**,
  pas une campagne rejouée par le reviewer ni 588 nouveaux tests au HEAD 245.
- Union syntax/schema/offline : **41/81/215** contrôles locaux PASS ;
  respectivement 41/41, 41/41 et 68/68 empreintes d'entrée conformes. Snapshot
  final-review-input/union-offline.json : HEAD exact 245f1f3, delta vide,
  68/68 empreintes, 215 PASS, missionVerdict INCONCLUSIVE.
- Snapshots natifs du lead au bon worktree : **30 profils résolus = 23 locaux
  + 7 natifs**, **17 skills locaux présents parmi 103 résolus**. Les QA ont
  **27 règles déclarées**, **32 résolues** (4 initiales héritées et browser deny
  final). Présence de métadonnées seulement, ni sélection/appel ni refus effectif.

G-01…07 et T-01…06 sont explicites ; G-08/T-07 inférés puis adoptés pour ce
harness. Oracles/registre recoupés dans les deux sens : correspondances/IDs,
barrières déclarées, mutations CLI causales, absence native exit2 et absence +
autre FAIL exit1, trois vrais QA forme B/static-presence. Aucun nouvel orphelin
matériel ni assertion/tolérance affaiblie ou skip masquant trouvé dans le diff.
La propriété replaceAll de T13:132 n'ajoute pas de preuve de workflow.

**Aucun finding démontré, aucune question correctrice bloquante nouvelle.**
KEEP : preuves locales/provenance/identité et boucle déclarée ; MODIFY : aucun.
REJECT : utiliser tests, lookup de policy, GET ou transport du substitut comme
preuve native/GPU ; nouveaux tests textuels redondants pour fermer ces gaps.
DEFER proposés, toujours ouverts : admission/enforcement (T-06/G-07), sélection/
appels S01–S12 et OP1–OP3 (G-03/G-04), workflow autonome T-02…05, interruptions/
publication/collecte native G-08/T-07, API/image/performance G-05/06.
Propriétaire : lead ; vrais profils/spécialistes avant revendication correspondante.
Ni acceptation humaine des gaps ni lancement de spécialiste déduits du rapport.

Risques QA inchangés, distincts de PM : permissions I5×P4=20, routage I4×P4=16,
faux vert I5×P3=15. Déclencheurs/blast radius/détection/mitigation/gaps détaillés
dans t14-proposal.md et validation-report.md. Les pins historiques e4b0aa7/
40a7cc3 ne valident pas le candidat courant. Le rapport auteur historique source
89bf66f reste correctement attribué ; Conan 2 recoupé sans build.

**PASS borné** de revue statique et recoupement des preuves locales ; aucun FAIL
nouveau démontré. Recommandation : livraison **PARTIELLE**, pas approbation globale,
pas preuve exhaustive. Ce fallback apporte une revue méthodologique indépendante,
pas la preuve des permissions du test-reviewer natif.

## Technique GPU — final-gpu-mapping

Profil **gpu-engineer**, contexte fresh, PM/LLM brut/retenu C65/R68. Lecture seule,
aucun test/build/GPU/capture/API modèle, aucune écriture. QA/parser/permissions
laissés à la revue parallèle conformément au partage de périmètre.

Les **19 associations** et minima **S01–S12** concordent entre catalogue manuel,
orchestrator:101–146, profils et skills ciblés. Frontières vérifiées : gpu-engineer
OpenGL historique ; quatre expertises, seul backend OpenGL ; compilation/ABI
distinctes du pipeline et solveur ; lifetime/hazards avant optimisation ; DDS
≠ D3D12 ; RTX ≠ capacité. Aucun second coordinateur ni gain non mesuré annoncé.
Les négatifs examinés distinguent rejet préalable, fixture API conditionnelle
et oracle image/numérique. Aucun conseil techniquement invalide confirmé.

VK-02 recipes.md:80–91 raccorde transition **et** VkRenderingAttachmentInfo à
DEPTH_STENCIL_ATTACHMENT_OPTIMAL ; D32/aspect DEPTH inchangés. Aucune contradiction
nouvelle ; Q1 original reste documentaire INCONCLUSIVE sans violation démontrée.

**PASS statique borné, aucun finding technique confirmé, aucun patch demandé.**
588 tests/41–81–215 checks restent docs/offline du lead, pas moteur. Métadonnées
≠ sélection/chargement/enforcement. S01–S12, OP1–OP3, canaris/gates NOT_RUN ;
API/image/performance INCONCLUSIVE, impact CPU/GPU non évalué. Pas de PASS global.

## Décision du coordinateur

Avis recoupés avec le candidat et les preuves intégrées. Aucune correction de
source demandée, donc aucun cycle correctif/ré-audit supplémentaire artificiel.
Clôture du développement local et livraison partielle sur la branche isolée ;
gaps natifs maintenus, sans les accepter implicitement au nom de l'utilisateur.
Les modifications suivantes sont uniquement des photographies documentaires et
preuves de clôture ; contrôles locaux à relancer sur leur diff final.
